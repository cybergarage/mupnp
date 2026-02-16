# mUPnP for ESP32 Examples

This directory contains example applications demonstrating how to use mUPnP on ESP32 microcontrollers using ESP-IDF.

## Prerequisites

- ESP-IDF v5.0 or later installed and configured
- ESP32 development board (ESP32, ESP32-S2, ESP32-S3, ESP32-C3, etc.)
- WiFi network for testing

## Examples

### 1. Control Point Example

Located in `control_point/`, this example demonstrates a UPnP control point that discovers devices on the network.

**Features:**
- WiFi station mode connection
- SSDP M-SEARCH for device discovery
- Periodic device scanning
- Device information display

**Building and Running:**

```bash
cd examples/esp32/control_point

# Configure WiFi credentials
idf.py menuconfig
# Navigate to "Example Configuration" and set your WiFi SSID and password

# Build
idf.py build

# Flash and monitor
idf.py -p /dev/ttyUSB0 flash monitor
```

### 2. Device Example

Located in `device/`, this example demonstrates a basic UPnP device that advertises itself on the network.

**Features:**
- WiFi station mode connection
- UPnP device advertisement via SSDP
- HTTP server for device description
- Basic service with action handlers

**Building and Running:**

```bash
cd examples/esp32/device

# Configure WiFi credentials
idf.py menuconfig
# Navigate to "Example Configuration" and set your WiFi SSID and password

# Build
idf.py build

# Flash and monitor
idf.py -p /dev/ttyUSB0 flash monitor
```

## Configuration

Both examples use `sdkconfig.defaults` to configure ESP-IDF and mUPnP settings:

### WiFi Configuration
- `CONFIG_ESP_WIFI_SSID`: Your WiFi network SSID
- `CONFIG_ESP_WIFI_PASSWORD`: Your WiFi password

### mUPnP Configuration
- `CONFIG_MUPNP_THREAD_STACK_SIZE`: Stack size for mUPnP threads (default: 4096 bytes)
- `CONFIG_MUPNP_LOG_LEVEL`: Logging verbosity (0=None, 3=Info, 5=Verbose)
- `CONFIG_MUPNP_MULTICAST_TTL`: TTL for SSDP multicast packets (default: 4)

### Network Configuration
The examples require the following LWIP features to be enabled:
- `CONFIG_LWIP_IGMP=y`: IGMP support for multicast
- `CONFIG_LWIP_SO_REUSE=y`: Socket reuse
- `CONFIG_LWIP_MULTICAST_TX_OPTIONS=y`: Multicast transmission options

These are already configured in `sdkconfig.defaults`.

## Testing

### Testing the Control Point

1. Ensure you have UPnP devices on your network (e.g., media servers, routers with UPnP enabled)
2. Flash and run the control point example
3. Monitor the serial output to see discovered devices
4. The control point will search for devices every 30 seconds

### Testing the Device

1. Flash and run the device example
2. Use a UPnP control point tool to discover the device:
   - **Windows**: Use [Intel Device Spy](https://www.intel.com/content/www/us/en/support/articles/000055952/wireless.html)
   - **Linux**: Use `gssdp-discover` from gssdp-tools
   - **Cross-platform**: Use the control point example from this repository
3. Verify the device appears as "ESP32 mUPnP Device"
4. Try accessing the device description at: `http://<ESP32_IP>:80/description.xml`

## Troubleshooting

### Device not discovered
- Check WiFi connection is established (look for "got ip" message)
- Verify multicast routing on your network
- Check firewall settings on your PC
- Ensure IGMP snooping is properly configured on your network switch

### Build errors
- Ensure ESP-IDF is properly installed and sourced
- Check that you're using ESP-IDF v5.0 or later
- Try `idf.py fullclean` and rebuild

### Memory issues
- Increase `CONFIG_MUPNP_THREAD_STACK_SIZE` if seeing stack overflow
- Reduce `CONFIG_LOG_MAXIMUM_LEVEL` to decrease memory usage
- Monitor free heap with `esp_get_free_heap_size()`

### Network timeouts
- Increase receive buffer sizes in `sdkconfig.defaults`
- Check network congestion
- Verify router/AP multicast support

## Architecture Notes

### Threading
mUPnP uses FreeRTOS tasks on ESP32. Each SSDP server, HTTP server, and event listener runs in its own task.

### Memory
The examples are configured with reasonable defaults for ESP32 with 520KB RAM. For devices with less memory (like ESP32-C3), you may need to:
- Reduce thread stack sizes
- Disable verbose logging
- Limit the number of concurrent connections

### Network
- SSDP uses UDP multicast to 239.255.255.250:1900
- HTTP server typically runs on port 80 (configurable)
- Requires lwIP with IGMP support for multicast

## Additional Resources

- [mUPnP Documentation](http://cybergarage.github.io/mupnp/)
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [UPnP Device Architecture](http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v2.0.pdf)
