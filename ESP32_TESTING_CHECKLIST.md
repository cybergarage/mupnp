# ESP32 Testing Checklist

This document provides a comprehensive checklist for testing the ESP32 implementation of mUPnP.

## Prerequisites

- [ ] ESP-IDF v5.0 or later installed and configured
- [ ] ESP32 development board (ESP32, ESP32-S3 recommended)
- [ ] USB cable for flashing and monitoring
- [ ] WiFi network with multicast support
- [ ] PC/laptop on same network for testing

## Build Testing

### Control Point Example

```bash
cd examples/esp32/control_point
```

- [ ] Run `idf.py menuconfig` successfully
- [ ] Set WiFi SSID and password
- [ ] Run `idf.py build` without errors
- [ ] Verify build outputs in `build/` directory
- [ ] Check binary size is reasonable (~500KB-1MB)

### Device Example

```bash
cd examples/esp32/device
```

- [ ] Run `idf.py menuconfig` successfully
- [ ] Set WiFi SSID and password
- [ ] Run `idf.py build` without errors
- [ ] Verify build outputs in `build/` directory
- [ ] Check binary size is reasonable (~500KB-1MB)

## Flash Testing

### Control Point

```bash
cd examples/esp32/control_point
```

- [ ] Connect ESP32 via USB
- [ ] Run `idf.py -p /dev/ttyUSB0 flash` (adjust port)
- [ ] Flash completes without errors
- [ ] Monitor output with `idf.py -p /dev/ttyUSB0 monitor`

### Device

```bash
cd examples/esp32/device
```

- [ ] Connect ESP32 via USB
- [ ] Run `idf.py -p /dev/ttyUSB0 flash` (adjust port)
- [ ] Flash completes without errors
- [ ] Monitor output with `idf.py -p /dev/ttyUSB0 monitor`

## Runtime Testing

### WiFi Connection

**Control Point:**
- [ ] Boots successfully
- [ ] Connects to WiFi
- [ ] Gets IP address (check for "got ip" message)
- [ ] IP address shown in logs

**Device:**
- [ ] Boots successfully
- [ ] Connects to WiFi
- [ ] Gets IP address (check for "got ip" message)
- [ ] IP address shown in logs

### Control Point Discovery

With control point running:

- [ ] Starts searching for devices
- [ ] SSDP M-SEARCH packets sent
- [ ] Discovers at least one device on network
- [ ] Device information displayed:
  - [ ] Device name
  - [ ] Device type
  - [ ] Location URL
- [ ] Periodic searches continue every 30 seconds
- [ ] No crashes or restarts

### Device Advertisement

With device running:

- [ ] Device starts successfully
- [ ] SSDP server listening on 239.255.255.250:1900
- [ ] HTTP server starts
- [ ] Device location URL shown in logs
- [ ] Sends SSDP NOTIFY advertisements
- [ ] No crashes or restarts

### Interoperability Testing

#### Test Device with Desktop Control Point

**Linux:**
```bash
# Install tools
sudo apt-get install gupnp-tools

# Discover devices
gssdp-discover
```

- [ ] ESP32 device appears in discovery
- [ ] Device name shown correctly
- [ ] Device type shown correctly
- [ ] Location URL accessible

**Windows:**
- [ ] Use Intel Device Spy or UPnP Inspector
- [ ] ESP32 device appears
- [ ] Device details accessible

**macOS:**
- [ ] Use Discovery app or similar
- [ ] ESP32 device appears

#### Test Control Point with Desktop Device

**VLC Media Server:**
- [ ] Start VLC with UPnP enabled
- [ ] ESP32 control point discovers VLC
- [ ] VLC details shown correctly

**Router UPnP:**
- [ ] Enable UPnP on router
- [ ] ESP32 control point discovers router
- [ ] Router IGD device shown

### HTTP Testing

With device running, from PC on same network:

```bash
# Get device IP from ESP32 logs, e.g., 192.168.1.100
curl http://192.168.1.100/description.xml
```

- [ ] HTTP server responds
- [ ] Device description XML returned
- [ ] XML is valid and parseable
- [ ] Contains correct device information

### SSDP Testing

**Packet Capture:**

```bash
# On Linux
sudo tcpdump -i wlan0 -n port 1900
```

**From Control Point:**
- [ ] See M-SEARCH packets to 239.255.255.250:1900
- [ ] See M-SEARCH responses from devices
- [ ] TTL is 4 (or configured value)

**From Device:**
- [ ] See NOTIFY packets to 239.255.255.250:1900
- [ ] NOTIFY includes device type, USN, location
- [ ] TTL is 4 (or configured value)

## Stress Testing

### Memory Stability

- [ ] Run for 1 hour, check for memory leaks
- [ ] Monitor free heap: `esp_get_free_heap_size()`
- [ ] Heap should remain stable (not decreasing)
- [ ] No "out of memory" errors

### Long-term Stability

- [ ] Run for 24 hours
- [ ] No crashes or restarts
- [ ] Device remains discoverable
- [ ] Control point continues searching

### Multiple Connections

**For Device:**
- [ ] Multiple control points connect simultaneously
- [ ] All connections handled correctly
- [ ] No connection refused errors
- [ ] Memory remains stable

