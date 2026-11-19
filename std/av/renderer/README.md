# Offline MediaRenderer foundation (issue #64)

This internal, uninstalled C model is a construction/dispatch fixture, not a
working MediaRenderer or a host-app feature switch. It deliberately has no start,
advertise, discovery, action-posting, playback backend or Objective-C API. Never
start the underlying device obtained through core service internals. Its fixed
fixture UDN and incomplete service descriptions must not be published. There is
no DLNA or UPnP conformance claim.

The optional CMake target builds only modern model code against the current core;
it does not compile or migrate the legacy MediaServer/AV wrappers. Default builds
and installed core headers/libraries are unchanged.

```sh
cmake -S . -B build-renderer -DMUPNP_ENABLE_RENDERER_MODEL=ON \
  -DMUPNP_ENABLE_EXAMPLES=OFF -DMUPNP_ENABLE_TEST=OFF
cmake --build build-renderer
ctest --test-dir build-renderer -R '^renderer_model$' --output-on-failure
```

`test/unix/renderer-model.sh` runs the same isolated check and is registered with
`make check` (skipped if CMake is absent). Do not run the full legacy AV renderer
test: it starts advertising and loops indefinitely.

## Model contract and tests

Construction parses a MediaRenderer:1 device and ConnectionManager, AVTransport
and RenderingControl services without starting sockets or workers. The internal
XML constructor permits parser failure/partial-construction cleanup tests.
`GetProtocolInfo` clears both output arguments: no source or sink formats are
supported. Instance 0 `GetTransportInfo` reports `NO_MEDIA_PRESENT`, `OK`, speed 1.
Invalid/missing instances are rejected with 718. `SetAVTransportURI`, `Play` and
`Stop` are rejected with 701; other actions (including `GetVolume`) return 401.
Dispatch returns a Boolean and an action status; it never posts SOAP. This narrow
behavior is not a complete device state machine or wire-level SOAP validation.

LastChange is stored separately for AVTransport and RenderingControl, as raw XML,
using the core setter that does not notify subscribers. No event stream is sent.
The setter stores text, not a validated event schema. The model is single-threaded;
concurrent access is outside this phase.

Regression checks cover service/argument ownership and direction, stale protocol
outputs, rejected playback, query behavior after rejection, foreign actions,
LastChange separation, invalid API inputs, repeated construction/destruction and
malformed device/service XML at every parse stage. POSIX tests override socket,
connect, bind, listen, send, sendto and getaddrinfo to abort before network I/O;
server lists and advertiser state must also remain idle. Other platforms retain
the idle-state checks but do not have the POSIX overrides. These checks do not
establish allocation-failure safety for all core parsers/constructors.

## Remaining engineering

Before exposing a network lifecycle, define the host-owned playback contract and
actual supported formats, implement required actions and errors, instance/state
transitions, URI/metadata validation, volume/mute and valid LastChange/GENA
behavior. Replace the fixture identity, validate descriptions against OCF
MediaRenderer:1, and test explicit user opt-in, shutdown and negative cases. Keep
remote playback disabled by default. The legacy `std/av` build still needs API,
include and packaging migration; its Objective-C renderer class posts outgoing
SOAP and does not supply playback. A thin maintained host bridge comes later.

## Later hardware comparison (not implemented or run)

Use the existing `examples/upnpavchk` client as the intended comparison harness.
Keep its default control-point-only behavior. Only after a genuine local renderer
exists, consider an explicit `upnpavchk --with-renderer` option; this is a design
proposal, not an available flag. Keep `examples/upnpavdump` unchanged.

On the same client and subnet, compare control point only with genuine local
renderer plus control point. Preserve QNAP approval policy/state across that pair;
record remembered approvals and also measure a manually allowed control-point-only
baseline. Record SSDP response, device-description retrieval and ContentDirectory
Browse independently, together with NAS/add-on versions. Do not infer a renderer
requirement from discovery alone or bypass NAS approval. Keep private identifiers
out of public results. If manual approval suffices, do not make a hosted renderer
a prerequisite. No hardware interoperability conclusion follows from these tests.
