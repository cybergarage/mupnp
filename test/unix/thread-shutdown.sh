#!/bin/sh
# Standalone native POSIX regression: keep its main() outside mupnptest.
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
case $(uname -s) in
  Darwin) dead_strip=-Wl,-dead_strip ;;
  Linux) dead_strip=-Wl,--gc-sections ;;
  *) echo 'SKIP: this regression requires native POSIX sockets'; exit 77 ;;
esac
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT HUP INT TERM
sanitizers=${SANITIZERS--fsanitize=address,undefined}
# Intentionally split compiler/sanitizer flags, like the existing host runners.
${CC:-cc} -std=gnu11 -g -pthread $sanitizers -ffunction-sections -fdata-sections \
  -I"$root/include" -Dpthread_kill=mupnp_test_pthread_kill \
  -c "$root/src/mupnp/util/thread.c" -o "$build/thread.o"
${CC:-cc} -std=gnu11 -g -pthread $sanitizers -ffunction-sections -fdata-sections \
  -I"$root/include" -Dconnect=mupnp_test_connect \
  -c "$root/src/mupnp/net/socket.c" -o "$build/socket.o"
${CC:-cc} -std=gnu11 -g -pthread $sanitizers -ffunction-sections -fdata-sections "$dead_strip" \
  -I"$root/include" "$root/test/posix/ThreadShutdownTest.c" "$build/thread.o" \
  "$root/src/mupnp/util/time.c" "$root/src/mupnp/util/cond.c" \
  "$root/src/mupnp/util/mutex.c" "$root/src/mupnp/util/list.c" \
  "$root/src/mupnp/util/string.c" "$root/src/mupnp/util/string_function.c" \
  "$root/src/mupnp/util/log.c" "$build/socket.o" \
  "$root/src/mupnp/net/datagram_packet.c" -o "$build/thread-shutdown"
"$build/thread-shutdown" "$@"
