#!/usr/bin/env python3
"""Loopback only: no physical interface, NAS or media URL fetch."""
import http.client
import http.server
import socket
import signal
import subprocess
import sys
import threading
import xml.etree.ElementTree as ET

helper, cli = sys.argv[1:]
subprocess.run([helper], check=True, timeout=5)
for args, expected in [(['--help'], 0), (['--with-renderer'], 2), (['--unknown'], 2),
                       (['--target', '127.0.0.1', '--interface', '0.0.0.0'], 2)]:
    assert subprocess.run([cli] + args, capture_output=True, timeout=5).returncode == expected

process = subprocess.Popen([helper, '--renderer-fixture'], stdin=subprocess.PIPE,
                           stdout=subprocess.PIPE, text=True)
port = int(process.stdout.readline())
probe = socket.socket()
probe.bind(('127.0.0.1', 0))
probe.listen(1)
probe.settimeout(0.1)
probe_url = f'http://127.0.0.1:{probe.getsockname()[1]}/must-not-fetch'
def request(method, path, body=None, headers=None):
    connection = http.client.HTTPConnection('127.0.0.1', port, timeout=3)
    connection.request(method, path, body, headers or {})
    response = connection.getresponse()
    result = response.status, response.read()
    connection.close()
    return result

def soap(service, name, args=''):
    body = (f'<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/">'
            f'<s:Body><u:{name} xmlns:u="urn:schemas-upnp-org:service:{service}:1">'
            f'{args}</u:{name}></s:Body></s:Envelope>')
    return request('POST', f'/{service}/control', body,
                   {'Content-Type': 'text/xml', 'SOAPAction': f'"urn:schemas-upnp-org:service:{service}:1#{name}"'})

try:
    status, description = request('GET', '/description.xml')
    assert status == 200 and b'no playback' in description and b'DLNADOC' not in description
    root = ET.fromstring(description)
    assert len(root.findall('.//{*}service')) == 3
    assert b'00000000-0000-0000-0000-000000000064' not in description
    for service in ['ConnectionManager', 'AVTransport', 'RenderingControl']:
        status, data = request('GET', f'/{service}.xml')
        assert status == 200 and ET.fromstring(data) is not None
        assert all(item.attrib.get('sendEvents') == 'no' for item in ET.fromstring(data).findall('.//{*}stateVariable'))
    status, data = soap('ConnectionManager', 'GetProtocolInfo')
    assert status == 200
    parsed = ET.fromstring(data)
    assert parsed.find('.//Source') is not None and not parsed.find('.//Source').text
    assert parsed.find('.//Sink') is not None and not parsed.find('.//Sink').text
    status, data = soap('AVTransport', 'GetTransportInfo', '<InstanceID>0</InstanceID>')
    assert status == 200 and b'NO_MEDIA_PRESENT' in data
    status, data = soap('AVTransport', 'GetTransportInfo')
    assert status == 500 and b'718' in data, 'missing input reused a previous instance'
    for name in ['SetVolume', 'SetMute']:
        status, data = soap('RenderingControl', name, '<InstanceID>0</InstanceID>')
        assert status == 500 and b'401' in data
    for command, args in [('Play', '<Speed>1</Speed>'), ('Stop', ''),
                           ('SetAVTransportURI', f'<CurrentURI>{probe_url}</CurrentURI><CurrentURIMetaData/>')]:
        status, data = soap('AVTransport', command, '<InstanceID>0</InstanceID>' + args)
        assert status == 500 and b'701' in data, (status, data)
    status, data = request('SUBSCRIBE', '/AVTransport/event', headers={
        'CALLBACK': f'<{probe_url}>', 'NT': 'upnp:event', 'TIMEOUT': 'Second-10'})
    assert status == 405
    assert request('GET', '/not-a-file')[0] == 404
    try:
        attempted, _ = probe.accept()
    except socket.timeout:
        pass
    else:
        attempted.close()
        raise AssertionError('diagnostic attempted a media/CALLBACK URL fetch')
finally:
    probe.close()
    process.stdin.close()
    process.wait(timeout=5)
