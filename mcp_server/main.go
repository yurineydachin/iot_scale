package main

import (
	"crypto/tls"
	"crypto/x509"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os"
	"time"

	iotpb "iot_scale/proto"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/google/uuid"
	"google.golang.org/protobuf/encoding/protojson"
)

type Config struct {
	BrokerURL  string `json:"broker_url"`
	CertFile   string `json:"cert_file"`
	KeyFile    string `json:"key_file"`
	RootCAFile string `json:"root_ca_file"`
	ClientID   string `json:"client_id"`
}

var (
	mqttClient mqtt.Client
	config     Config
)

func loadConfig(path string) error {
	file, err := os.ReadFile(path)
	if err != nil {
		return fmt.Errorf("failed to read config file: %w", err)
	}
	if err := json.Unmarshal(file, &config); err != nil {
		return fmt.Errorf("failed to unmarshal config: %w", err)
	}
	return nil
}

func createTLSConfig() (*tls.Config, error) {
	rootCA, err := os.ReadFile(config.RootCAFile)
	if err != nil {
		return nil, fmt.Errorf("failed to read root CA file: %v", err)
	}
	cert, err := tls.LoadX509KeyPair(config.CertFile, config.KeyFile)
	if err != nil {
		return nil, fmt.Errorf("failed to load client key pair: %v", err)
	}
	certPool := x509.NewCertPool()
	if !certPool.AppendCertsFromPEM(rootCA) {
		return nil, fmt.Errorf("failed to parse root CA certificate")
	}
	return &tls.Config{
		RootCAs:            certPool,
		Certificates:       []tls.Certificate{cert},
		InsecureSkipVerify: false,
	}, nil
}

func connectMQTT() (mqtt.Client, error) {
	tlsConfig, err := createTLSConfig()
	if err != nil {
		return nil, err
	}
	opts := mqtt.NewClientOptions()
	opts.AddBroker(config.BrokerURL)
	opts.SetClientID(config.ClientID)
	opts.SetTLSConfig(tlsConfig)
	client := mqtt.NewClient(opts)
	if token := client.Connect(); token.Wait() && token.Error() != nil {
		return nil, token.Error()
	}
	return client, nil
}

func sendCommandHandler(w http.ResponseWriter, r *http.Request) {
	deviceID := r.URL.Query().Get("device_id")
	if deviceID == "" {
		http.Error(w, "device_id parameter is required", http.StatusBadRequest)
		return
	}
	paramName := r.URL.Query().Get("param")
	if paramName == "" {
		http.Error(w, "param parameter is required", http.StatusBadRequest)
		return
	}
	paramValue := r.URL.Query().Get("value")

	cmdPayload := &iotpb.CommandPayload{}
	if paramValue != "" {
		cmdPayload.Command = &iotpb.CommandPayload_SetParams{SetParams: &iotpb.CmdSetParams{
			Params: map[string]string{paramName: paramValue},
		}}
	} else {
		cmdPayload.Command = &iotpb.CommandPayload_GetParams{GetParams: &iotpb.CmdGetParams{
			Param: []string{paramName},
		}}
	}

	cmd := &iotpb.Command{
		ChainId: uuid.New().String(),
		Payload: cmdPayload,
	}

	validUntil := uint64(time.Now().Add(5 * time.Minute).Unix())
	pkt := &iotpb.Packet{
		Version:    0x00010000,
		Timestamp:  uint64(time.Now().Unix()),
		ValidUntil: &validUntil,
		What:       &iotpb.Packet_Command{Command: cmd},
	}

	if mqttClient != nil && mqttClient.IsConnected() {
		topic := fmt.Sprintf("$devices/%s/commands/command", deviceID)
		log.Printf("Sending command to device %s: %s=%s", deviceID, paramName, paramValue)

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
		log.Printf("Successfully sent command to device %s (topic: %s)", deviceID, topic)
		fmt.Fprintf(w, "Command sent to device %s", deviceID)
	} else {
		http.Error(w, "MQTT client not connected", http.StatusServiceUnavailable)
	}
}

func main() {
	log.Println("Starting MCP Server...")

	if err := loadConfig("config.json"); err != nil {
		log.Fatalf("Failed to load config: %v", err)
	}

	var err error
	mqttClient, err = connectMQTT()
	if err != nil {
		log.Fatalf("Failed to connect to MQTT broker: %v", err)
	}
	log.Println("Successfully connected to MQTT broker")

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		fmt.Fprintf(w, "MCP Server is running!")
	})
	http.HandleFunc("/send_command", sendCommandHandler)

	log.Println("MCP Server listening on port 8082")
	if err := http.ListenAndServe(":8082", nil); err != nil {
		log.Fatalf("Failed to start server: %v", err)
	}
}
