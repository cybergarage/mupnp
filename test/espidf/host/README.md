# ESP-IDF host regression tests

Run from the repository root on Linux with a C11 compiler and pthreads:

```sh
test/espidf/host/check_generators.sh
test/espidf/host/run.sh
test/espidf/host/run_socket.sh
test/espidf/host/run_xml_alloc.sh
```

This builds the actual `ESP_PLATFORM` thread, time, condition, mutex and list
implementations. The only IDF shims are the monotonic timer and stack-size
configuration. This is a lifecycle regression test, not an emulator or a
substitute for an ESP-IDF target build and device tests.

AddressSanitizer and UndefinedBehaviorSanitizer are enabled by default. Set
`SANITIZERS=''` for an unsanitized run or choose another supported compiler with
`CC`. In ptrace-based sandboxes, LeakSanitizer cannot run; use:

```sh
ASAN_OPTIONS=detect_leaks=0 test/espidf/host/run.sh
```

The test independently accounts for library object allocation/free, even when
LeakSanitizer is disabled. It covers:

- TLS lookup from a foreign task before any library worker starts
- Invalid starts, pthread creation failure and recovery
- Repeated start/stop, natural completion/reuse and concurrent stop requests
- Immediate interruption of a 24-hour sleep, without truncating the duration
- Lost condition signals before waiting and stop during an indefinite wait
- Preservation of a timed condition wait's overall deadline
- Deferred self-deletion and release of every library object

ESP-IDF lifecycle behavior: externally owned workers remain joinable and stop
waits for their action to return. Actions must cooperate by checking
`mupnp_thread_isrunnable()` and avoiding unbounded application-specific blocking
calls. Self-owned workers may call `mupnp_thread_delete()` at the end of their
action; the wrapper frees them after the action returns. Do not race a
self-owned worker's deletion with external access/deletion.

ESP-IDF's `pthread_join()` uses the caller's default FreeRTOS task-notification
slot. Use event groups, queues, semaphores or a separate indexed notification
slot for application notifications during shutdown.

The generator check ensures `bootstrap` preserves the ESP component CMake
branch and keeps standalone ESP test programs out of the normal Boost test
binary. The socket suite checks cooperative TCP/UDP receive cancellation and
socket timeouts configured before a descriptor is created, using host socket
pairs rather than a live network. These tests do not emulate lwIP.
