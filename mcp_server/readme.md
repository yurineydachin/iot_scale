# MCP Server

This document describes how to use the `mcp-server` application.

The server communicates over standard input/output (stdio) and uses an MQTT broker to send commands to a device.

## Running the server

To run the server, execute the following command:

```bash
go run main.go
```

## Configuration

The server requires a `config.json` file in the same directory with the following structure:

```json
{
  "broker_url": "tls://mqtt.cloud.yandex.net:8883",
  "cert_file": "../credentials/cert.pem",
  "key_file": "../credentials/key.pem",
  "root_ca_file": "../credentials/rootCA.crt",
  "client_id": "mcp_server_stdio",
  "device_id": "your-device-id"
}
```

## Tools

The server exposes the following tools:

### `lock`

Locks the device.

### `unlock`

Unlocks the device.

### `send_command`

Sends a command to the device.

**Parameters:**

*   `command` (string, required): The command to send.