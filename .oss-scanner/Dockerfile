# OSS Scanner build for mUPnP for C (https://github.com/cybergarage/mupnp).
#
# The scanner builds this file with the mupnp checkout as the build context. The network is available only while this
# file builds; the scan itself runs offline.
#
# Two CMake trees are built from the same checkout in /src:
#
#   /src/build       gcc, RelWithDebInfo, -g -fno-omit-frame-pointer, Expat XML parser. Mirrors the CI build.
#                    Library:   /src/build/libMUPNP.a
#                    Tests:     /src/build/test/mupnptest (Boost.Test), /src/build/test/mupnp_alloc_failure_test
#                    Examples:  /src/build/examples/{clock,binarylight,upnpdump,upnpavdump,upnpigddump}/...
#
#   /src/build-asan  clang, AddressSanitizer + UndefinedBehaviorSanitizer, same layout as /src/build.
#                    Use it to confirm memory-safety findings (ctest --test-dir /src/build-asan, or run the binaries).
#                    -fsanitize=function is turned off: the list code calls typed destructors such as
#                    mupnp_xml_node_delete through void (*)(void *) on every list teardown, by design.
#
# The host-side regression scripts in test/espidf/host/ (run.sh, run_socket.sh, run_xml_alloc.sh) compile their own
# ASan/UBSan binaries with cc and need nothing beyond this image.
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive \
    LANG=C.UTF-8

RUN apt-get update && apt-get install -y --no-install-recommends \
      ca-certificates git build-essential clang libclang-rt-18-dev llvm cmake ninja-build pkg-config \
      autoconf autoconf-archive automake libtool \
      libexpat1-dev libxml2-dev libcurl4-openssl-dev libssl-dev uuid-dev libboost-test-dev \
      python3 gdb iproute2 \
    && rm -rf /var/lib/apt/lists/*

# scanner contract: the checkout lives inside the image, at /src
COPY . /src
WORKDIR /src

# 1) Reference build (gcc, debug info, frame pointers), with tests and examples.
RUN cmake -S /src -B /src/build -G Ninja \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_C_FLAGS="-g -fno-omit-frame-pointer" \
      -DCMAKE_CXX_FLAGS="-g -fno-omit-frame-pointer" \
      -DMUPNP_ENABLE_TEST=ON \
      -DMUPNP_ENABLE_EXAMPLES=ON \
      -DMUPNP_ENABLE_PLATFORM_SMOKE=ON \
      -DMUPNP_USE_CFILE=ON \
    && cmake --build /src/build -j"$(nproc)"

# 2) Sanitizer build (clang, ASan + UBSan), same targets.
RUN CC=clang CXX=clang++ cmake -S /src -B /src/build-asan -G Ninja \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_C_FLAGS="-g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize=function -fno-sanitize-recover=undefined" \
      -DCMAKE_CXX_FLAGS="-g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize=function -fno-sanitize-recover=undefined" \
      -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined" \
      -DMUPNP_ENABLE_TEST=ON \
      -DMUPNP_ENABLE_EXAMPLES=ON \
      -DMUPNP_ENABLE_PLATFORM_SMOKE=ON \
      -DMUPNP_USE_CFILE=ON \
    && cmake --build /src/build-asan -j"$(nproc)"

# Sanitizer defaults for anyone running the ASan binaries in this image.
ENV ASAN_OPTIONS=detect_leaks=1:abort_on_error=1:symbolize=1 \
    UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1

# Smoke tests so the log shows whether the image works. Some unit tests open sockets and enumerate network interfaces,
# so results can differ between the build container and the offline scan VM. Failures are printed but never fail the
# build. LeakSanitizer needs ptrace, which `docker build` does not grant, so leak detection is off in this step only.
RUN (timeout 5m ctest --test-dir /src/build --output-on-failure --timeout 120 \
       || echo "WARNING: ctest (/src/build) reported failures") \
    && (ASAN_OPTIONS=detect_leaks=0:abort_on_error=1 timeout 5m ctest --test-dir /src/build-asan --output-on-failure --timeout 180 \
       || echo "WARNING: ctest (/src/build-asan) reported failures") \
    && (timeout 2m sh test/espidf/host/run_xml_alloc.sh \
       || echo "WARNING: test/espidf/host/run_xml_alloc.sh reported failures")
