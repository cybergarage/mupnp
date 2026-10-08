# StateVariable startup stall investigation

## Finding (2026-10-08)

Investigated PR #46 at `3d84d76f3bebc3017601ca29eac870ffe6fe7c6a`.
The existing `StateVariable` test starts a network device; the new
`StateVariableDefaultValueAndDataType` test only parses SCPD XML.
Neither the SCPD accessors nor either existing test is changed here.

A C probe using the actual `test/TestDevice.c` and library reproduced the
startup stall in the current restricted Linux environment:

```text
getifaddrs failed: errno=1 (Operation not permitted)
interfaces=0
before start, port=38400
http_open calls=1 port=38400 result=0
http_open calls=2 port=38401 result=0
http_open calls=3 port=38402 result=0
http_open calls=30000 port=68399 result=0
```

The original library made over 14 million HTTP-open attempts in three seconds;
the external timeout returned 124. Linker wrapping recorded calls to the real
`mupnp_http_serverlist_open()` without modifying its behavior. `strace` could
not run because this environment prohibits ptrace.

The causal path is:

1. `upnp_test_device_new()` parses an in-memory device description and the root
   service's SCPD, then installs listeners. It does not start network services.
2. `mupnp_device_start()` first stops the device, then populates `dev->ifCache`.
3. On Linux, `mupnp_net_gethostinterfaces()` returns an empty list when
   `getifaddrs()` fails. It also excludes down and loopback interfaces; an
   isolated host with no eligible addresses can reach the same result.
4. `mupnp_http_serverlist_open()` enumerates interfaces separately. With no
   interfaces, it returns false without opening a listening socket.
5. The non-ESP device-start path retries forever, incrementing the integer
   HTTP port even beyond 65535. It does not reach the state update, the test's
   explicit stop, or its final delete.

This establishes a reproducible cause of the observed type of stall. There
is no syscall trace of the October 7 run, so its exact runtime cause cannot
be proved retrospectively. A different host could still block elsewhere.

## Minimal change and regression

Extend the existing ESP empty-interface check to all platforms. If the cached
interface list is empty, startup returns false before attempting HTTP ports.
The normal startup and shutdown paths are otherwise unchanged.

`test/unix/device-start-no-interfaces.sh` injects an empty interface list using
Linux linker wrapping. It uses the real TestDevice fixture and device lifecycle.
An HTTP-open attempt fails the test immediately, so the old implementation
produces a clear failure instead of hanging the test runner. The test checks
failed startup, unchanged port, inactive advertiser, empty server lists, two
successive attempts, stop, and delete. Automake and its source generator both
include this test. Non-Linux hosts skip this GNU-linker-specific check.

Validated locally:

- Built the current C library from source with GCC and Expat.
- Regression against the original `device.c`: exit 1, "HTTP port retry reached
  with no usable interfaces".
- Regression against the fixed library: exit 0, including retry/stop/delete.
- Real environment probe after the fix: start returns false, port remains
  38400, and delete completes within the five-second deadline.
- Checked linking/running the regression with `device.c` instrumented for
  coverage and `CODE_COVERAGE_LIBS=--coverage`.
- Shell syntax and `git diff --check` passed.

The current environment lacks Boost.Test, Autoconf/Automake, and clang-format;
the package installation attempt could not locate those packages. The old
Boost test executable cannot load `libboost_unit_test_framework.so.1.83.0`.
Consequently, this investigation does not claim a fresh full Boost suite,
full coverage run, or formatter run. The C build used the checked-in configure
script, an explicit `./config.status config.h`, and suppressed regeneration of
Autotools outputs. It is not a substitute for the normal CI build below.

The original `StateVariable` test will report a startup assertion failure in an
environment with no usable interfaces after this fix; that environment still
cannot run network integration tests successfully. It will no longer spin in
this particular retry loop. Persistent bind failures with a nonempty interface
list, or interfaces disappearing after the cache check, remain separate cases;
this change does not redesign the HTTP retry policy.

## Compare a developer host with CI

PR #46's build run `37639510483`, job `112854401345`, passed `make check` on
Ubuntu 24.04.5. Its runner image was `ubuntu24/20261004.327`. The test executable
ran for approximately 37 seconds after linking, and ran successfully again
during coverage generation. The published job log does not record interface
inventory, so a particular CI NIC configuration is not established.

The ESP-IDF success is additional, separate evidence: its host regressions and
ESP32 cross-build do not run the ordinary Boost `StateVariable` case.

On a normal development host and on a disposable CI runner, collect:

```sh
git rev-parse HEAD
uname -a
cat /etc/os-release
cc --version
c++ --version
ip -details address show
ip route show
cat /proc/net/dev
ldd test/unix/mupnptest
```

Record actual interface enumeration failures as well as interface inventory.
An `ip` or `getifaddrs` permission error is evidence, not evidence that the
machine has no physical NIC. Do not remove host security restrictions to make
the tests pass; use an ordinary permitted developer host or CI runner.

Build with CI's settings (requires its documented build dependencies):

```sh
./bootstrap
./configure --enable-test --enable-code-coverage --enable-examples
make -j4
make -C test/unix mupnptest -j4
timeout -k 5 60 test/unix/mupnptest --run_test=StateVariable \
  --log_level=all --report_level=detailed --color_output=no
timeout -k 5 20 test/unix/mupnptest \
  --run_test=StateVariableDefaultValueAndDataType \
  --log_level=all --report_level=detailed --color_output=no
timeout -k 5 180 make check -j4
make check-code-coverage
```

Use GNU `timeout` (`gtimeout` on macOS). Record each exit code and preserve
`test/unix/test-suite.log` and individual test logs even after failure.
The Automake summary counts test programs/scripts, not Boost assertions.
`make -j4` versus CI's `-j20` controls build/test-driver concurrency; it does not
itself make individual Boost cases run in parallel. A 20-second whole-suite
timeout is not sufficient evidence of deadlock when CI takes about 37 seconds.

Run just the deterministic regression from a configured build tree:

```sh
cd test/unix
make check TESTS=device-start-no-interfaces.sh
```

If the focused StateVariable case still stalls on a permitted host, use:

```sh
timeout -k 5 30 strace -f -tt -T -o statevariable.strace \
  test/unix/mupnptest --run_test=StateVariable --log_level=all
gdb --args test/unix/mupnptest --run_test=StateVariable --log_level=all
```

In GDB, `run`, interrupt once stalled, then collect `thread apply all bt full`.
Break on `mupnp_device_start`, `mupnp_http_serverlist_open`,
`mupnp_device_advertiser_start`, `mupnp_device_advertiser_stop`,
`mupnp_http_serverlist_stop`, and `mupnp_ssdp_serverlist_stop` to distinguish
startup, advertising, and shutdown phases. Remember that start itself calls
stop before opening any services. At an HTTP-open breakpoint inspect `port`
and the caller's `dev->ifCache`; repeated enumeration failures with rising
ports identify this issue. Repeated `bind` failures with eligible interfaces
identify a different retry problem. A blocked main thread in condition/join
waiting requires all worker stacks (HTTP accept, SSDP receive, advertiser),
not only the main-thread stack.

Where ptrace is unavailable, use flushed stderr markers before and after
device creation, start, state update, stop, and delete in a temporary probe,
and linker wrapping as used by the regression. Avoid committed timing sleeps
or disabling the StateVariable test merely to make the suite appear green.
