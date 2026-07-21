# BinaryLight:1 device example

This example implements the standardized UPnP `BinaryLight:1` device and its
required `SwitchPower:1` service.

The service supports:

- `SetTarget(newTargetValue)`
- `GetTarget(RetTargetValue)`
- `GetStatus(ResultStatus)`
- Event notifications for changes to `Status`

After building the project, start the device with:

```sh
./examples/binarylight/upnpbinarylight [cache_control_in_sec]
```

Press `1` to turn the light on, `0` to turn it off, space to toggle it, or `q`
to quit. A UPnP control point can invoke the `SwitchPower:1` actions, and the
device's `/presentation` URL provides the same on/off controls in a browser.

Specifications:

- [BinaryLight:1 Device](https://upnp.org/specs/ha/UPnP-ha-BinaryLight-v1-Device.pdf)
- [SwitchPower:1 Service](https://upnp.org/specs/ha/UPnP-ha-SwitchPower-v1-Service.pdf)
