# Copilot Instructions for mUPnP for C

mUPnP for C is a cross-platform UPnP™ library written in C. It implements SSDP (discovery), SOAP (control), GENA (events), and HTTP as protocol stacks on top of a portable networking/threading/XML foundation.

## Build Commands

**Autotools (primary):**
```bash
./bootstrap          # Regenerate autotools config (after modifying Makefile.am or configure.ac)
./configure          # Standard config
./configure_linux    # Linux: adds --enable-libxml2
./configure_macosx   # macOS: sets CPPFLAGS/LDFLAGS for Homebrew paths
make
make install
```

**CMake (alternative):**
```bash
cmake .
make
```

**Key configure flags:**
- `--enable-test` — compile test suite
- `--enable-examples` — compile example programs
- `--enable-libxml2` — use Libxml2 instead of Expat (default)
- `--enable-code-coverage` — enable lcov coverage support
- `--enable-clog-debug` — enable debug log output

**CMake equivalents:** `-DCG_XMLPARSER_LIBXML2=ON`, `-DCG_USE_OPENSSL=ON`, `-DCG_HTTP_CURL=ON`

## Tests

```bash
./configure --enable-test
make check                    # Run full test suite (CppUnit)
make check-code-coverage      # Generate lcov coverage report
./coverage                    # Full clean → configure → build → test → coverage report
```

Tests live in `test/unix/`. Each test class (e.g., `HttpTest.cpp`, `DeviceTest.cpp`) is compiled into a single binary `mupnptest`. To run a single test case, build and invoke `mupnptest` directly with CppUnit test path filtering.

Coverage output: `coverage-report/index.html` and `.info` files for codecov.

## Lint / Format

```bash
./clang-format               # Runs clang-tidy --fix then clang-format -i on all source files
```

Style is enforced by `.clang-format`:
- Based on WebKit style, 2-space indent, Stroustrup braces
- Run before committing changes to `src/` or `include/`

## Architecture

The library is organized into two planes:

**Public API (`include/mupnp/`)** — headers are the stable interface:
- `device.h` / `controlpoint.h` — top-level objects applications instantiate
- `service.h`, `action.h`, `statevariable.h` — UPnP service model
- `icon.h` — device icon support

**Implementation (`src/mupnp/`)** — organized by protocol sub-system:
| Directory | Responsibility |
|-----------|---------------|
| `http/`   | HTTP client/server, headers, persistent connections |
| `soap/`   | SOAP envelope, request/response |
| `ssdp/`   | SSDP discovery, M-SEARCH, multicast/unicast sockets |
| `event/`  | GENA subscriptions, notifications, subscriber lists |
| `control/`| Action invocation, state variable queries |
| `net/`    | Socket abstraction, network interface enumeration, URL/URI |
| `xml/`    | XML parsing abstraction over Expat or Libxml2 |
| `util/`   | Strings, linked lists, dictionaries, threads, mutexes, logging |
| `io/`     | Optional file I/O |

Device and control-point internals are in `device_*.c` / `controlpoint_*.c` files alongside `device.c` and `controlpoint.c`.

## Key Conventions

**Naming:**
- Types: `mUpnpXxx` (e.g., `mUpnpDevice`, `mUpnpService`)
- Functions: `mupnp_module_verb()` (e.g., `mupnp_device_new()`, `mupnp_device_setfriendlyname()`)
- Constants/macros: `MUPNP_XXX_YYY` (all caps)
- Constructors: `mupnp_xxx_new()` — allocates and zero-initializes
- Destructors: `mupnp_xxx_delete()` — frees object and its owned children

**Coding patterns:**
- Null checks use Yoda style: `if (NULL != ptr)` / `if (NULL == ptr)`
- Every function starts with `mupnp_log_debug_l4("Entering...\n");` and ends with `mupnp_log_debug_l4("Exiting...\n");`
- Collections are intrusive linked lists; collection types are named `mUpnpXxxList` with `mupnp_xxxlist_*` functions
- Callbacks use typedef'd function pointer types (e.g., `MUPNP_DEVICE_LISTENER`)
- Platform-specific code is gated with `#if defined(WIN32)`, `#if defined(__APPLE__)`, etc.

**Include order:**
1. System headers (`<stdio.h>`, `<stdlib.h>`)
2. mUPnP headers (`<mupnp/typedef.h>`, `<mupnp/util/list.h>`, …)
3. Local/module-private headers

**XML-driven design:** UPnP device and service descriptions are parsed from XML at runtime. The XML abstraction layer (`xml/`) can swap between Expat (default) and Libxml2 at compile time without changing calling code.

## Objective-C Wrapper

`wrapper/objc/` provides an iOS/macOS Objective-C wrapper around the core C library. Xcode projects are in `wrapper/objc/xcode/`. Only control point functionality is currently wrapped.

## Examples

`examples/` contains working programs:
- `clock/` — a simple UPnP clock device
- `upnpdump/` — prints all UPnP devices found via SSDP
- `upnpavdump/` — control point for UPnP/AV media servers
- `upnpigddump/` — control point for Internet Gateway Devices (routers)

Build with `./configure --enable-examples && make`.
