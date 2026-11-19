# Diagnostic MediaRenderer foundation (issue #64)

This internal, uninstalled model is not a playable or conformant MediaRenderer.
The default model is constructed in memory. The explicit POSIX CMake example
`upnpavchk --with-renderer` adapts it into a short-lived **diagnostic service
advertisement**, with no playback and no DLNA certification claim. This tests the
owner's service-presence hypothesis; it does not implement production rendering.

Default core/Objective-C behavior and `upnpavdump` are unchanged. The old Windows
example project is unchanged; the new diagnostic is built through CMake on POSIX.
No automatic network activity occurs during a build or the model-only test.

## Build and invoke

```sh
cmake -S . -B build-renderer -DMUPNP_ENABLE_RENDERER_MODEL=ON
cmake --build build-renderer
ctest --test-dir build-renderer --output-on-failure

# Set both variables privately for the intended target/interface; never publish them.
build-renderer/examples/upnpavchk/upnpavchk \
  --target "$NAS_IPV4" --interface "$CLIENT_IPV4" --seconds 3
build-renderer/examples/upnpavchk/upnpavchk \
  --target "$NAS_IPV4" --interface "$CLIENT_IPV4" --seconds 3 --with-renderer
```

The model option defaults OFF. In that build the target-only diagnostic works,
and `--with-renderer` fails before opening sockets with a build-option message.
`--help`, malformed flags and missing target/interface also perform no network
I/O. No arguments retain the existing directory-listing behavior; that legacy
mode can discover/fetch other devices and print directory contents. Use the
explicit target diagnostic for a scoped comparison.

Both addresses must be numeric IPv4 unicast on the same selected local subnet.
The diagnostic binds outgoing discovery and HTTP traffic to that interface,
sends one TTL-1 `upnp:rootdevice` M-SEARCH, and discards other sources before
parsing LOCATION. Only an HTTP LOCATION whose host is the exact numeric target
is accepted. Description and control URLs are validated; credentials, redirects,
foreign hosts and URL fragments are rejected. No hostnames are resolved, no SCPD
or arbitrary media URLs are fetched, and no subscriptions are made to the NAS.
After a successful description, exactly one root `BrowseMetadata` request is sent
with RequestedCount 1. Output contains only stage results/status codes, not
addresses, URLs, NAS names or media filenames. The unchanged legacy listing mode
has different, broader behavior.

## Temporary service lifecycle and limitations

Only `--with-renderer` starts the diagnostic server. It binds an ephemeral HTTP
port on the selected address, generates a fresh UDN and labels itself "no
playback". Device, UUID and all three service types are advertised once, with a
30-second lease. SSDP alive/byebye/response packets use a dedicated socket bound
to the selected address and multicast TTL 1. There is no device-wide/all-interface
start operation or background advertiser. A passive local receive verifies the
MediaRenderer alive notification when available; it does **not** prove that the
NAS received it or approved the client. A self-GET checks the description server.

HTTP reads/SOAP introspection are accepted only from the selected NAS or the
selected local address. Other peers are rejected. GetProtocolInfo reports empty
Source and Sink: no supported playback formats. GetTransportInfo for instance 0
reports NO_MEDIA_PRESENT/OK/speed 1; missing/invalid instances return 718.
SetAVTransportURI, Play and Stop return 701; other unsupported actions return 401.
RenderingControl does not claim a functional volume/mute backend. Subscriptions
are rejected and all diagnostic SCPD variables declare sendEvents=no. No CALLBACK
URL, media URL, upload or file is fetched/handled. Requests are serialized and
stored action inputs are cleared before each request.

On completion, SIGINT or SIGTERM, the diagnostic stops its SSDP listener, attempts
byebye on the selected interface, stops/drains HTTP workers and closes sockets.
Connect/read/write operations use short timeouts. A byebye-send failure is reported;
if a process is forcibly killed, caches may retain its entry until lease expiry.
NAS remembered approval/client records may persist independently. Do not change
approval policy to force a successful result.

The model-only target still has no network lifecycle or installed API. It stores
AVTransport and RenderingControl LastChange separately without notifications;
raw LastChange text is not schema validated. Production playback, required action
coverage, state transitions, eventing, conformance and a maintained Objective-C
host bridge remain outside this diagnostic phase. No claim of normal rendering
support or necessity for NAS browsing follows from service advertisement alone.

## Tests and M4 observation

The in-memory regression checks wiring/argument directions, stale protocol
outputs, unsupported playback, foreign actions, LastChange separation and parser
cleanup. POSIX model tests abort on socket/connect/bind/listen/send/sendto/DNS.
The separate loopback fixture checks HTTP/SOAP, rejected media/CALLBACK URLs,
foreign endpoints, denial/redirect handling, listener closure and actual CLI
SIGINT cleanup. Only the latter interrupt case emits SSDP on **loopback**, never
a physical interface. ASan/UBSan cover both executables. The isolated
`test/unix/renderer-model.sh` make-check entry runs these checks (requires Python 3
and CMake); other network-capable suites are not needed for this comparison.

On 2026-11-19, the owner's Apple M4 ran both modes on the same selected interface,
with no NAS approval/configuration changes. In the root-device search comparison:

| Stage | Target-only | With diagnostic renderer |
| --- | --- | --- |
| Target SSDP response within 3 seconds | None observed | None observed |
| Target description HTTP | Not run | Not run |
| Target ContentDirectory Browse | Not run | Not run |
| Local renderer | Absent | Started; alive observed locally; self-description HTTP 200; stopped with byebye |

An earlier MediaServer:1-targeted search produced the same no-response outcome.
No guessed-port HTTP request, policy bypass or NAS administration was attempted.
These results establish local advertisement/teardown, **not** QNAP compatibility
or the renderer hypothesis. NAS/add-on version, target reachability/service state,
and remembered approval state were not independently verified. A manually allowed
control-point-only baseline remains untested; no approval was changed to create
one. Renderer announcements may have affected remembered records, so this is not
a clean approval-state experiment and no compatibility conclusion is justified.

Later authorized verification should confirm the NAS/add-on and ordinary service
availability, then repeat with unchanged/recorded approval state, including a
manually allowed control-point-only baseline. Record SSDP, description and Browse
separately. Keep all addresses, private logs and media names out of public reports.
