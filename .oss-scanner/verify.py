#!/usr/bin/env python3
"""Validate the Scanner image with fake secrets and offline, non-root fixtures."""
import json
from pathlib import Path
import subprocess
import tempfile
import uuid

ROOT = Path(__file__).resolve().parents[1]


def run(*args, **kwargs):
    return subprocess.run(args, check=True, **kwargs)


def verify_context(work):
    context = work / 'context'
    exported = work / 'filtered'
    (context / '.oss-scanner').mkdir(parents=True)
    # Export the complete filtered context, rather than hiding leaks behind
    # the production Dockerfile's narrower COPY statements.
    (context / '.oss-scanner/Dockerfile').write_text('FROM scratch\nCOPY . /filtered/\n')
    admitted = ['CMakeLists.txt', 'CMakeLists.txt.build', 'COPYING', 'include/api.h', 'include/deep/api.h',
                'src/main.c', 'test/Test.cpp', 'test/platform-smoke.c.in',
                'test/espidf/host/run.sh', 'test/unix/Makefile.am.build', 'examples/CMakeLists.txt',
                'examples/common/example.h', '.oss-scanner/threat_model.md']
    excluded = ['.env', '.git/config', '.codex/local.json', 'unrelated.h',
                'include/.env', 'src/deep/credentials.json', 'test/private.key',
                'examples/deep/.env', 'build/old.h', 'wrapper/private.c',
                'test/.git/secret.c', 'include/.env.h']
    for name in admitted + excluded:
        path = context / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('SYNTHETIC_FIXTURE_ONLY\n')
    # Control: the former broad COPY boundary transfers synthetic secrets.
    control = work / 'control'
    run('docker', 'buildx', 'build', '--progress=plain', '-f',
        str(context / '.oss-scanner/Dockerfile'), '--output',
        'type=local,dest=' + str(control), str(context))
    assert (control / 'filtered/.env').read_text() == 'SYNTHETIC_FIXTURE_ONLY\n'
    assert (control / 'filtered/src/deep/credentials.json').is_file()
    (context / '.oss-scanner/Dockerfile.dockerignore').write_text(
        (ROOT / '.oss-scanner/Dockerfile.dockerignore').read_text())
    run('docker', 'buildx', 'build', '--progress=plain', '-f',
        str(context / '.oss-scanner/Dockerfile'), '--output',
        'type=local,dest=' + str(exported), str(context))
    actual = {str(p.relative_to(exported / 'filtered'))
              for p in (exported / 'filtered').rglob('*') if p.is_file()}
    assert actual == set(admitted), (actual - set(admitted), set(admitted) - actual)
    print('PASS: real Docker context matcher admits source and excludes root/nested fake secrets', flush=True)


def verify_image(image, work):
    metadata = json.loads(subprocess.check_output(['docker', 'image', 'inspect', image], text=True))[0]
    assert metadata['Config']['User'] == 'scanner:scanner'
    print('Image:', metadata['Id'], 'user:', metadata['Config']['User'], flush=True)
    run('docker', 'run', '--rm', '--network', 'none',
        '-e', 'ASAN_OPTIONS=detect_leaks=0:abort_on_error=1:symbolize=1', image,
        'bash', '-euc', '''
        test "$(id -u)" = 10001
        test "$(id -g)" = 10001
        for path in /src /src/build /src/build-asan /home/scanner /tmp; do
          test -w "$path"
        done
        test ! -e /src/.git
        test ! -e /src/.codex
        test -z "$(find /src -type f \\( -name .env -o -name '*.key' -o -name '*.json' \\) -print -quit)"
        for build in build build-asan; do
          test -f /src/$build/libMUPNP.a
          for sample in clock/upnpclock binarylight/upnpbinarylight upnpdump/upnpdump upnpavdump/upnpavdump upnpigddump/upnpigddump; do
            test -x /src/$build/examples/$sample
          done
          ctest --test-dir /src/$build --output-on-failure -R '^(platform_smoke|mupnp_alloc_failure_test)$'
          /src/$build/test/mupnptest '--run_test=Action*,Gateway*,XML*,URI*,String*,Dictionary*,ServiceAbsoluteURL,Http*' --color_output=no
        done
        sh /src/test/espidf/host/check_generators.sh
        sh /src/test/espidf/host/run.sh
        sh /src/test/espidf/host/run_socket.sh
        sh /src/test/espidf/host/run_xml_alloc.sh
        ''')
    source = work / 'mounted-source'
    source.mkdir()
    (source / 'CMakeLists.txt').write_text('cmake_minimum_required(VERSION 3.5)\nproject(MountFixture C)\nadd_executable(fixture main.c)\n')
    (source / 'main.c').write_text('int main(void) { return 0; }\n')
    run('docker', 'run', '--rm', '--network', 'none', '--mount',
        'type=bind,src=' + str(source) + ',dst=/src,readonly', image, 'bash', '-euc', '''
        test "$(id -u)" = 10001
        test ! -e /src/build
        if mkdir /src/build 2>/dev/null; then exit 1; fi
        cmake -S /src -B /tmp/mount-build
        cmake --build /tmp/mount-build
        /tmp/mount-build/fixture
        ''')
    print('PASS: non-root builds, offline fixtures, image contents and read-only source mount', flush=True)


def main():
    image = 'mupnp-scanner-check:' + uuid.uuid4().hex
    with tempfile.TemporaryDirectory(prefix='mupnp-scanner-fixtures-') as directory:
        work = Path(directory)
        verify_context(work)
        try:
            run('docker', 'build', '--progress=plain', '-f', str(ROOT / '.oss-scanner/Dockerfile'),
                '-t', image, str(ROOT))
            verify_image(image, work)
        finally:
            exists = subprocess.run(['docker', 'image', 'inspect', image],
                                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode == 0
            if exists:
                run('docker', 'image', 'rm', image)


if __name__ == '__main__':
    main()
