#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT HUP INT TERM
cd "$root"
./CMakeLists.txt.build > "$build/CMakeLists.txt"
cmp CMakeLists.txt "$build/CMakeLists.txt"
(cd test/unix && ./Makefile.am.build) > "$build/Makefile.am"
if grep -q 'espidf/' "$build/Makefile.am"; then
  echo 'Standalone ESP regression programs leaked into the host test binary' >&2
  exit 1
fi
grep -q '../HttpTest.cpp' "$build/Makefile.am"
grep -q '../TestMain.cpp' "$build/Makefile.am"
echo 'Build generator regression checks passed'
