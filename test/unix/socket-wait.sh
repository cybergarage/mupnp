#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
case $(uname -s) in
  Darwin) dead_strip=-Wl,-dead_strip ;;
  Linux) dead_strip=-Wl,--gc-sections ;;
  *) echo 'SKIP: native POSIX TLS test'; exit 77 ;;
esac
if ! pkg-config --exists openssl; then
  echo 'SKIP: OpenSSL development package is required'; exit 77
fi
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT HUP INT TERM
sanitizers=${SANITIZERS--fsanitize=address,undefined}
${CC:-cc} -std=gnu11 -g -pthread $sanitizers -ffunction-sections -fdata-sections \
  -I"$root/include" -Dpthread_kill=mupnp_test_pthread_kill \
  -c "$root/src/mupnp/util/thread.c" -o "$build/thread.o"
${CC:-cc} -std=gnu11 -g -pthread $sanitizers -ffunction-sections -fdata-sections "$dead_strip" \
  -DMUPNP_USE_OPENSSL -I"$root/include" $(pkg-config --cflags openssl) \
  "$root/test/posix/SocketWaitTest.c" "$build/thread.o" \
  "$root/src/mupnp/util/time.c" "$root/src/mupnp/util/cond.c" \
  "$root/src/mupnp/util/mutex.c" "$root/src/mupnp/util/list.c" \
  "$root/src/mupnp/util/string.c" "$root/src/mupnp/util/string_function.c" \
  "$root/src/mupnp/util/log.c" "$root/src/mupnp/net/socket.c" \
  "$root/src/mupnp/net/datagram_packet.c" $(pkg-config --libs openssl) -o "$build/socket-wait"
# Process-local fixture trust only; never install a certificate or alter OS trust.
openssl req -x509 -newkey rsa:2048 -nodes -days 1 -subj /CN=fixture \
  -addext subjectAltName=IP:127.0.0.1 -keyout "$build/key.pem" -out "$build/cert.pem" >/dev/null 2>&1
SSL_CERT_FILE="$build/cert.pem" "$build/socket-wait" "$build/cert.pem" "$build/key.pem"
SSL_CERT_FILE=/dev/null "$build/socket-wait" "$build/cert.pem" "$build/key.pem" reject-untrusted
openssl req -x509 -newkey rsa:2048 -nodes -days 1 -subj /CN=fixture \
  -addext subjectAltName=IP:127.0.0.2 -keyout "$build/wrong-key.pem" -out "$build/wrong-cert.pem" >/dev/null 2>&1
SSL_CERT_FILE="$build/wrong-cert.pem" "$build/socket-wait" "$build/wrong-cert.pem" "$build/wrong-key.pem" reject-identity
