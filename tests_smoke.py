"""Run local HTTP regression checks without a database or external scan."""
import http.client
from pathlib import Path
import socket
import subprocess
import time

port = 19090
process = subprocess.Popen([str(Path('build/Httpserver').resolve()), str(port)], stdout=subprocess.DEVNULL)
try:
    for _ in range(100):
        if process.poll() is not None:
            raise RuntimeError('Server exited before startup')
        try:
            with socket.create_connection(('127.0.0.1', port), timeout=0.2):
                break
        except OSError:
            time.sleep(0.1)
    else:
        raise RuntimeError('Server did not start')
    def request(path, body=None):
        connection = http.client.HTTPConnection('127.0.0.1', port, timeout=5)
        connection.request('POST' if body is not None else 'GET', path, body,
                           {'Content-Type': 'application/x-www-form-urlencoded'} if body is not None else {})
        response = connection.getresponse()
        payload = response.read()
        assert len(payload) == int(response.getheader('Content-Length'))
        connection.close()
        return response.status, payload
    assert request('/')[0] == 200
    assert request('/not-found')[0] == 404
    assert request('/../CMakeLists.txt')[0] == 400
    large = Path('build/wwwroot/smoke-large.html')
    large.write_bytes(b'large-response-' * 20000)
    try:
        assert request('/smoke-large.html')[1] == large.read_bytes()
    finally:
        large.unlink()
    # Invalid CGI input is rejected before invoking Nmap.
    assert request('/CgiTool', 'ip=invalid&port=80')[0] == 500
    with socket.create_connection(('127.0.0.1', port), timeout=5) as sock:
        sock.sendall(b'POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: -1\r\n\r\n')
        response = http.client.HTTPResponse(sock)
        response.begin()
        assert response.status == 400
        response.read()
    print('HTTP regression checks passed')
finally:
    process.terminate()
    try:
        process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        process.kill()
        process.wait()
