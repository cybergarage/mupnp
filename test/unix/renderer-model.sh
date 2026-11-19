#!/bin/sh
# Select only model/loopback checks; no physical-interface discovery.
set -eu
command -v cmake >/dev/null 2>&1 || { echo 'SKIP: CMake is required'; exit 77; }
command -v python3 >/dev/null 2>&1 || { echo 'SKIP: Python 3 is required'; exit 77; }
root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT HUP INT TERM
cmake -S "$root" -B "$build" -DMUPNP_ENABLE_RENDERER_MODEL=ON \
  -DMUPNP_ENABLE_EXAMPLES=ON -DMUPNP_ENABLE_TEST=OFF
cmake --build "$build"
ctest --test-dir "$build" -R '^(renderer_model|avchk_loopback)$' --output-on-failure
