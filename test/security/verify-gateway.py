#!/usr/bin/env python3
"""macOS C/ObjC regressions using XML and 127.0.0.1 fixtures only.
Requires CMake, Boost, Apple clang; no discovery or physical-device requests.
Usage: python3 test/security/verify-gateway.py /tmp/mupnp-gateway-asan [--curl] [--libxml2]
"""
from pathlib import Path
import subprocess
import sys

root = Path(__file__).resolve().parents[2]
output = Path(sys.argv[1]).resolve()
flags = '-fsanitize=address,undefined -g -O1'

def run(command):
    subprocess.run(command, cwd=root, check=True, timeout=180)

run(['cmake', '-S', str(root), '-B', str(output),
     '-DCMAKE_C_COMPILER=/usr/bin/clang', '-DCMAKE_CXX_COMPILER=/usr/bin/clang++',
     '-DCMAKE_C_FLAGS=' + flags, '-DCMAKE_CXX_FLAGS=' + flags,
     '-DMUPNP_ENABLE_EXAMPLES=OFF', '-DMUPNP_ENABLE_TEST=ON',
     '-DMUPNP_HTTP_CURL=' + ('ON' if '--curl' in sys.argv else 'OFF'),
     '-DMUPNP_XMLPARSER_LIBXML2=' + ('ON' if '--libxml2' in sys.argv else 'OFF')])
run(['cmake', '--build', str(output), '-j4'])
# Avoid the existing multicast discovery/device tests entirely.
selection = 'Action*,Gateway*,XML*,URI*,String*,Dictionary*,ServiceAbsoluteURL,HttpFixed*,HttpChunked*,HttpRequest*,HttpTruncatedBodies,HttpCompleteBodies,HttpHuge*,HttpLarge*,HttpListener*,HttpServer*'
# This older regression asserts native-backend synthetic status/body behavior;
# curl's legacy partial-response behavior is unchanged by the additive API.
if '--curl' not in sys.argv:
    selection += ',HttpPostRejectsIncompleteChunkedResponse'
run([str(output / 'test/mupnptest'), '--color_output=no', '--run_test=' + selection])
link = ['-lexpat', '-lpthread']
if '--curl' in sys.argv:
    link += ['-lcurl']
if '--libxml2' in sys.argv:
    link += ['-lxml2']
run(['xcrun', 'clang', '-Wno-nullability-completeness', '-DMUPNP_OBJC_TEST_HOOKS',
     '-Iinclude', '-Iwrapper/objc/mUPnP', *flags.split(), '-framework', 'Foundation',
     *map(str, sorted((root / 'wrapper/objc/mUPnP').glob('*.m'))),
     'test/security/objc-api-regression.m', 'test/TestDevice.c',
     str(output / 'libMUPNP.a'), *link, '-o', str(output / 'objc-api-regression')])
run([str(output / 'objc-api-regression'), 'GatewayLifetime', 'StrictActionResults',
     'StateVariableWithoutNativeObject', 'StateVariableKeepsNativeUserData',
     'StateVariableWithoutValue', 'DeviceInitializationFailures', 'DeviceDescriptionUTF8',
     'DeviceDescriptionRejectsInvalidInput', 'ServiceDescriptionUTF8',
     'ActionRejectsInvalidArguments', 'ActionPostsValidArguments',
     'ActionReportsFaultAndConnectionFailure'])
print('PASS: C/ObjC local gateway/action fixtures and selected regressions (ASan/UBSan)')
