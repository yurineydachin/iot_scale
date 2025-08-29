package main

import (
	"crypto/tls"
	"crypto/x509"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"time"

	iotpb "iot_scale/proto"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/google/uuid"
	"google.golang.org/protobuf/encoding/protojson"
)

const (
	brokerURL     = "ssl://mqtt.cloud.yandex.net:8883"
	certFile      = "credentials/cert.pem"
	keyFile       = "credentials/key.pem"
	rootCAFile    = "credentials/rootCA.crt"
	logFile       = "logs/bike_control.log"
	clientID      = "bike_control_server"
	reconnectWait = 5 * time.Second
)

var (
	mqttClient mqtt.Client
	connected  = false
)

func monitorConnection() {
	for {
		if !connected || !mqttClient.IsConnected() {
			log.Println("Attempting to (re)connect to MQTT broker...")
			client, err := connectMQTT()
			if err != nil {
				log.Printf("Connection failed: %v. Retrying in %v", err, reconnectWait)
				time.Sleep(reconnectWait)
				continue
			}
			mqttClient = client
			connected = true
			log.Println("Successfully connected to MQTT broker")
		}
		time.Sleep(reconnectWait)
	}
}

func createTLSConfig() (*tls.Config, error) {
	// Load root CA certificate
	rootCA, err := os.ReadFile(rootCAFile)
	if err != nil {
		return nil, fmt.Errorf("failed to read root CA file: %v", err)
	}

	// Load client certificate and key
	cert, err := tls.LoadX509KeyPair(certFile, keyFile)
	if err != nil {
		return nil, fmt.Errorf("failed to load client key pair: %v", err)
	}

	// Create certificate pool and add root CA
	certPool := x509.NewCertPool()
	if !certPool.AppendCertsFromPEM(rootCA) {
		return nil, fmt.Errorf("failed to parse root CA certificate")
	}

	return &tls.Config{
		RootCAs:            certPool,
		Certificates:       []tls.Certificate{cert},
		InsecureSkipVerify: false, // Enforce certificate verification
	}, nil
}

func connectMQTT() (mqtt.Client, error) {
	connected = false
	tlsConfig, err := createTLSConfig()
	if err != nil {
		return nil, err
	}

	opts := mqtt.NewClientOptions()
	opts.AddBroker(brokerURL)
	opts.SetClientID(clientID)
	opts.SetTLSConfig(tlsConfig)

	client := mqtt.NewClient(opts)
	if token := client.Connect(); token.Wait() && token.Error() != nil {
		return nil, token.Error()
	}

	return client, nil
}

// enableCORS adds CORS headers to responses
func enableCORS(next http.HandlerFunc) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Access-Control-Allow-Origin", "*")
		w.Header().Set("Access-Control-Allow-Methods", "POST, GET, OPTIONS")
		w.Header().Set("Access-Control-Allow-Headers", "Content-Type")
		next(w, r)
	}
}

func handler(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path == "/" {
		http.ServeFile(w, r, "control.html")
		return
	}
	fmt.Fprintf(w, "Bike Control HTTP Handler is running!")
}

