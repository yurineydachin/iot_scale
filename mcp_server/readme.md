# MCP Server

This document describes how to use the HTTP server in `main.go`.

## Running the server

To run the server, execute the following command:

```bash
go run main.go
```

The server will start on port 8082.

## Configuration

The server requires a `config.json` file in the same directory with the following structure:

```json
{
  "broker_url": "tls://mqtt.cloud.yandex.net:8883",
  "cert_file": "../credentials/cert.pem",
  "key_file": "../credentials/key.pem",
  "root_ca_file": "../credentials/rootCA.crt",
  "client_id": "mcp_server_http"
}
```

## Endpoints

### `GET /`

This endpoint can be used to check if the server is running.

**Response:**

```
MCP Server is running!
```

### `GET /send_command`

This endpoint is used to send a command to a device.

**Query Parameters:**

*   `device_id` (required): The ID of the device.
*   `param` (required): The name of the parameter to set or get.
*   `value` (optional): The value of the parameter to set. If this parameter is not provided, the server will send a "get" command.

**Example (Set Parameter):**

To set the `report_period` parameter to `10` for device `my-device-id`, use the following request:

```
http://localhost:8082/send_command?device_id=my-device-id&param=report_period&value=10
```

**Example (Get Parameter):**

To get the value of the `report_period` parameter for device `my-device-id`, use the following request:

```
http://localhost:8082/send_command?device_id=my-device-id&param=report_period
```

**Success Response:**

```
Command sent to device {device_id}
```

**Error Responses:**

*   `400 Bad Request`: If `device_id` or `param` is missing.
*   `500 Internal Server Error`: If the command fails to be sent.
*   `503 Service Unavailable`: If the MQTT client is not connected.