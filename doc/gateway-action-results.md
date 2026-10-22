# Gateway ownership and independent action results

These additive APIs preserve the legacy recursive first-match lookups and
partial-update action API. The core is portable C; Objective-C only retains
owners, validates NSString conversion, and copies result values.

## Device identity

Keep `(root UDN, owning-device UDN, service ID)` together. Service IDs are scoped
to the owning device, so a recursive first match cannot identify multiple WAN
connections. Enumerate `embeddedDevices` recursively and inspect each device's
`deviceType` and direct `services`. Use `getOwnedServiceForID:` on an owner, or
`getServiceForID:owningDeviceUDN:` on a root/subtree. The latter includes the
receiver itself but never searches outside its subtree. `rootUDN` and
`CGUpnpService.owningDeviceUDN` expose the two identities explicitly.

C callers use existing device tree iterators and `mupnp_device_getrootdevice`,
plus `mupnp_device_getownedservicebyserviceid` and
`mupnp_device_getservicebyownerudn`. The pointers are borrowed. These new
lookups perform no I/O, including in optimized CP builds; load SCPDs explicitly
before invoking actions when that build mode requires it.

The [IGD:2 specification, sections 1.1 and 2.2](https://openconnectivity.org/wp-content/uploads/2015/11/UPnP-gw-InternetGatewayDevice-v2-Device-20100910.pdf)
permits multiple WANDevice and WANConnectionDevice instances. Traverse the
actual hierarchy and exact advertised service versions for IGD:1 and IGD:2;
do not infer a WAN connection from an AV service elsewhere in the same tree.
The API deliberately implements no preferred-WAN selection or gateway writes.

Embedded Objective-C wrappers retain their parent, services retain their owning
wrapper, and actions retain services. Discovery snapshots and their copied
SCPDs therefore outlive release of the root wrapper by its caller. Retaining a
borrowed `initWithCObject:` wrapper does not make externally owned C memory
owned: use `initWithDeviceSnapshot:` when crossing cache/lifetime boundaries.
As with existing wrappers, reparsing/replacing native objects invalidates
previous descendants; retaining them is not permission to mutate the tree.

## Strict action calls

`mupnp_action_postwitharguments(action, inputs)` accepts exactly the declared
input set, including present empty strings. It rejects unknown names, duplicate
entries, output arguments, missing inputs and invalid/duplicate SCPD argument
names/directions before I/O. NULL inputs is the empty set. It creates private
argument values and never changes the source action's values or status.

The returned `mUpnpActionResult` owns copied values and error strings. Free it
with `mupnp_action_result_delete`; it remains valid after action/device deletion.
`mupnp_action_result_fromresponse` provides the same decoding for offline HTTP
fixtures/transports. A NULL result indicates allocation failure.

| Outcome | Meaning |
| --- | --- |
| SUCCESS | Valid matching SOAP response, every declared output present |
| INVALID_ARGUMENTS | Rejected before sending |
| TRANSPORT_ERROR | No complete HTTP response; HTTP status reported as 0 |
| HTTP_ERROR | Non-2xx response without a decoded SOAP fault |
| SOAP_FAULT | SOAP fault; retain HTTP status, faultcode/faultstring, optional UPnP code/description |
| PARSE_ERROR | Empty/malformed/wrong SOAP response on 2xx, unknown/duplicate/non-output fields |
| INCOMPLETE | Valid response omits a declared output; only present outputs are returned |

SOAP fault detection applies even on HTTP 200. UPnP code 0 means absent/invalid;
its numeric space is distinct from HTTP status and the result outcome. Missing
output returns NULL in C / absent dictionary key in Objective-C; an empty XML
output element returns an empty string. A valid empty action response succeeds
when the action declares no outputs; an empty HTTP body is a parse error.
Malformed responses and faults have no output values.

The single-attempt HTTP path reports actual receive completion separately from
legacy synthetic HTTP error status. It bypasses SOAP's 405/M-POST fallback;
the curl backend uses a new connection/handle and does not reuse its connection
cache. No automatic write retry is performed. A transport failure after sending
can still mean the device executed the action; do not blindly retry writes.
Caller owns action/service/device lifetime and serializes description mutation
during a call. Result objects share no mutable action values.

Objective-C exposes `postResultWithArguments:` and immutable `CGUpnpActionResult`
with `outcome`, `httpStatusCode`, `upnpErrorCode`, fault strings and `outputs`.
The C result is freed after copying; no native pointers escape in this object.

## HomeKit migration (baseline 2cbd9d2)

`Sources/ConciergeCore/UPnPReadAdapter.swift` currently clears previous outputs
before `perform` and `browse`, then calls `post(withArguments:)`. After updating
the dependency pin, pass the complete catalog-derived input dictionary to
`postResult(withArguments:)`, check `.outcome == .success`, and read the returned
`outputs`. Remove the manual `setArgumentValue("", forName:)` loops. Handle an
absent requested output separately from a present empty string. Browse may pass
the independent outputs to its existing page decoder; this PR does not change
DIDL or paging behavior.

Objective-C equivalent:

```objc
CGUpnpService* service = [root getServiceForID:serviceID owningDeviceUDN:ownerUDN];
CGUpnpActionResult* result = [[service getActionForName:actionName]
    postResultWithArguments:completeInputs];
if (result && result.outcome == CGUpnpActionResultSuccess) {
  NSString* value = result.outputs[outputName]; // nil != @""
}
```

Carry root and owner UDNs in HomeKit action/binding identities before switching
lookup, including discovery of embedded devices. Continue the current action
allowlist, session/generation checks, and explicit confirmation for writes.
No HomeKit source files are changed by this PR.

## Validation and follow-ups

On macOS with CMake and Boost installed:

```sh
python3 test/security/verify-gateway.py /tmp/mupnp-gateway-asan
python3 test/security/verify-gateway.py /tmp/mupnp-gateway-libxml --libxml2
python3 test/security/verify-gateway.py /tmp/mupnp-gateway-curl --curl
```

These ASan/UBSan suites use local XML and 127.0.0.1 fixtures, and exclude the
existing multicast/discovery tests. The curl run also excludes the legacy
`HttpPostRejectsIncompleteChunkedResponse` case, whose synthetic status/body
expectations apply to the native backend; the new strict API separately tests
truncated HTTP responses on both backends. CI runs all three backends. C fixtures are
also included in CMake and Autotools test runners. Live gateways, full multicast
integration, and coverage-report generation are separate validation boundaries.

Future focused changes: DIDL decoding/paging, SCPD value constraints, and a
session/generation facade. They are intentionally outside this API increment.
