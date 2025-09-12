# MCP Client

This document describes how to use the `mcp-client` application.

The client sends an "unlock" command to the MCP server.

## Running the client

To run the client, first ensure the `mcp-server` is running. Then, execute the following command in the `mcp_client` directory:

```bash
go run main.go
```

The client will send a `POST` request to `http://localhost:8080/tools/unlock/call` to unlock the device.