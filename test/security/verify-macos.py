#!/usr/bin/env python3
"""Run security fixtures with ASan/UBSan on macOS.
Requires clang, an existing config.h, and Homebrew boost and openssl@3.
Run outside a network sandbox; fixtures use local sockets only.
Usage: python3 test/security/verify-macos.py /tmp/mupnp-security-build
"""
from pathlib import Path
import concurrent.futures
import subprocess
import sys

root = Path(__file__).resolve().parents[2]
output = Path(sys.argv[1] if len(sys.argv) > 1 else '/tmp/mupnp-security-build').resolve()
output.mkdir(parents=True, exist_ok=True)
sanitizers = ['-fsanitize=address,undefined', '-g']
openssl = Path('/opt/homebrew/opt/openssl@3')
boost = Path('/opt/homebrew/opt/boost')
link = ['-L' + str(openssl / 'lib'), '-lssl', '-lcrypto', '-lexpat', '-lpthread']
# Use Xcode's compilers for every step: mixing a PATH clang (e.g. Homebrew LLVM)
# with Apple clang objects fails to link the ASan runtime.
cc = ['xcrun', 'clang']
cxx = ['xcrun', 'clang++']


def run(command, **kwargs):
    return subprocess.run(command, cwd=root, check=True, **kwargs)


def compile_source(source):
    target = output / (source.replace('/', '_') + '.o')
    result = subprocess.run([*cc, '-DHAVE_CONFIG_H', '-DMUPNP_USE_OPENSSL', '-I.', '-Iinclude',
                             '-I' + str(openssl / 'include'), *sanitizers, '-O1', '-c', source,
                             '-o', str(target)], cwd=root, capture_output=True, text=True)
    if result.returncode:
        raise RuntimeError(source + '\n' + result.stderr)
    return str(target)


sources = subprocess.check_output(['rg', '--files', 'src/mupnp', '-g', '*.c'], cwd=root, text=True).splitlines()
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
    objects = list(pool.map(compile_source, sorted(sources)))
archive = output / 'libmupnp.a'
archive.unlink(missing_ok=True)
run(['ar', 'rcs', str(archive), *objects])
run([*cc, '-DHAVE_CONFIG_H', '-I.', '-Iinclude', *sanitizers, '-c', 'test/TestDevice.c', '-o', str(output / 'TestDevice.o')])
run([*cc, '-Iinclude', '-Iwrapper/objc/mUPnP', *sanitizers, '-framework', 'Foundation',
     *map(str, sorted((root / 'wrapper/objc/mUPnP').glob('*.m'))), 'test/security/objc-lifetime.m',
     str(output / 'TestDevice.o'), str(archive), *link, '-o', str(output / 'objc-lifetime')])
run([str(output / 'objc-lifetime')], timeout=30)
# Wrapper API regressions; MUPNP_OBJC_TEST_HOOKS exposes native alloc/delete hooks.
run([*cc, '-DMUPNP_OBJC_TEST_HOOKS', '-Iinclude', '-Iwrapper/objc/mUPnP', *sanitizers, '-framework', 'Foundation',
     *map(str, sorted((root / 'wrapper/objc/mUPnP').glob('*.m'))), 'test/security/objc-api-regression.m',
     str(output / 'TestDevice.o'), str(archive), *link, '-o', str(output / 'objc-api-regression')])
run([str(output / 'objc-api-regression')], timeout=60)
run([*cc, '-x', 'c', '-Iinclude', *sanitizers, '-c', 'test/security/media-cycles.c.fixture', '-o', str(output / 'media-cycles.o')])
run([*cc, *sanitizers, str(output / 'media-cycles.o'), str(archive), *link, '-o', str(output / 'media-cycles')])
run([str(output / 'media-cycles')], timeout=10)
run([sys.executable, 'test/security/verify-tls.py', str(archive)], timeout=45)

cpp = [*cxx, '-std=c++17', '-Iinclude', '-Itest', '-I' + str(boost / 'include'), *sanitizers]
test_link = ['-L' + str(boost / 'lib'), '-lboost_unit_test_framework', *link]
run([*cpp, *map(str, sorted((root / 'test').glob('*.cpp'))), str(output / 'TestDevice.o'),
     str(archive), *test_link, '-o', str(output / 'mupnptest')])
run([str(output / 'mupnptest'), '--color_output=no'], timeout=120)

# Exercise the optional XML backend and the debug receive variant explicitly.
run([*cc, '-DHAVE_CONFIG_H', '-DMUPNP_XMLPARSER_LIBXML2', '-I.', '-Iinclude',
     '-I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/libxml2', *sanitizers,
     '-c', 'src/mupnp/xml/xml_parser_libxml2.c', '-o', str(output / 'libxml.o')])
run([*cpp, 'test/TestMain.cpp', 'test/XMLTest.cpp', str(output / 'libxml.o'), str(archive),
     *test_link, '-lxml2', '-o', str(output / 'libxml-tests')])
run([str(output / 'libxml-tests'), '--color_output=no'], timeout=10)
run([*cc, '-DHAVE_CONFIG_H', '-DMUPNP_USE_OPENSSL', '-DSOCKET_DEBUG', '-I.', '-Iinclude',
     '-I' + str(openssl / 'include'), *sanitizers, '-c', 'src/mupnp/net/socket.c', '-o', str(output / 'debug-socket.o')])
run([*cpp, 'test/TestMain.cpp', 'test/HttpTest.cpp', str(output / 'TestDevice.o'),
     str(output / 'debug-socket.o'), str(archive), *test_link, '-o', str(output / 'debug-http-tests')])
run([str(output / 'debug-http-tests'), '--run_test=HttpTruncatedBodies,HttpCompleteBodies', '--color_output=no'], timeout=10)
print('PASS: Objective-C lifetime and API regressions, media cycles, TLS trust/identity, full C suite, libxml2, and SOCKET_DEBUG (ASan/UBSan)')
