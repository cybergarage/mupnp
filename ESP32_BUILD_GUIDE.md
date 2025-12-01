# Building mUPnP for ESP32

This guide provides detailed instructions for building mUPnP on ESP32 using ESP-IDF.

## Prerequisites

### 1. Install ESP-IDF

Install ESP-IDF v5.0 or later. Follow the official ESP-IDF installation guide:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

```bash
# Quick install on Linux/macOS
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
git checkout v5.1  # or later version
./install.sh all
. ./export.sh
```

### 2. Verify ESP-IDF Installation

```bash
idf.py --version
# Should show: ESP-IDF v5.x or later
```

## Building the Examples

### Option 1: Control Point Example

```bash
# Navigate to control point example
cd examples/esp32/control_point

# Configure project (set WiFi credentials)
idf.py menuconfig
# In menuconfig:
#  - Component config -> mUPnP Configuration
#  - Set WiFi SSID and password in the example configuration

# Build
idf.py build

# Flash to ESP32 (replace PORT with your device port)
idf.py -p /dev/ttyUSB0 flash

# Monitor output
idf.py -p /dev/ttyUSB0 monitor

# To exit monitor, press Ctrl+]
```

### Option 2: Device Example

```bash
# Navigate to device example
cd examples/esp32/device

# Configure and build
idf.py menuconfig  # Set WiFi credentials
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Using mUPnP in Your Own ESP32 Project

### Method 1: As an ESP-IDF Component (Recommended)

1. Create your ESP-IDF project:

```bash
idf.py create-project my-upnp-app
cd my-upnp-app
```

2. Add mUPnP as a component:

```bash
mkdir -p components
cd components
git clone https://github.com/cybergarage/mupnp.git
cd ..
```

3. Your project structure should look like:

```
my-upnp-app/
├── CMakeLists.txt
├── main/
│   ├── CMakeLists.txt
│   └── main.c
└── components/
    └── mupnp/
        ├── CMakeLists.txt
        ├── Kconfig
        └── ...
```

4. In your `main/CMakeLists.txt`, add mupnp requirement:

```cmake
idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES mupnp)
```

5. Use mUPnP in your code:

```c
#include <mupnp/upnp.h>

void app_main(void) {
    // Initialize WiFi first (see examples)
    
    // Create control point
    mUpnpControlPoint* cp = mupnp_controlpoint_new();
    mupnp_controlpoint_start(cp);
    
    // Search for devices
    mupnp_controlpoint_search(cp, MUPNP_ST_ROOT_DEVICE);
    
    // ... your code
}
```

### Method 2: Using ESP-IDF Component Registry (Future)

Once published to the component registry:

```bash
idf.py add-dependency "cybergarage/mupnp^4.2.0"
```

## Configuration Options

### WiFi Settings

Configure via menuconfig or directly in `sdkconfig`:

```
CONFIG_ESP_WIFI_SSID="your-ssid"
CONFIG_ESP_WIFI_PASSWORD="your-password"
```

### mUPnP Settings

Available in menuconfig under "Component config -> mUPnP Configuration":

- **Thread Stack Size** (CONFIG_MUPNP_THREAD_STACK_SIZE): Default 4096 bytes
- **Log Level** (CONFIG_MUPNP_LOG_LEVEL): 0-5, default 3 (Info)
- **Multicast TTL** (CONFIG_MUPNP_MULTICAST_TTL): Default 4
- **HTTP Server Port** (CONFIG_MUPNP_HTTP_SERVER_PORT): Default 80
- **SSDP Announce Interval** (CONFIG_MUPNP_SSDP_ANNOUNCE_INTERVAL): Default 1800 seconds

### LWIP Settings (Required)

These should be enabled automatically via `sdkconfig.defaults`:

```
CONFIG_LWIP_IGMP=y
CONFIG_LWIP_SO_REUSE=y
CONFIG_LWIP_SO_RCVBUF=y
CONFIG_LWIP_MULTICAST_TX_OPTIONS=y
```

## Troubleshooting

### Build Errors

**Error: "mupnp/upnp.h: No such file or directory"**
- Ensure mupnp is in the components directory
- Check that CMakeLists.txt includes REQUIRES mupnp

**Error: "undefined reference to `pthread_create`"**
- ESP32 uses FreeRTOS, not pthreads. This error suggests the ESP32 platform detection failed.
- Verify ESP_PLATFORM or ESP32 is defined during build

**Expat XML Parser errors**
- Ensure expat component is available
- Add to idf_component.yml: `espressif/expat: "^2.4.7"`

### Runtime Issues

**Device not discovered**
- Verify WiFi connection (check for "got ip" message)
- Ensure IGMP is enabled in sdkconfig
- Check that multicast is supported by your router/network
- Verify firewall settings

**Stack overflow**
- Increase CONFIG_MUPNP_THREAD_STACK_SIZE
- Monitor with `esp_get_minimum_free_heap_size()`

**Memory issues**
- ESP32 has ~520KB RAM; ESP32-C3 has ~400KB
- Reduce logging verbosity
- Limit concurrent connections
- Reduce buffer sizes if necessary

### Debug Tips

1. **Enable verbose logging:**
```
CONFIG_MUPNP_LOG_LEVEL=5
CONFIG_LOG_DEFAULT_LEVEL_VERBOSE=y
```

2. **Monitor heap:**
```c
ESP_LOGI(TAG, "Free heap: %d", esp_get_free_heap_size());
```

3. **Check task stack:**
```c
ESP_LOGI(TAG, "Stack HWM: %d", uxTaskGetStackHighWaterMark(NULL));
```

## Performance Considerations

### Memory Usage

Typical memory footprint:
- Base library: ~100KB Flash, ~20KB RAM
- Control Point: +30KB RAM (with device cache)
- Device Server: +20KB RAM (per active connection)

### Network Performance

- SSDP discovery typically takes 1-3 seconds
- HTTP requests complete in 100-500ms on good WiFi
- Multicast reception reliable up to ~10 devices simultaneously

### Optimization Tips

1. **Reduce stack sizes** if memory constrained
2. **Disable features** not needed (query control, eventing)
3. **Use static buffers** for XML parsing when possible
4. **Limit device cache size** in control points
5. **Reduce SSDP announcement frequency** for devices

## Testing Devices on Network

### Using Control Point Example

```bash
# Flash control point
cd examples/esp32/control_point
idf.py -p /dev/ttyUSB0 flash monitor

