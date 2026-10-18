# Signal-free native POSIX shutdown

Native POSIX thread stop no longer installs a SIGQUIT handler, rewrites worker
signal masks, or sends a process signal. Condition wait stop retains its explicit
condition notification/repeated wakeup. Managed advertiser/notification sleeps
observe runnable state using monotonic 100ms slices. External callers still get
a synchronous wait; arbitrary user callbacks must cooperate with the runnable
flag and must not hold locks required by their stopping owner.

The C socket wrapper uses monotonic readiness deadlines and native poll with
100ms slices. ESP-IDF keeps its existing select backend. Native poll supports
valid descriptors above FD_SETSIZE. Descriptor flags are preserved: explicit
caller O_NONBLOCK retains immediate I/O behavior. Plain send/receive uses
MSG_DONTWAIT; sends also use a bounded native syscall timeout, restoring the
original SO_SNDTIMEO immediately after each call. The original timeout determines
the overall operation deadline, including caller-supplied subsecond values.
Native timeout errors remain EAGAIN/EWOULDBLOCK; connect deadlines use ETIMEDOUT;
cooperative cancellation uses EINTR. The socket stays owned/alive until its
worker finishes; shutdown does not close a descriptor from another thread.

Accept/connect and serialized SSL operations temporarily use nonblocking mode
and restore original flags. Accepted children retain their original listener's
blocking contract across the BSD/Linux inheritance difference. SSL retries the
same operation/buffer for WANT_READ or WANT_WRITE, uses that requested readiness
direction, attempts buffered plaintext before waiting, and retains the existing
peer-chain and DNS/IP verification. No new public function or struct field is
introduced. TLS objects/descriptor flags must not be concurrently mutated by a
caller while an operation owns them. A failed write still reports failure, not
verified delivery, even if a prefix reached the local socket.

This trades periodic idle readiness wakeups for bounded cooperative cancellation
without adding wake-pipe fields to the public thread structure. Windows and
legacy non-POSIX stop implementations are unchanged. System DNS resolution and
arbitrary external callbacks remain outside the interruptible socket wrapper;
this is not forced cancellation of user code.

See [POSIX regressions](../test/posix/README.md). OpenSSL retry constraints follow
[SSL_get_error](https://docs.openssl.org/3.0/man3/SSL_get_error/) and
[SSL_write](https://docs.openssl.org/3.0/man3/SSL_write/).
