# ESP-IDF Wi-Fi control point

This example connects an ESP32 to Wi-Fi, starts a control point after receiving
an IPv4 address, and searches for UPnP root devices every 30 seconds. Serial logs
show SSDP response ST/USN/LOCATION fields and device-cache additions/removals.
Network changes stop and recreate the control point from a single owner task.

See [the ESP-IDF guide](../../../doc/espidf.md) for supported configuration,
complete setup, security notes, and the hardware smoke-test checklist.

```sh
# In an ESP-IDF v5.5.5 shell, from this directory:
idf.py set-target esp32
idf.py menuconfig  # mUPnP control point example -> Wi-Fi credentials
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Wi-Fi credentials are stored in plaintext in the local `sdkconfig` and firmware.
Do not commit them. The example never logs the password. `sdkconfig.defaults`
contains only non-secret defaults; no credentials are needed for a compile-only
CI build. CI uses the non-secret SSID in `sdkconfig.ci` so the full application
is linked instead of being optimized away by the empty-SSID check.
The example logs a setup instruction and exits if no SSID is configured.

The example uses RAM-only Wi-Fi configuration. Its standard NVS initialization
recovery erases the default NVS partition if it is full or has an incompatible
version. Adapt that recovery before integrating it into an existing product.