# Look for output like:
# I (5000) upnp_cp: Found device:
# I (5000) upnp_cp:   Name: My UPnP Device
# I (5000) upnp_cp:   Type: urn:schemas-upnp-org:device:MediaServer:1
```

### Using Desktop Tools

**Linux:**
```bash
# Install gssdp-tools
sudo apt-get install gupnp-tools

# Discover devices
gssdp-discover

# Or use mupnp native example
cd examples/upnpdump/unix
make
./upnpdump
```

**Windows:**
- Intel Device Spy (discontinued but available)
- UPnP Inspector
- VLC (View -> Playlist -> Local Network -> Universal Plug'n'Play)

**macOS:**
- Kinsky (Linn)
- Discovery - UPnP device inspector

## Advanced Topics

### Custom Device Descriptions

Place device XML in your main app or load from SPIFFS/LittleFS:

```c
static const char* MY_DEVICE_XML = 
"<?xml version=\"1.0\"?>\n"
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n"
"  <!-- your device description -->\n"
"</root>\n";

mUpnpDevice* device = mupnp_device_new();
mupnp_device_parsedescription(device, MY_DEVICE_XML, strlen(MY_DEVICE_XML));
```

### Persistent Storage

Use NVS for storing UUIDs and configuration:

```c
#include "nvs_flash.h"
#include "nvs.h"

// Store UUID
nvs_handle_t handle;
nvs_open("mupnp", NVS_READWRITE, &handle);
nvs_set_str(handle, "uuid", device_uuid);
nvs_commit(handle);
nvs_close(handle);
```

### OTA Updates

Consider device state during updates:
- Stop SSDP announcements before update
- Send byebye messages
- Save state to NVS

## Support and Resources

- **mUPnP Documentation**: http://cybergarage.github.io/mupnp/
- **ESP-IDF Documentation**: https://docs.espressif.com/projects/esp-idf/
- **UPnP Specifications**: http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v2.0.pdf
- **Issue Tracker**: https://github.com/cybergarage/mupnp/issues

## Contributing

Contributions are welcome! Please:
1. Test on real ESP32 hardware
2. Verify with actual UPnP clients/devices
3. Include examples of usage
4. Update documentation

## License

mUPnP for C is licensed under the BSD-style license. See COPYING file for details.
