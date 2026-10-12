# Threat model: mUPnP for C

## What this project does and where untrusted input enters

mUPnP for C is a C library for building UPnP devices and control points. It implements SSDP, HTTP/HTTPU,
SOAP and GENA, and is embedded in desktop software and in ESP32 firmware (ESP-IDF component).

Every byte that arrives from the network is untrusted. UPnP runs on a local network with no authentication,
so any host on the LAN is a potential attacker; a device that is reachable from the Internet (misconfigured
router, port forwarding) widens that to anyone. The main entry points are:

- **SSDP** (`src/mupnp/ssdp/`): multicast and unicast UDP datagrams on port 1900 (NOTIFY, M-SEARCH and
  M-SEARCH responses). Parsed by both devices and control points without any prior exchange.
- **HTTP server** (`src/mupnp/http/`): requests to devices (description, SOAP control, GENA SUBSCRIBE /
  UNSUBSCRIBE) and to control points (GENA NOTIFY callbacks). Request line, headers, chunked bodies and
  Content-Length handling are all in scope.
- **HTTP client**: responses a control point receives when it fetches device and service descriptions
  from a `LOCATION` URL supplied by a (possibly malicious) SSDP announcement, and SOAP / GENA responses.
- **XML** (`src/mupnp/xml/`): device descriptions, SCPD documents, SOAP envelopes and GENA property sets,
  parsed with Expat (default) or libxml2.
- **URI/URL parsing** (`src/mupnp/net/uri.c`, `url.c`): `LOCATION`, `CALLBACK`, `URLBase`, control and
  event URLs taken from untrusted messages.

Application callbacks (action handlers, event listeners) are written by the library's users and are out of
scope, except where the library hands them malformed or dangling data.

## Components that matter most / least

- Highest priority: HTTP request/response parsing, SSDP packet parsing, XML node handling, URI/URL parsing,
  GENA subscription lifetime (SID handling, subscriber lists, timeouts), and string utilities in
  `src/mupnp/util/string*.c` that these use.
- Also in scope: thread, list and dictionary code in `src/mupnp/util/` when misuse can be triggered from the
  network (races, use-after-free between server threads and API calls).
- Lower priority: `src/mupnp/io/` file helpers, `src/mupnp/util/log.c`, `time_wince.c`.
- Out of scope: `examples/` (sample programs, not shipped as a product), `wrapper/` (Objective-C wrapper),
  `test/`, `lib/` build scaffolding, Windows CE and other legacy platform code paths that are not built on
  Linux, ESP32, macOS or Windows (MinGW).

## How to exercise it

The image contains two CMake trees (see the Dockerfile header):

- `/src/build` (gcc, release with debug info) and `/src/build-asan` (clang, ASan + UBSan).
- `test/mupnptest` in each tree is the Boost.Test unit suite; `ctest --test-dir /src/build-asan` runs it
  with sanitizers. Some cases (e.g. `NetworkInterface`) need a non-loopback network interface and fail when
  only `lo` is up; that is an environment limitation, not a bug.
- `test/mupnp_alloc_failure_test` injects allocation failures into XML parsing.
- `examples/upnpdump/upnpdump` (control point) and `examples/clock/upnpclock` and
  `examples/binarylight/upnpbinarylight` (devices) in each tree are convenient drivers for end-to-end tests
  over loopback.
- `test/espidf/host/*.sh` build small ASan/UBSan harnesses for thread lifecycle, socket shutdown and XML
  allocation failure.
- There are no fuzz targets in the tree yet. Small harnesses that feed bytes to
  `mupnp_http_request_read`/`mupnp_http_response_read` over a socketpair, to `mupnp_ssdp_packet_*`, or to
  `mupnp_xml_parse` are welcome, and a harness worth keeping can be proposed as part of a patch.

## How you rate severity

- Critical: memory corruption (heap/stack overflow, use-after-free, double free) reachable by an
  unauthenticated network peer with attacker-controlled data and a plausible path to code execution.
- High: any other memory-safety bug reachable from the network (out-of-bounds read that leaks memory,
  use-after-free without demonstrated control), or a crash of a device or control point triggered by a
  single SSDP datagram or HTTP request.
- Medium: denial of service that needs sustained traffic (unbounded memory or thread growth, subscription
  table exhaustion), infinite loops on malformed input, SSRF-style behaviour where a control point fetches
  attacker-chosen URLs beyond what the UPnP specification requires.
- Low: bugs reachable only through local API misuse by the embedding application, or only on platforms
  that are not built by CI.
- UPnP has no authentication by design; "an unauthenticated peer can invoke actions or subscribe to
  events" is expected behaviour and not a finding on its own.

Please include a reproducer (a byte sequence, a small C program, or a test case) and, where possible, a
patch in the style of the surrounding code (`.clang-format` is in the repository root).

## Anything to leave alone

- Memory leaks on process exit, and leaks in `examples/`.
- Findings that depend only on `CLOG_DEBUG` logging paths.
- Calls to typed destructors through `void (*)(void *)` in `src/mupnp/util/list.c` (what clang's
  `-fsanitize=function` reports). This is the library's list design; the sanitizer build turns that check off.
- Absence of TLS, authentication or access control in the UPnP protocols themselves.
