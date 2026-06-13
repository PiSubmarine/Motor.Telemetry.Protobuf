# Depth.Telemetry.Protobuf

`PiSubmarine.Depth.Telemetry.Protobuf` adapts `Depth.Telemetry.Api` to and from
raw protobuf payloads.

- `Serializer` implements `Telemetry.Api::IRawSource` by querying a local
  `Depth::Telemetry::Api::IProvider`
- `Deserializer` implements `Depth::Telemetry::Api::IProvider` by reading bytes
  from a `Telemetry.Api::IRawSource`
