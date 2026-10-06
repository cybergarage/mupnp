#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT HUP INT TERM
# Override SANITIZERS='' when the host compiler lacks sanitizer support.
flags=${SANITIZERS--fsanitize=address,undefined}
# The test itself is held to -Werror; the library sources are compiled as in
# the host build, so their existing warnings do not fail this check.
${CC:-cc} -std=gnu11 -Wall -Wextra -Werror -g -pthread $flags \
  -I"$root/include" -c "$root/test/espidf/host/xml_alloc_failure_test.c" \
  -o "$build/xml_alloc_failure_test.o"
${CC:-cc} -std=gnu11 -g -pthread $flags -I"$root/include" \
  "$build/xml_alloc_failure_test.o" \
  "$root/src/mupnp/xml/xml_attribute.c" "$root/src/mupnp/xml/xml_attribute_list.c" \
  "$root/src/mupnp/xml/xml_node.c" "$root/src/mupnp/xml/xml_node_list.c" \
  "$root/src/mupnp/xml/xml_parser.c" "$root/src/mupnp/xml/xml_parser_expat.c" \
  "$root/src/mupnp/xml/xml_function.c" "$root/src/mupnp/util/string.c" \
  "$root/src/mupnp/util/string_function.c" "$root/src/mupnp/util/list.c" \
  "$root/src/mupnp/util/log.c" "$root/src/mupnp/util/mutex.c" -lexpat \
  -Wl,--wrap=malloc -Wl,--wrap=realloc \
  -o "$build/xml_alloc_failure_test"
"$build/xml_alloc_failure_test"
