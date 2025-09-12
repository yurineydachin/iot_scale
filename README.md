# IoT Scale

[YC IoT docs](https://yandex.cloud/ru/docs/iot-core/operations/)

This document describes the commands that can be sent to the IoT Scale device via an MQTT client.

## Subscribing to Device Events

### Command Results

To subscribe to the results of commands sent to the device, use the following command:

```bash
yc --profile prod iot mqtt subscribe --cert credentials/cert.pem --key credentials/key.pem -t '$devices/{device-id}/events/status'
```

### Telemetry Data

To subscribe to telemetry data from the device, use this command:

```bash
yc --profile prod iot mqtt subscribe --cert credentials/cert.pem --key credentials/key.pem -t '$devices/{device-id}/events/telemetry'
```

---

## Publishing Commands

All commands are sent to the `$devices/{device-id}/commands/command` topic. The general structure of a command message is a JSON object with the following fields:

- `version`: The protocol version (e.g., `65536` for 1.0.0).
- `timestamp`: UNIX timestamp of when the command is sent.
- `validUntil`: UNIX timestamp until which the command is valid.
- `command`: An object containing the command details.
    - `chainId`: A unique identifier for the command sequence.
    - `payload`: The actual command payload.

### Get Parameters (`getParams`)

This command requests the values of one or more parameters from the device.

**Payload:** `{"getParams":{"param":["param1", "param2"]}}`

**Example:** Get the `heartbeat` parameter.

```bash
yc --profile prod iot mqtt publish --cert credentials/cert.pem --key credentials/key.pem -t '$devices/{device-id}/commands/command' -m '{"version":65536, "timestamp":"1756459015", "validUntil":"1756459315", "command":{"chainId":"385a869c-8c6d-4887-99e8-e2e727212bb3", "payload":{"getParams":{"param":["heartbeat"]}}}}'
```

### Set Parameters (`setParams`)

This command sets the values of one or more parameters on the device.

**Payload:** `{"setParams":{"params":{"param1":"value1", "param2":"value2"}}}`

**Example:** Lock the vehicle.

```bash
yc --profile prod iot mqtt publish --cert credentials/cert.pem --key credentials/key.pem -t '$devices/{device-id}/commands/command' -m '{"version":65536, "timestamp":"1756459015", "validUntil":"1756459315", "command":{"chainId":"385a869c-8c6d-4887-99e8-e2e727212bb3", "payload":{"setParams":{"params":{"vehicle_lock":"locked"}}}}}'
```

**Example:** Unlock the vehicle.

```bash
yc --profile prod iot mqtt publish --cert credentials/cert.pem --key credentials/key.pem -t '$devices/{device-id}/commands/command' -m '{"version":65536, "timestamp":"1756459015", "validUntil":"1756459315", "command":{"chainId":"385a869c-8c6d-4887-99e8-e2e727212bb3", "payload":{"setParams":{"params":{"vehicle_lock":"unlocked"}}}}}'
```

### Configure States (`configure`)

This command defines a dictionary of states and their corresponding parameter values.

**Payload:** `{"configure":{"states":{"state_name":{"state_params":{"param1":"value1"}}}}}`

**Example:** Configure a "riding" state.

```bash
yc --profile prod iot mqtt publish --cert credentials/cert.pem --key credentials/key.pem -t '$devices/{device-id}/commands/command' -m '{"version":65536, "timestamp":"1756459015", "validUntil":"1756459315", "command":{"chainId":"385a869c-8c6d-4887-99e8-e2e727212bb3", "payload":{"configure":{"states":{"state_riding":{"state_params":{"engine_lock":"disable","speed_limit":"25"}}}}}}}'
```

### Set State (`setState`)

This command switches the device to a previously defined state.

**Payload:** `{"setState":{"state":"state_name"}}`

**Example:** Switch to the "riding" state.

```bash
yc --profile prod iot mqtt publish --cert credentials/cert.pem --key credentials/key.pem -t '$devices/{device-id}/commands/command' -m '{"version":65536, "timestamp":"1756459015", "validUntil":"1756459315", "command":{"chainId":"385a869c-8c6d-4887-99e8-e2e727212bb3", "payload":{"setState":{"state":"state_riding"}}}}'
```

### Ping (`ping`)

This command is a simple ping message to check connectivity.

**Payload:** `{"ping":{}}`

**Example:**

```bash
yc --profile prod iot mqtt publish --cert credentials/cert.pem --key credentials/key.pem -t '$devices/{device-id}/commands/command' -m '{"version":65536, "timestamp":"1756459015", "validUntil":"1756459315", "command":{"chainId":"385a869c-8c6d-4887-99e8-e2e727212bb3", "payload":{"ping":{}}}}'
