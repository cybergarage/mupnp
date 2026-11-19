#!/bin/sh
# Native Foundation ownership/lifecycle test; start/stop calls are inert stubs.
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
build=$1
xcrun clang -c -fno-objc-arc -I"$root/include" -I"$root/wrapper/objc/mUPnP" \
  -Dcg_upnpav_dmr_new=renderer_test_new -Dcg_upnpav_dmr_delete=renderer_test_delete \
  -Dcg_upnpav_dmr_start=renderer_test_start -Dcg_upnpav_dmr_stop=renderer_test_stop \
  "$root/wrapper/objc/mUPnP/CGUpnpAvRenderer.m" -o "$build/RendererObjC.o"
xcrun clang -fno-objc-arc -I"$root/include" -I"$root/wrapper/objc/mUPnP" \
  "$root/test/RendererObjCTest.m" "$build/RendererObjC.o" "$build/libMUPNP.a" \
  -framework Foundation -lexpat -lxml2 -o "$build/renderer-objc-test"
"$build/renderer-objc-test"