assert process.returncode == 0
with socket.socket() as sock:
    sock.settimeout(1)
    assert sock.connect_ex(('127.0.0.1', port)) != 0, 'listener survived teardown'

class Fixture(http.server.BaseHTTPRequestHandler):
    mode = 'success'
    gets = 0
    posts = 0
    def log_message(self, *args):
        pass
    def do_GET(self):
        Fixture.gets += 1
        assert self.path == '/description.xml'
        if self.mode == 'denied':
            self.send_response(403)
            data = b''
        elif self.mode == 'redirect':
            self.send_response(302)
            self.send_header('Location', 'http://127.0.0.2/never-fetch')
            data = b''
        else:
            self.send_response(200)
            control = 'http://127.0.0.2/never-fetch' if self.mode == 'foreign-control' else '/control'
            data = (f'<root xmlns="urn:schemas-upnp-org:device-1-0"><specVersion><major>1</major><minor>0</minor></specVersion>'
                    f'<device><deviceType>urn:schemas-upnp-org:device:MediaServer:1</deviceType><UDN>uuid:loopback-fixture</UDN><serviceList>'
                    f'<service><serviceType>urn:schemas-upnp-org:service:ContentDirectory:1</serviceType>'
                    f'<serviceId>urn:upnp-org:serviceId:ContentDirectory</serviceId><SCPDURL>/never-fetch-scpd</SCPDURL>'
                    f'<controlURL>{control}</controlURL><eventSubURL>/never-subscribe</eventSubURL></service>'
                    f'</serviceList></device></root>').encode()
        self.send_header('Content-Length', str(len(data)))
        self.end_headers()
        self.wfile.write(data)
    def do_POST(self):
        Fixture.posts += 1
        assert self.path == '/control'
        data = self.rfile.read(int(self.headers['Content-Length']))
        assert b'BrowseMetadata' in data and b'<RequestedCount>1</RequestedCount>' in data
        body = (b'<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body>'
                b'<u:BrowseResponse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">'
                b'<Result>&lt;DIDL-Lite/&gt;</Result><NumberReturned>1</NumberReturned>'
                b'<TotalMatches>1</TotalMatches><UpdateID>1</UpdateID></u:BrowseResponse></s:Body></s:Envelope>')
        self.send_response(200)
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        self.wfile.write(body)

server = http.server.HTTPServer(('127.0.0.1', 0), Fixture)
thread = threading.Thread(target=server.serve_forever, daemon=True)
thread.start()
try:
    location = f'http://127.0.0.1:{server.server_port}/description.xml'
    for mode in ['success', 'denied', 'redirect', 'foreign-control']:
        Fixture.mode = mode
        before = Fixture.posts
        run = subprocess.run([helper, '--inspect-fixture', location], capture_output=True, text=True, timeout=8)
        assert run.returncode == (0 if mode == 'success' else 1), (mode, run.stdout, run.stderr)
        assert Fixture.posts - before == (1 if mode == 'success' else 0), mode
    assert Fixture.gets == 4
finally:
    server.shutdown()
    server.server_close()
# Exercise the actual CLI signal cleanup. Multicast is explicitly bound to
# loopback for this case; no physical interface or NAS is involved.
started = threading.Event()
lines = []
process = subprocess.Popen([cli, '--target', '127.0.0.1', '--interface', '127.0.0.1',
                            '--with-renderer', '--seconds', '10'], stdout=subprocess.PIPE,
                           stderr=subprocess.PIPE, text=True)
def collect():
    for line in process.stdout:
        lines.append(line)
        if line.startswith('renderer_description_http='):
            started.set()
reader = threading.Thread(target=collect, daemon=True)
reader.start()
try:
    assert started.wait(5), 'loopback diagnostic failed to start'
    process.send_signal(signal.SIGINT)
    assert process.wait(timeout=5) == 130
    reader.join(timeout=1)
    assert 'renderer_description_http=200\n' in lines, lines
    assert 'renderer=stopped-byebye-sent\n' in lines, lines
finally:
    if process.poll() is None:
        process.terminate()
        process.wait(timeout=5)
print('loopback HTTP, SOAP rejection, exact-target reads and interrupt teardown passed')
