# MediaRenderer service host (issue #64)

The maintained C lifecycle is `include/mupnp/std/av/cmediarenderer.h`, implemented
in `src/mupnp/std/av/cmediarenderer.c`. It hosts MediaRenderer:1 services through
the normal mUPnP Device implementation. It is a service-only compatibility host:
there is no playback backend, supported media format, event callback backend or
DLNA/conformance claim. The other legacy `std/av` sources are not migrated.

Construction and deletion before start are inert. `cg_upnpav_dmr_start` explicitly
starts the normal Device HTTP/SSDP lifecycle on its discovered interfaces; stop
uses the normal Device teardown. The owner must serialize lifecycle calls and
stop before releasing it. A fresh UDN and a 30-second lease limit stale discovery
entries; forced termination can still leave cached entries until expiry.

## Example and Objective-C packaging

`upnpavchk --with-renderer` constructs/starts the C renderer, then constructs,
starts and searches the ordinary ControlPoint for `upnp:rootdevice`. It waits and
prints the existing directory listing, stops/deletes the ControlPoint, then
stops/deletes the renderer. Errors and SIGINT/SIGTERM follow the same cleanup (SIGBREAK too where provided by the C runtime).
No custom diagnostic networking remains. No-argument behavior and `upnpavdump`
are unchanged. This normal discovery path can contact discovered LAN devices and
print private media names; run it only in an authorized environment, and do not
publish private output. `--help` and invalid arguments do not start networking.

The existing `wrapper/objc/xcode/mUPnP4ObjC.xcodeproj` packages public
`CGUpnpAvRenderer.h` and its implementation for iOS. `CGUpnpAvRenderer` directly
owns the C API and provides inert initialization, `start`, `stop`, `isRunning`,
`friendlyName`, and `udn`. Use it on the existing controller's serialized worker,
start before the intended search, and stop after that search, cancellation or
foreground exit. Application integration should be an explicit, one-search
compatibility action, disabled for ordinary and automatic discovery. Do not add
a permanent/background advertiser or a second ControlPoint.

GetProtocolInfo returns empty Source/Sink. GetTransportInfo instance 0 reports
NO_MEDIA_PRESENT/OK/speed 1; invalid instances return 718. Playback/mutating
transport actions return 701; other unsupported actions return 401. Event
subscriptions are rejected (405), so arbitrary CALLBACK/media URLs are not
fetched. HTTP requests use the normal Device dispatcher under the host mutex.
LastChange storage is separate for AVTransport and RenderingControl; it is an
internal foundation only, without schema validation or notifications. Full
required actions, playback state transitions, eventing and conformance remain
future work.

## Offline validation

```sh
cmake -S . -B build -DMUPNP_ENABLE_RENDERER_TESTS=ON
cmake --build build
ctest --test-dir build -R '^(renderer_api|renderer_lifecycle|avchk_lifecycle)$' --output-on-failure
# macOS: Foundation ownership test, with inert lifecycle substitutes
./test/unix/renderer-objc.sh build
```

The test option defaults OFF; the inert public C API is part of the core library.
The tests check service wiring/directions, unsupported playback, stale inputs,
LastChange separation, constructor errors, exact CLI lifecycle ordering, failure
cleanup and interruption. POSIX network guards abort on network operations;
Device lifecycle calls in lifecycle tests are replaced by inert counters. These
tests do not validate physical-interface advertising or NAS interoperability.

The earlier bespoke diagnostic's M4 observation does not validate this replacement.
Later authorized hardware comparison should use the same client/interface and
unchanged NAS approvals: ordinary control-point-only, explicit renderer plus
control point, and a separately recorded manually allowed control-point baseline.
Record SSDP, description retrieval and ContentDirectory Browse independently;
verify host advertisement and teardown separately. Remembered approval state can
contaminate comparisons. Do not alter NAS policy to force success.

## Portability

CMake includes upnpavchk on every supported platform. The earlier UNIX gate was
introduced for the removed bespoke diagnostic's POSIX sockets/select/interface
code; it is not required by the normal Device/ControlPoint lifecycle. Waiting
uses mupnp_sleep and interrupts use C runtime signals, with SIGBREAK where
available. Windows console-close/forced process termination can bypass normal
cleanup; no console GUI or process-termination guarantee is implied.

The platform matrix builds examples and renderer tests on Windows MinGW/UCRT,
macOS and Linux. CLI help/invalid-argument checks and inert lifecycle tests make
no LAN requests. Cross compilation alone does not establish Windows runtime or
console signal behavior.
