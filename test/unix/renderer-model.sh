#!/bin/sh
# Explicitly select only the offline model. Never run discovery examples/tests.
set -eu
command -v cmake >/dev/null 2>&1 || { echo 'SKIP: CMake is required'; exit 77; }
root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT HUP INT TERM
cmake -S "$root" -B "$build" -DMUPNP_ENABLE_RENDERER_MODEL=ON \
  -DMUPNP_ENABLE_EXAMPLES=OFF -DMUPNP_ENABLE_TEST=OFF
cmake --build "$build" --target mupnp_renderer_model_test
ctest --test-dir "$build" -R '^renderer_model$' --output-on-failure
