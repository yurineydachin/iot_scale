package main

import (
	"context"
	"crypto/tls"
	"crypto/x509"
	"encoding/json"
	"fmt"
	"log"
	"os"
	"time"

	"github.com/harriteja/mcp-go-sdk/pkg/server"
	"github.com/harriteja/mcp-go-sdk/pkg/server/transport/stdio"
	"github.com/harriteja/mcp-go-sdk/pkg/types"

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
	DeviceID   string `json:"device_id"`
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

	s, err := server.New(&server.Options{
		Name:    "mcp-server-with-mqtt",
		Version: "1.0.0",
	})
	s.OnListTools(func(ctx context.Context) ([]types.Tool, error) {
		return []types.Tool{
			{
				Name:        "lock",
				Description: "Lock the device",
			},
			{
				Name:        "unlock",
				Description: "Unlock the device",
			},
			{
				Name:        "send_command",
				Description: "Send a command to the device",
				Parameters: &types.Parameters{
					Type: "object",
					Properties: map[string]types.Parameter{
						"command": {Type: "string", Description: "The command to send"},
					},
				},
			},
		}, nil
	})

	s.OnCallTool(func(ctx context.Context, name string, args map[string]interface{}) (interface{}, error) {
		paramName, paramValue := "", ""
		switch name {
		case "lock":
			paramName = "vehicle_lock"
			paramValue = "locked"
		case "unlock":
			paramName = "vehicle_lock"
			paramValue = "unlocked"
		case "send_command":
			paramName = args["command"].(string)
			if paramName == "" {
				return nil, fmt.Errorf("command param is requied")
			}
			// For send_command, we might not have a value
		default:
			return nil, fmt.Errorf("unknown tool")
		}

		return sendCommand(paramName, paramValue)
	})
	if err != nil {
		log.Fatalf("failed to create server: %v", err)
	}

	// Create stdio transport
	transport := stdio.New(s, stdio.Options{})

	// Start the server
	log.Printf("Starting stdio server")
	if err := transport.Start(); err != nil {
		log.Printf("Failed to serve: " + err.Error())
	}
}

func sendCommand(paramName, paramValue string) (interface{}, error) {
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
		topic := fmt.Sprintf("$devices/%s/commands/command", config.DeviceID)
		log.Printf("Sending command to device %s: %s=%s", config.DeviceID, paramName, paramValue)

		jsonData, err := protojson.Marshal(pkt)
		if err != nil {
			log.Printf("Failed to marshal packet to JSON: %v", err)
			return nil, fmt.Errorf("Failed to marshal packet to JSON")
		}

		token := mqttClient.Publish(topic, 1, false, jsonData)
		token.Wait()
		if token.Error() != nil {
			log.Printf("Failed to publish MQTT message to %s: %v", topic, token.Error())
			return nil, fmt.Errorf("Failed to send command")
		}
		log.Printf("Successfully sent command to device %s (topic: %s)", config.DeviceID, topic)
		return "Command sent", nil
	}

	return nil, fmt.Errorf("MQTT client not connected")
}
