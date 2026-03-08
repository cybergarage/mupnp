# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## About

mUPnP for C is a C library implementing UPnP™ (Universal Plug and Play). It abstracts the underlying protocols (SSDP, SOAP, GENA, HTTP, HTTPU) and provides APIs for creating UPnP devices and control points.

## Build System

The project uses **Autotools** as its primary build system, with **CMake** as an alternative.

### Autotools (primary)

```sh
./bootstrap          # Generate configure script (needed after cloning or modifying configure.ac)
./configure
make
```

With tests and examples:
```sh
./configure --enable-test --enable-code-coverage --enable-examples
make -j 20
make check           # Run tests
```

Run a single test binary directly:
```sh
./test/unix/mupnptest
```

### CMake (alternative)

```sh
mkdir build && cd build
cmake ..
make
```

Key CMake options: `-DCG_XMLPARSER_LIBXML2=ON`, `-DCG_HTTP_CURL=ON`, `-DCG_USE_OPENSSL=ON`

### Dependencies

- **Required**: expat (default XML parser) or libxml2 (`--enable-libxml2`), pthreads
- **Optional**: libuuid, libcurl (`--enable-libcurl`), OpenSSL (`--enable-openssl`)
- **Tests**: Boost Unit Test Framework (`libboost-all-dev`)
- **CI setup**: `sudo apt install -y libboost-all-dev autoconf autoconf-archive lcov`

## Architecture

### UPnP Object Model

The library models UPnP as a hierarchy:
- **Device** (`src/mupnp/device.c`, `include/mupnp/device.h`) — UPnP device that advertises itself via SSDP and serves an HTTP description document
- **Service** (`src/mupnp/service.c`) — A device contains services; each service has actions and state variables
- **Action** (`src/mupnp/action.c`) — Remote procedure calls invoked via SOAP
- **StateVariable** (`src/mupnp/statevariable.c`) — Service state; evented variables trigger GENA notifications to subscribers
- **ControlPoint** (`src/mupnp/controlpoint.c`) — Client that discovers devices (M-SEARCH/SSDP), invokes actions (SOAP), and subscribes to events (GENA)

### Protocol Subsystems (`src/mupnp/` subdirectories)

| Directory | Responsibility |
|-----------|----------------|
| `ssdp/` | SSDP discovery (M-SEARCH, NOTIFY, multicast/unicast UDP) |
| `soap/` | SOAP request/response for action invocation |
| `http/` | HTTP server/client, persistent connections |
| `event/` | GENA eventing — subscriptions, notifications, property lists |
| `control/` | Action/query control flow (request, response, ctrl) |
| `net/` | Socket abstraction, network interfaces, URI/URL parsing |
| `xml/` | XML parser wrapper (Expat or libxml2), node/attribute trees |
| `util/` | Threads, mutexes, conditions, strings, lists, dictionary, logging |
| `io/` | File I/O utilities |

### Key Design Patterns

- **Linked lists**: Most collections (device list, service list, action list, etc.) use the generic `util/list.c` linked list. Each list element struct embeds a `mUpnpList` head.
- **Callbacks**: Device action handling and control point device/event listeners are registered as function pointers.
- **Thread-per-server**: HTTP servers, SSDP servers, and device advertisers each run in their own thread managed via `util/thread.c`.

### Headers

All public headers live under `include/mupnp/`. The single-include entry point for applications is `#include <mupnp/upnp.h>`.

## Tests

Tests are in `test/` and use the **Boost Unit Test Framework** (C++). Test sources mix C (TestDevice.c) and C++ (.cpp). The test binary is `test/unix/mupnptest`.

## Code Coverage

```sh
make check-code-coverage   # Generates mupnp-*-coverage.info
```
