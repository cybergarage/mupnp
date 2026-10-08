# ESP-IDF port

mUPnP for C 3.1.0 is the first release that features ESP32 support, building
on the initial port introduced in 3.0.5 with fixes found during ESP32 hardware
testing. For a reproducible build, use the latest release tag (`3.1.1`) rather
than an arbitrary `master` revision.

## Scope and status

The port targets **ESP32, one active Wi-Fi station interface, and IPv4**.
The build/CI baseline is **ESP-IDF v5.5.5**; the component manifest allows
`>=5.5,<6.0`. Other ESP32-family targets, Ethernet, simultaneous STA/AP or
multiple interfaces, IPv6, TLS/OpenSSL, and libcurl have not been validated by
this port. Enabling IPv6 elsewhere in an application does not enable IPv6 in
mUPnP.

The complete mUPnP library is compiled, including device, control point, SOAP,
and GENA code, but the supplied example exercises control-point discovery only.
A successful cross-build alone does not establish interoperability or memory
stability. The user-reported hardware results below cover a limited control-point
configuration; device hosting and broader deployment scenarios remain unverified.

### Reported hardware coverage

The reported configuration was ESP32 DevKitC-VE, PSRAM disabled, ESP-IDF 5.5.5,
Expat 2.8.4, one Wi-Fi STA interface and IPv4. The patched control point ran for
29 minutes without the previously observed crash; heap measurements were stable
for an 11-minute interval. Discovery, device leave/return, and single-address SOAP
and GENA interactions were exercised with a test helper. These observations are
limited smoke-test evidence, not long-term reliability or all-ESP32-family proof.
See the public [PR #30 report](https://github.com/cybergarage/mupnp/pull/30) and
[PR #31 report](https://github.com/cybergarage/mupnp/pull/31).

Actual multiple-LOCATION behavior was not reproduced on hardware. Its coverage
comes from host tests, which announce the same endpoint with different URL
spellings. Patched Wi-Fi reconnect testing was explicitly skipped. Device hosting,
other ESP32-family boards, multiple interfaces, and long-duration operation remain
unverified. Follow the checklist below for application-specific validation.

### Release limits and compatibility

Version 3.0.5 introduced the initial ESP-IDF port (#27). Version 3.1.0 adds the
XML allocation-failure and LOCATION/GENA fixes (#30) and HTTP TIME_WAIT
rebinding changes (#31), and is the version the hardware results above refer to.

`mupnp_xml_attributelist_set()` now returns `bool` instead of `void`; callers
ignoring the result still compile, but function-pointer declarations must match.
Allocation failure is reported as parse failure rather than a partial description.
When a cached device is retained during a LOCATION update, listeners receive
`mUpnpDeviceStatusUpdated`. Unchanged descriptions retain service pointers and
subscriptions. Actual description replacement preserves the device pointer and
carries subscription state by UDN/serviceId, but invalidates earlier service
pointers; reacquire services after that change. Embedded devices without UDNs
cannot carry subscription state in this replacement path.

The public #30 report also records a pre-existing device-side SUBSCRIBE URI leak
and a use-after-free when a device is deleted immediately after SUBSCRIBE while
its initial-event thread is pending. Both are fixed in 3.1.1 (#44).

Version 3.1.1 adds the fixes listed in the [ChangeLog](../ChangeLog). On ESP-IDF,
socket timeouts keep their existing polling implementation, and
`mupnp_device_start()` still fails when no usable interface exists (#47 applies
that check to other platforms). The 3.1.1 changes were verified by host
regression tests and the ESP-IDF cross-build in CI, not on ESP32 hardware; the
hardware results above refer to 3.1.0.

The component manifest currently labels the license `BSD-3-Clause`, while
`COPYING` includes an additional patent-related condition. That metadata needs
maintainer/legal review before publication to the ESP Component Registry; 3.1.0
does not change the license terms or establish that the SPDX label is correct.
Until then, use the component from this repository as described below.

## Build the example

1. Install and activate [ESP-IDF v5.5.5](https://github.com/espressif/esp-idf/releases/tag/v5.5.5)
   using Espressif's [ESP32 getting-started instructions](https://docs.espressif.com/projects/esp-idf/en/v5.5.5/esp32/get-started/index.html).
2. Clone this repository and enter the example:

   ```sh
   git clone --branch 3.1.1 https://github.com/cybergarage/mupnp.git
   cd mupnp/examples/espidf/control_point
   idf.py set-target esp32
   idf.py menuconfig
   ```

3. Under **mUPnP control point example**, set the Wi-Fi SSID/password. The example
   accepts open networks or WPA2-and-later personal networks; use a trusted test
   network. Credentials are plaintext in `sdkconfig` and the firmware image, so
   do not commit or share either. No credentials are needed to compile.
4. Build and flash, replacing the serial port with your board's port:

   ```sh
   idf.py build
   idf.py -p /dev/ttyUSB0 flash monitor
   ```

   Exit the serial monitor with `Ctrl-]`. A blank SSID causes the example to log
   a configuration instruction and exit rather than attempting to connect.

The ESP-IDF Component Manager downloads the real
[`espressif/expat` component](https://components.espressif.com/components/espressif/expat/versions/2.8.4/readme)
from the official registry (`^2.8.4`). Internet access is needed on the first
build. There is no replacement XML parser or header-only stub. Preserve the
application's generated `dependencies.lock` for reproducible deployments;
review dependency/security updates before upgrading it. The example uses
`EXTRA_COMPONENT_DIRS` to build the repository root and shares exactly the same
source list as the standalone host CMake build.

## Use in another application

Place this repository at `<project>/components/mupnp` (for example, as a Git
submodule checked out at the `3.1.1` tag), or add its path to `EXTRA_COMPONENT_DIRS` **before** including
ESP-IDF's `project.cmake`. In the application's component registration, add
`REQUIRES mupnp` and include `<mupnp/upnp.h>`. ESP-IDF names a local component
after its directory, so adjust `REQUIRES` if you use a different directory name.
Leave Component Manager enabled so `idf_component.yml` resolves Expat.

mUPnP's root CMake detects `ESP_PLATFORM`, registers an IDF component, exports
its public headers and `MUPNP_` feature definitions, then returns without running
the host project, examples, or `find_library()` logic. The existing standalone
host options retain their behavior; they are not used for the IDF branch.
The ESP branch uses Expat and IDF's lwIP, esp-netif, pthread, timer, and hardware
random-number facilities. It does not require POSIX interface enumeration,
Unix signals, or a host UUID library.

### Required configuration

The supplied `sdkconfig.defaults` sets:

- `CONFIG_LWIP_IPV4=y`: this port's SSDP and HTTP addressing is IPv4
- `CONFIG_LWIP_SO_REUSE=y`: allow SSDP multicast port sharing
- `CONFIG_LWIP_SO_REUSE_RXTOALL=y`: deliver multicast to all matching local
  sockets, necessary when a device and control point both listen on port 1900
- `CONFIG_VFS_SUPPORT_SELECT=y`: interruptible socket readiness polling
- `CONFIG_LWIP_MAX_SOCKETS=16`: room for discovery, HTTP descriptions, and
  application sockets; adjust for concurrent devices, requests, and subscriptions
- `CONFIG_MUPNP_THREAD_STACK_SIZE=8192`: bytes per mUPnP pthread; configured under
  **Component config > mUPnP** and applied to each library worker
- `CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT=8192`: example default for any other
  pthreads; mUPnP explicitly applies its own setting above

ESP-IDF 5.5 supplies lwIP IGMP/socket support; the application must initialize
esp-netif and start an IPv4 network interface. Keep the default mUPnP per-interface
addressing: do not define `MUPNP_NET_USE_ANYADDR`. The initial configuration
intentionally does not enable `MUPNP_USE_IPV6`, HTTP persistent connections,
OpenSSL, or libcurl. The example disables Wi-Fi power saving to make multicast
reception more predictable; evaluate latency and power behavior on your hardware.

These settings are **defaults for a new configuration**. Existing `sdkconfig`
values take precedence. After changing defaults in a project that already has a
configuration, use `idf.py menuconfig` to review the effective values. The
component rejects configurations without IPv4, socket reuse, or select support.

### Lifetime and memory

Initialize a control point only after `IP_EVENT_STA_GOT_IP`. Stop and delete it
when the interface goes down or its address changes, then create it again after
IPv4 is available. Socket/multicast bindings are snapshots of the active
interface; the library does not run a Wi-Fi reconnection manager for you.

The example's Wi-Fi callbacks only update an event group and reconnect Wi-Fi.
One application task owns all control-point start/search/delete operations.
Network-change bits remain pending during teardown, so rapid disconnects and
reconnections still rebuild the bindings. The example deliberately does not
use that task's notification slot: ESP-IDF's `pthread_join()` uses it internally.
Never stop/delete a control point in one of its own listener callbacks, and do
not stop it concurrently from multiple tasks. Callbacks should return promptly
and must not retain library-owned packet/device pointers.

Workers stop cooperatively and are joined before their sockets and objects are
freed. Blocking network operations poll their runnable state; application
callbacks and other unbounded user work can still delay shutdown. Size stacks
for the deepest XML descriptions and callbacks you expect, measure heap and
stack high-water marks, and budget for multiple worker stacks plus the complete
device/service XML cache. Resource use scales with the number and complexity
of discovered devices. The example searches only `upnp:rootdevice` to limit
redundant discovery traffic, but discovery can still consume significant heap.

UPnP discovers and downloads XML supplied by other hosts without authenticating
them. Use a trusted LAN, do not expose the listeners to the Internet, and keep
Expat and IDF security fixes current. The
[Expat 2.8.4 README](https://components.espressif.com/components/espressif/expat/versions/2.8.4/readme)
links outstanding upstream security concerns. This port is not a security audit
or a guarantee that arbitrary untrusted XML cannot exhaust memory.

## Validation

### Automated cross-build

From the repository root, with the SDK activated:

```sh
cd examples/espidf/control_point
idf.py -B build-ci -D SDKCONFIG=sdkconfig.ci.generated \
  -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci" set-target esp32
idf.py -B build-ci -D SDKCONFIG=sdkconfig.ci.generated \
  -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci" build
idf.py -B build-ci -D SDKCONFIG=sdkconfig.ci.generated size
```

[The ESP-IDF workflow](../.github/workflows/espidf.yml) builds the example using
a pinned revision of Espressif's official
[`esp-idf-ci-action`](https://github.com/espressif/esp-idf-ci-action) and the
`espressif/idf:v5.5.5` container. It requires no Wi-Fi secrets or hardware.
`sdkconfig.ci` provides a non-secret SSID placeholder so the compiler retains
the complete application after its blank-SSID guard. A build with a blank SSID
still compiles library objects but can optimize away most application/library
code at final link. Use the fresh configuration above for full-link validation,
and real local credentials before flashing. The CI build compiles and links the example and library; it does not verify radio behavior.
A separate host job checks the bootstrap build generators and runs the
ESP-specific thread lifecycle, socket-shutdown, and XML allocation-failure
regressions with AddressSanitizer and UndefinedBehaviorSanitizer:

```sh
test/espidf/host/check_generators.sh
test/espidf/host/run.sh
test/espidf/host/run_socket.sh
test/espidf/host/run_xml_alloc.sh
```

These tests exercise the ESP conditional code using small host adapters. They
supplement the real SDK build and cannot validate FreeRTOS/lwIP radio behavior.

The host build remains independent:

```sh
cmake -S . -B build
cmake --build build
```

### Additional hardware validation checklist

1. Put the ESP32 and a known UPnP device on the same IPv4 LAN. Disable client/AP
   isolation and ensure the network permits IGMP and UDP multicast
   `239.255.255.250:1900`.
2. Flash the example. Verify an IPv4 log, `Control point started`,
   `M-SEARCH sent for upnp:rootdevice`, and SSDP responses with a plausible
   `LOCATION`. A `Device added` log verifies that description retrieval and XML
   parsing progressed far enough to populate the device cache.
3. Compare discovered UDNs with the host `upnpdump` example or a packet capture.
   Check both active search replies and passive SSDP alive/byebye handling.
4. Disconnect/reconnect Wi-Fi repeatedly, including short outages and a changed
   DHCP address. Verify teardown, restart, rediscovery, no overlapping control
   points, and no progressive loss of heap or available sockets.
5. Repeat with a slow/unreachable description URL and unplug the access point
   during description retrieval. Confirm cleanup completes without a watchdog
   reset, use-after-free, or permanently blocked thread join.
6. Measure minimum free heap and each worker's stack high-water mark over a long
   run with your expected device count. Increase socket/stack/memory budgets if
   needed. Exercise SOAP and event subscriptions separately if your application
   uses them, including renewals, unsubscribe, and network loss.

If searches produce no responses, inspect the AP's multicast/isolation settings
and compare with a capture before changing library code. If responses appear
but no devices are added, check reachability of each `LOCATION` URL and available
heap. A cross-build cannot distinguish those network/runtime failures.
