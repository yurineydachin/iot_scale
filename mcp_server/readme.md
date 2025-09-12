# MCP Server

This document describes how to use the `mcp-server` application.

The server communicates over HTTP and uses an MQTT broker to send commands to a device.

## Running the server

To run the server, execute the following command:

```bash
go run main.go
```

The server will start on `http://localhost:8080`.

## Configuration

The server requires a `config.json` file in the same directory with the following structure:

```json
{
  "broker_url": "tls://mqtt.cloud.yandex.net:8883",
  "cert_file": "../credentials/cert.pem",
  "key_file": "../credentials/key.pem",
  "root_ca_file": "../credentials/rootCA.crt",
  "client_id": "mcp_server_http",
  "device_id": "your-device-id"
}
```

## API Endpoints

The server exposes the following HTTP endpoints based on the `mcp-go-sdk`:

### `GET /tools`

Lists the available tools.

**Example response:**
```json
[
    {
        "name": "lock",
        "description": "Lock the device"
    },
    {
        "name": "unlock",
        "description": "Unlock the device"
    },
    {
        "name": "send_command",
        "description": "Get a parameter from the device",
        "parameters": {
            "type": "object",
            "properties": {
                "command": {
                    "type": "string",
                    "description": "The parameter to get"
                }
            }
        }
    }
]
```

### `POST /tools/:tool_name/call`

Calls a specific tool. The arguments for the tool are passed as a JSON body.

**Example for `lock`:**
```bash
curl -X POST http://localhost:8080/tools/lock/call
```

**Example for `send_command`:**
```bash
curl -X POST -H "Content-Type: application/json" \
  -d '{"command": "vehicle_status"}' \
  http://localhost:8080/tools/send_command/call
```

## Tools

The server exposes the following tools:

### `lock`

Locks the device. This sends a `set_params` command via MQTT with `vehicle_lock: "locked"`.

### `unlock`

Unlocks the device. This sends a `set_params` command via MQTT with `vehicle_lock: "unlocked"`.

### `send_command`

Gets a parameter from the device. This sends a `get_params` command via MQTT.

**Parameters:**

*   `command` (string, required): The name of the parameter to get from the device.