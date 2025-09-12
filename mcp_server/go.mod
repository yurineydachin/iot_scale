module mcp_server

go 1.23.11

replace iot_scale => ../

require (
	github.com/eclipse/paho.mqtt.golang v1.5.0
	github.com/google/uuid v1.6.0
	google.golang.org/protobuf v1.36.9
	iot_scale v0.0.0-00010101000000-000000000000
)

require (
	github.com/gorilla/websocket v1.5.3 // indirect
	github.com/harriteja/mcp-go-sdk v0.0.0-20250610095401-8f92c15c5398 // indirect
	golang.org/x/net v0.27.0 // indirect
	golang.org/x/sync v0.7.0 // indirect
)
