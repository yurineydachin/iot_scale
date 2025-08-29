# iot_scale
IoT vibe coding

Subscribe command results
```
yc --profile prod iot mqtt subscribe --cert credentials/cert.pem --key credentials/key.pem -t '$devices/are0b0rpeijg822eirct/events/status'
```

Subscribe telemetry
```
yc --profile prod iot mqtt subscribe --cert credentials/cert.pem --key credentials/key.pem -t '$devices/are0b0rpeijg822eirct/events/telemetry'
```

Publish get command
```
yc --profile prod iot mqtt publish --cert credentials/cert.pem --key credentials/key.pem -t '$devices/are0b0rpeijg822eirct/commands/command' -m '{"version":65536, "timestamp":"1756459015", "validUntil":"1756459315", "command":{"chainId":"385a869c-8c6d-4887-99e8-e2e727212bb3", "payload":{"getParams":{"param":["heartbeat"]}}}}'
```
