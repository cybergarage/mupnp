# Native POSIX shutdown characterization

Run from the repository root:

```sh
test/unix/thread-shutdown.sh
test/unix/thread-shutdown.sh --require-signal-free
```

The default runner uses ASan/UBSan. Set `SANITIZERS=` for an unsanitized run.
Linux and macOS are supported; other hosts return Automake's skip code 77.
The normal mode is included in `make check` and a Linux/macOS CI matrix.

`ThreadShutdownTest.c` exercises four waits, each with 25 starts/stops on the
same worker and wait object: condition wait, stream receive through a local
socketpair, datagram receive through a local socketpair, and TCP accept on an
idle ephemeral listener bound only to 127.0.0.1. No multicast, household discovery,
HTTP/SOAP requests or appliance actions are issued. Each cycle calls stop again,
asserts the worker completed before cleanup, and verifies that repeated stop
sends no further signal. A per-cycle five-second watchdog bounds hangs; measured
stop must finish within two seconds. Socket workers announce entry immediately
before blocking, followed by a short scheduling window; the tests do not claim
kernel scheduler determinism. Condition tests synchronize through the wait mutex.

The standalone runner compiles the unchanged `thread.c` with only its
`pthread_kill` call renamed to a test recorder. That recorder counts signals,
then calls the real `pthread_kill` with the original target and signal. It does
not replace the library's SIGQUIT handler, block/ignore SIGQUIT, modify debugger
settings, cancel workers or change production library objects. Only this test's
SIGALRM watchdog has a test-owned handler. The standalone main is excluded from
the Boost binary by the maintained Makefile generator.

## Baseline result (master 2ab9c0db)

On macOS arm64 and Linux arm64 (official GCC 14 container, network disabled),
normal mode passed 100 lifecycle cycles with ASan/UBSan. Each wait kind sent
25 SIGQUITs, total 100. Repeated stops sent none. An additional macOS run without
sanitizers also passed. The explicit signal-free mode failed with exit 1 on both
hosts after completing all lifecycle assertions:

```text
OBSERVED: 100 offline lifecycle cycles; SIGQUIT sent=100 (not 100 app Search presses)
FAIL: signal-free shutdown contract violated by SIGQUIT delivery
```

Normal mode intentionally characterizes the current behavior; it does not assert
signal-free shutdown. The opt-in stricter contract records the known failure for
a future implementation fix without making ordinary CI knowingly red. A future
fix should make the strict mode pass and then promote it to the default gate.
Socket/TLS/thread runtime source is unchanged in this test-only change. TLS,
Windows, ESP hardware and a complete cross-platform shutdown fix are unverified.

The new Automake test also passed on macOS using:

```sh
make -C /tmp/mupnp-posix-makecheck/test/unix -o Makefile check \
  TESTS=thread-shutdown.sh check_PROGRAMS=
```

This is a targeted integration run, not the full existing Boost/LAN test suite.
Build-generator checks, shell syntax and scoped clang-format validation passed.
The source Makefile generator and shipped Makefile.in include the standalone
test; no new main() is linked into the Boost binary. Full bootstrap was not run:
the host has no Automake, and the available container Automake 1.17 rejects the
repository's 1.16.5 macro version. The shipped test template was kept in sync
and used successfully for the out-of-tree targeted make-check run.

## Separate actual-app evidence

Before this test-only branch, HomeKit Search was pressed three times under a
debugger: two manually attached LLDB sessions and one Xcode 27 Run session.
All three stopped on SIGQUIT in mUPnP shutdown. Xcode Run specifically displayed
`Thread 13: signal SIGQUIT` at `__psynch_cvwait + 8`; the concurrent sender stack
was DeviceModelSession cleanup → UPnPDiscoveryWorker.stop → controlpoint stop →
thread stop. These are three actual interactions, not 100 Search presses. None
of those post-press scans was continued through the stop. Household IGD identity
and successful rescan completion remain unverified. This branch requires no
screen use and does not change HomeKit PR22's released dependency pins.
