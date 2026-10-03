#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT HUP INT TERM
# Override SANITIZERS='' when the host compiler lacks sanitizer support.
flags=${SANITIZERS--fsanitize=address,undefined}
${CC:-cc} -std=gnu11 -Wall -Wextra -Werror -g -pthread -DESP_PLATFORM $flags \
  -I"$root/test/espidf/host/shims" -I"$root/include" \
  "$root/test/espidf/host/thread_lifecycle_test.c" \
  "$root/src/mupnp/util/thread.c" "$root/src/mupnp/util/time.c" \
  "$root/src/mupnp/util/cond.c" "$root/src/mupnp/util/mutex.c" \
  "$root/src/mupnp/util/list.c" -Wl,--wrap=pthread_create -Wl,--wrap=malloc -Wl,--wrap=free \
  -o "$build/thread_lifecycle_test"
"$build/thread_lifecycle_test"