func sendBikeCommand(w http.ResponseWriter, r *http.Request, paramName string, paramValue string, successMsg string) {
	/*if r.Method != http.MethodPost {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}*/

	bikeID := r.URL.Query().Get("bike_id")
	if bikeID == "" {
		http.Error(w, "bike_id parameter is required", http.StatusBadRequest)
		return
	}

	// Create Command payload
	cmdPayload := &iotpb.CommandPayload{}
	if paramValue != "" {
		// Create SetParams command
		cmdPayload.Command = &iotpb.CommandPayload_SetParams{SetParams: &iotpb.CmdSetParams{
			Params: map[string]string{paramName: paramValue},
		}}
	} else {
		// Create SetParams command
		cmdPayload.Command = &iotpb.CommandPayload_GetParams{GetParams: &iotpb.CmdGetParams{
			Param: []string{paramName},
		}}
	}

	// Create full Command
	cmd := &iotpb.Command{
		ChainId: uuid.New().String(),
		Payload: cmdPayload,
	}

	// Create Packet to encapsulate command
	validUntil := uint64(time.Now().Add(5 * time.Minute).Unix())
	pkt := &iotpb.Packet{
		Version:    0x00010000, // Version 1.0.0
		Timestamp:  uint64(time.Now().Unix()),
		ValidUntil: &validUntil,
		What:       &iotpb.Packet_Command{Command: cmd},
	}

	// Publish via MQTT
	if mqttClient != nil && mqttClient.IsConnected() {
		topic := fmt.Sprintf("$devices/%s/commands/command", bikeID)
		log.Printf("Sending command to bike %s: %s=%s", bikeID, paramName, paramValue)

		// Log packet in JSON format
		jsonData, err := protojson.Marshal(pkt)
		if err != nil {
			log.Printf("Failed to marshal packet to JSON: %v", err)
		} else {
			log.Printf("MQTT packet JSON: %s", string(jsonData))
		}

		token := mqttClient.Publish(topic, 1, false, jsonData)
		token.Wait()
		if token.Error() != nil {
			log.Printf("Failed to publish MQTT message to %s: %v", topic, token.Error())
			http.Error(w, "Failed to send command", http.StatusInternalServerError)
			return
		}
		log.Printf("Successfully sent command to bike %s (topic: %s)", bikeID, topic)
	} else {
		http.Error(w, "MQTT client not connected", http.StatusServiceUnavailable)
		return
	}

	fmt.Fprintf(w, successMsg, bikeID)
}

func bikeLockHandler(w http.ResponseWriter, r *http.Request) {
	sendBikeCommand(w, r, "vehicle_lock", "locked", "Lock command sent to bike %s")
}

func bikeUnlockHandler(w http.ResponseWriter, r *http.Request) {
	sendBikeCommand(w, r, "vehicle_lock", "unlocked", "Unlock command sent to bike %s")
}

func batteryUnlockHandler(w http.ResponseWriter, r *http.Request) {
	sendBikeCommand(w, r, "battery_lock_status", "unlocked", "Battery unlock command sent to bike %s")
}

func bikeCommandHandler(w http.ResponseWriter, r *http.Request) {
	paramName := r.URL.Query().Get("paramName")
	if paramName == "" {
		http.Error(w, "paramName parameter is required", http.StatusBadRequest)
		return
	}
	paramValue := r.URL.Query().Get("paramValue")

	sendBikeCommand(w, r, paramName, paramValue, fmt.Sprintf("Command with param %s and value %s", paramName, paramValue) + " sent to bike %s")
}

func main() {
    log.Println("Running")
    // Set up logging to both file and console
    logFile, err := os.OpenFile(logFile, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
    if err != nil {
	log.Fatal("Failed to open log file:", err)
    }
    defer logFile.Close()

    multiWriter := io.MultiWriter(logFile)
    log.SetOutput(multiWriter)
    log.SetFlags(log.LstdFlags | log.Lshortfile)

    log.Println("Initializing Bike Control server...")
    log.Println("Log file:", logFile)
    log.Println("MQTT Broker:", brokerURL)

    // Start MQTT connection monitor
    go monitorConnection()

    // Set up HTTP handlers with CORS
    http.HandleFunc("/", handler)
    http.HandleFunc("/bike/lock", enableCORS(bikeLockHandler))
    http.HandleFunc("/bike/unlock", enableCORS(bikeUnlockHandler))
    http.HandleFunc("/bike/battery_unlock", enableCORS(batteryUnlockHandler))
    http.HandleFunc("/bike/command", enableCORS(bikeCommandHandler))

    log.Println("Registered HTTP handlers")
    log.Println("Bike Control server starting on port 8081...")

    server := &http.Server{
	Addr:     ":8081",
	Handler:  nil,
	ErrorLog: log.New(multiWriter, "HTTP: ", log.LstdFlags),
    }

    err = server.ListenAndServe()
    if err != nil {
	log.Fatal("HTTP server failed:", err)
    }
}
