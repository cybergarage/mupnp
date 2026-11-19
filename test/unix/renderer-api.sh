#!/bin/sh
# Select only inert renderer/lifecycle checks; no physical-interface discovery.
set -eu
command -v cmake >/dev/null 2>&1 || { echo 'SKIP: CMake is required'; exit 77; }
root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT HUP INT TERM
cmake -S "$root" -B "$build" -DMUPNP_ENABLE_RENDERER_TESTS=ON \
  -DMUPNP_ENABLE_EXAMPLES=ON -DMUPNP_ENABLE_TEST=OFF
cmake --build "$build"
ctest --test-dir "$build" -R '^(renderer_api|renderer_lifecycle|avchk_lifecycle)$' --output-on-failure

if [ "$(uname -s)" = Darwin ]; then
  "$root/test/unix/renderer-objc.sh" "$build"
fi
