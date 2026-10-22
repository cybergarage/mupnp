# Cooperative POSIX shutdown regressions

From the repository root:

```sh
test/unix/thread-shutdown.sh --require-signal-free
test/unix/socket-wait.sh
```

Both runners default to ASan/UBSan (`SANITIZERS=` disables instrumentation).
They are registered with `make check` and the Linux/macOS CI matrix. Other
platforms return skip code 77; the TLS runner also skips when OpenSSL development
files are absent. CI installs the fixture dependencies explicitly.

## Signal-free lifecycle contract

`ThreadShutdownTest.c` runs eight waits, each with 25 starts/stops on the same
worker and wait object (200 cycles): condition wait, stream/datagram receive,
loopback accept, backpressured write, an injected pending connect on a full local
stream, long advertiser-style sleep and a large partially progressing write.
Each cycle checks completion and a repeated stop, with a five-second watchdog
and a two-second stop bound. Stream/datagram pairs are local AF_UNIX fixtures;
the accept listener is ephemeral and bound only to 127.0.0.1. The pending-connect
fixture forwards all other connects and never probes an unreachable LAN target.

Only the test build renames thread.c's `pthread_kill` call to a recorder that
forwards the original signal/target. The test also installs an application-owned
SIGQUIT disposition before starting library workers and verifies it remains
unchanged and receives no signals, then restores it. No debugger is configured
to ignore a signal. No process cancellation or forced worker termination is used.
Default mode now requires zero SIGQUIT, as does `--require-signal-free`; the
previous known-failure contract from PR55 is promoted to the ordinary gate.

## Socket and TLS contracts

`SocketWaitTest.c` checks native EAGAIN timeouts, caller-supplied O_NONBLOCK and
subsecond SO_RCVTIMEO, zero-length/partial reads, EOF, refused connect and high
file descriptors. Local TLS fixtures verify handshake/read/backpressured-write
cancellation, handshake/read deadlines, buffered decrypted data and close-notify
EOF. Correctly trusted peers work; untrusted and wrong-IP certificates fail.
Fixture keys/certificates are generated in the runner's private temporary build
directory and removed on exit. SSL_CERT_FILE applies only to each test process;
no certificate is installed and OS trust/security settings are unchanged.

The maintained Makefile generator excludes these standalone main() programs
from the Boost binary. Linux and macOS sanitizer runs are separate from ESP host
regressions, native C/ObjC XML/HTTP fixtures, and actual app interaction evidence.
No multicast, appliance action or household discovery is issued by these tests.

## Baseline and actual-app evidence

PR55 on master 2ab9c0db characterized 100 lifecycle cycles with 100 SIGQUIT sends
on both Linux and macOS. Its opt-in signal-free mode failed with exit 1. Those
results are historical, not the expected result of the cooperative runtime.

Before this fix, three actual HomeKit Search presses under a debugger (two
manual LLDB attaches and one Xcode 27 Run) stopped on SIGQUIT. Xcode showed
Thread 13 at __psynch_cvwait; another thread was in DeviceModelSession cleanup →
UPnPDiscoveryWorker.stop → controlpoint stop → thread stop. These are three
actual interactions, not 100 or 200 UI searches. Candidate-framework Xcode checks
are recorded separately from these headless fixtures. Formal HomeKit dependency
pins must remain on released versions until upstream integration is reviewed.
