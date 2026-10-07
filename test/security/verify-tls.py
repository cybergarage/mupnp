#!/usr/bin/env python3
"""Local-only TLS trust and identity controls; pass a TLS-enabled ASan archive.
Usage: python3 test/security/verify-tls.py /tmp/mupnp-security-build/libmupnp.a
Requires macOS clang and Homebrew openssl@3; no external service is contacted.
"""
from pathlib import Path
import os
import socket
import ssl
import subprocess
import sys
import tempfile
import threading

root = Path(__file__).resolve().parents[2]
openssl_dir = Path('/opt/homebrew/opt/openssl@3')
openssl = str(openssl_dir / 'bin/openssl')
with tempfile.TemporaryDirectory(prefix='mupnp-tls-') as work:
    directory = Path(work)
    key, cert = directory / 'key.pem', directory / 'cert.pem'
    subprocess.run([openssl, 'req', '-x509', '-newkey', 'rsa:2048', '-nodes', '-days', '1',
                    '-keyout', str(key), '-out', str(cert), '-subj', '/CN=localhost',
                    '-addext', 'subjectAltName=DNS:localhost'], check=True, capture_output=True)
    source = directory / 'client.c'
    source.write_text('''#include <mupnp/net/socket.h>
#include <stdlib.h>
int main(int argc, char** argv) {
  if (argc != 3) return 2;
  mUpnpSocket* sock = mupnp_socket_new(MUPNP_NET_SOCKET_STREAM | MUPNP_NET_SOCKET_SSL);
  bool connected = mupnp_socket_connect(sock, argv[1], atoi(argv[2]));
  mupnp_socket_delete(sock);
  return connected ? 0 : 1;
}
''')
    client = directory / 'client'
    subprocess.run(['xcrun', 'clang', '-DMUPNP_USE_OPENSSL', '-fsanitize=address,undefined', '-g',
                    '-I' + str(root / 'include'), '-I' + str(openssl_dir / 'include'),
                    str(source), sys.argv[1], '-L' + str(openssl_dir / 'lib'),
                    '-lssl', '-lcrypto', '-lexpat', '-lpthread', '-o', str(client)], check=True)
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain(cert, key)
    for host, trust, expected in [('localhost', False, 1), ('localhost', True, 0), ('::1', True, 1)]:
        listener = socket.socket(socket.AF_INET6)
        listener.bind(('::1', 0))
        listener.listen(1)
        listener.settimeout(10)
        def serve():
            try:
                connection, _ = listener.accept()
                with context.wrap_socket(connection, server_side=True) as stream:
                    stream.recv(1)
            except (ssl.SSLError, OSError):
                pass
            finally:
                listener.close()
        worker = threading.Thread(target=serve)
        worker.start()
        environment = dict(os.environ)
        if trust:
            environment['SSL_CERT_FILE'] = str(cert)
        else:
            environment['SSL_CERT_FILE'] = str(directory / 'missing.pem')
        environment['SSL_CERT_DIR'] = str(directory / 'missing-directory')
        result = subprocess.run([str(client), host, str(listener.getsockname()[1])], env=environment, timeout=10)
        worker.join(11)
        assert result.returncode == expected, (host, trust, result.returncode, expected)
        assert not worker.is_alive()
        print('PASS:', host, 'trusted' if trust else 'untrusted', 'accepted' if expected == 0 else 'rejected')
