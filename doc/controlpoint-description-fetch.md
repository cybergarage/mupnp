# Control point description fetch lifecycle

SSDP response and notification workers previously held the control point's
cache mutex while downloading a description and every SCPD. An unreachable
LOCATION could therefore prevent even an owned Objective-C device snapshot
from being made for the HTTP timeout. Moving the caller to another queue does
not remove that cache lock contention.

The control point now reserves an owned packet and a pending-fetch entry under
its mutex, then performs HTTP/XML/SCPD work on a private device tree outside
that mutex, on the existing synchronous SSDP worker. No cached device pointer
is dereferenced across that unlocked interval and no new worker pool is added.

Before publishing, it reacquires the mutex and checks the stop/discovery epoch,
target UDN membership, pending reservation, and cached LOCATION. Concurrent
announcements for the same LOCATION share the in-flight operation. A different
LOCATION for a known root supersedes its older pending operation; ordinary
announcements for the current cached LOCATION just refresh its SSDP data.
Embedded targets are resolved to their owning root before cache updates, and a
second root-UDN lookup prevents duplicate root insertion.

A no-I/O device commit retains service pointers for an unchanged structure and
CONFIGID. A changed structure carries GENA subscription SID, timeout, timestamp
and event key to services matched by owning-device UDN and service ID. Failed
alternate descriptions leave the working cached tree and its expiration intact.
Candidates are fully parsed before any tree swap; their cleanup owns the old
tree after a swap.

Stop marks the control point as stopping and invalidates unpublished work before
joining the existing SSDP workers. Their existing cooperative socket/libcurl
cancellation remains in use. It waits for outstanding synchronous fetch scopes
before clearing the cache. A direct non-SDK caller has no library worker stop
flag; its own HTTP operation must return (or reach its existing timeout) before
that join completes. A byebye conservatively invalidates outstanding candidates
before removal, including candidates whose root hierarchy is not known yet.
A subsequent start opens a fresh discovery epoch. No SIGQUIT policy is changed.

## Regression coverage

`test/DiscoveryFetchTest.cpp` uses ephemeral loopback TCP servers with held HTTP
responses. It checks cache access during HTTP waits, duplicate root/embedded
notifications, byebye and stop during a fetch, retry, unrelated UDN rejection,
owned root/child identity on an alternate LOCATION, failed alternate LOCATION
retention, superseded updates, and cooperative SDK worker cancellation.

Build and run with CMake (`MUPNP_ENABLE_TEST=ON`). Run the full native suite and
`DiscoveryFetch*` under ASan/UBSan and ThreadSanitizer as appropriate. Existing
`LocationChange*` exercises real subscription, notification, renewal and
unsubscribe against the test-owned device. Run multicast fixture suites
serially: their shared test UDN and default service ports collide across
processes. CMake and autotools include the new tests; autotools now explicitly
includes the existing LOCATION regression source too.

The curl persistent-connection implementation still serializes HTTP requests
with its own transport mutex. This patch removes cache-mutex contention and does
not change that transport policy. The existing curl
`LocationChangeWithNewDescriptionCarriesSid` failure was also reproduced with
the previously built released 3.1.3 baseline; it remains a separate limitation.

This is an unreleased mUPnP candidate. HomeKit's released framework pin and the
HomeKit search/lighting PRs are independent. Real LAN probes use SSDP and HTTP
reads only; no household identifiers or endpoint addresses belong in public
validation artifacts. GUI responsiveness and application cancellation require
an unlocked desktop and must not be inferred from CLI heartbeat measurements.

## Candidate validation

The final native suite passed all 63 cases under ASan/UBSan. Focused runs passed
7 loopback cases under ThreadSanitizer, 9 fetch/LOCATION cases with libxml2 and
ASan/UBSan, 7 fetch cases through curl, and 7 with optimized control point mode.
Unsigned arm64 macOS and iOS Simulator Objective-C framework builds succeeded.
Focused LLVM coverage was collected separately; it is not full-suite coverage.
Local autotools `make check` / `make check-code-coverage` could not be run because
`automake` was absent. The PR CI remains the authority for those gates.

A read-only native-wrapper LAN comparison completed three snapshot searches:
owned snapshot calls took approximately 1.6–8.0 ms, with native stop calls across
four trials taking approximately 97–206 ms. A previous released-pin comparison
had observed an approximately 28-second snapshot wait. These were separate live
arrival sets, so no speedup factor or exhaustive device coverage is claimed.
No application GUI behavior was measured: the desktop automation service still
reported the Mac locked. CLI MainActor heartbeat is only a surrogate and is not
UI responsiveness evidence.