**For Control Point:**
- [ ] Discovers multiple devices (if available)
- [ ] All devices shown correctly
- [ ] Cache handles multiple devices

## Performance Testing

### Timing Measurements

**Control Point:**
- [ ] Discovery time: ____ seconds (typical: 1-3s)
- [ ] Boot to ready: ____ seconds (typical: 2-5s)

**Device:**
- [ ] Boot to advertise: ____ seconds (typical: 2-5s)
- [ ] HTTP response time: ____ ms (typical: 50-200ms)

### Memory Usage

**Control Point:**
- [ ] Free heap at start: ______ bytes
- [ ] Free heap after discovery: ______ bytes
- [ ] Free heap after 1 hour: ______ bytes
- [ ] Minimum free heap: ______ bytes (should be >100KB)

**Device:**
- [ ] Free heap at start: ______ bytes
- [ ] Free heap after serving: ______ bytes
- [ ] Free heap after 1 hour: ______ bytes
- [ ] Minimum free heap: ______ bytes (should be >100KB)

## Error Handling

### Network Issues

**Disconnect WiFi:**
- [ ] ESP32 attempts reconnection
- [ ] mUPnP handles network down gracefully
- [ ] No crashes

**Re-connect WiFi:**
- [ ] ESP32 reconnects successfully
- [ ] mUPnP resumes operation
- [ ] Device re-advertises (for device example)

### Invalid Inputs

**For Device:**
- [ ] Invalid HTTP requests handled
- [ ] Malformed XML handled
- [ ] Buffer overflows prevented

**For Control Point:**
- [ ] Invalid device responses handled
- [ ] Malformed XML handled
- [ ] Timeout on non-responsive devices

## Configuration Testing

### Kconfig Options

Test with different configurations:

**Log Levels:**
- [ ] Level 0 (None): Minimal logging
- [ ] Level 3 (Info): Normal logging
- [ ] Level 5 (Verbose): Detailed logging

**Stack Sizes:**
- [ ] 2048 bytes: Minimum functional
- [ ] 4096 bytes: Default/recommended
- [ ] 8192 bytes: Large buffers

**Multicast TTL:**
- [ ] TTL 1: Local subnet only
- [ ] TTL 4: UPnP recommended
- [ ] TTL 32: Extended range

## Platform Testing

### ESP32 Variants

**ESP32 (dual-core, 520KB RAM):**
- [ ] Builds successfully
- [ ] Runs control point
- [ ] Runs device
- [ ] No memory issues

**ESP32-S3 (dual-core, 512KB RAM):**
- [ ] Builds successfully
- [ ] Runs control point
- [ ] Runs device
- [ ] No memory issues

**ESP32-S2 (single-core, 320KB RAM):**
- [ ] Builds successfully
- [ ] Runs control point
- [ ] Runs device
- [ ] May need optimization for memory

**ESP32-C3 (RISC-V, 400KB RAM):**
- [ ] Builds successfully
- [ ] Runs control point
- [ ] Runs device
- [ ] Performance acceptable

## Documentation Validation

- [ ] README.md ESP32 section is accurate
- [ ] ESP32_BUILD_GUIDE.md instructions work
- [ ] ESP32_IMPLEMENTATION_NOTES.md is technically correct
- [ ] examples/esp32/README.md is helpful
- [ ] Code comments are clear

## Regression Testing

Verify no impact on other platforms:

- [ ] Unix build still works (if possible to test)
- [ ] Windows build still works (if possible to test)
- [ ] CMakeLists.txt changes don't break existing builds

## Security Testing

- [ ] Passwords not logged in production builds
- [ ] No buffer overflows in network code
- [ ] No stack overflows under load
- [ ] Safe integer arithmetic (no overflows)

## Final Checklist

- [ ] All basic functionality tests pass
- [ ] No critical bugs found
- [ ] Performance is acceptable
- [ ] Memory usage is reasonable
- [ ] Documentation is accurate
- [ ] Code quality is good

## Issues Found

Document any issues discovered during testing:

| Issue # | Description | Severity | Status |
|---------|-------------|----------|--------|
| | | | |

## Test Results Summary

- **Build Tests**: PASS / FAIL / NOT TESTED
- **Runtime Tests**: PASS / FAIL / NOT TESTED
- **Interoperability**: PASS / FAIL / NOT TESTED
- **Stress Tests**: PASS / FAIL / NOT TESTED
- **Overall Status**: PASS / FAIL / NEEDS WORK

## Tester Information

- **Tester Name**: _______________
- **Date**: _______________
- **ESP-IDF Version**: _______________
- **ESP32 Board**: _______________
- **Network Setup**: _______________

## Notes

Additional observations or comments:

---

## Quick Start Testing

For a quick validation (30 minutes):

1. Build control point example
2. Flash to ESP32
3. Monitor output
4. Verify WiFi connection
5. Check device discovery
6. Run for 10 minutes stable

Minimum passing criteria:
- ✅ Builds without errors
- ✅ Connects to WiFi
- ✅ Discovers at least one device
- ✅ No crashes in 10 minutes
- ✅ Memory remains stable
