#!/usr/bin/env python3
"""
BeatCrate debug log server — run on your PC, same WiFi as Quest.
The mod POSTs log lines to http://<your-pc-ip>:8080/log

Usage:
    python3 debug_server.py

Then in BeatCrate Mod Settings, enter your PC's IP in the Debug Host field.
Find your PC IP:  macOS → System Settings → Wi-Fi → Details
                  Windows → ipconfig (look for IPv4 Address)
"""

from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs, unquote_plus
import datetime

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        parsed = urlparse(self.path)
        params = parse_qs(parsed.query)
        msg = unquote_plus(params.get('msg', [''])[0])
        ts  = datetime.datetime.now().strftime('%H:%M:%S.%f')[:-3]
        print(f"[{ts}] {msg}")
        self.send_response(200)
        self.end_headers()
        self.wfile.write(b'ok')

    def do_POST(self):
        length = int(self.headers.get('Content-Length', 0))
        body   = self.rfile.read(length).decode('utf-8', errors='replace')
        ts     = datetime.datetime.now().strftime('%H:%M:%S.%f')[:-3]
        print(f"[{ts}] {body}")
        self.send_response(200)
        self.end_headers()
        self.wfile.write(b'ok')

    def log_message(self, *args):
        pass  # silence default access log noise

if __name__ == '__main__':
    host = '0.0.0.0'
    port = 8080
    print(f"BeatCrate debug server listening on :{port}")
    print(f"Enter your PC's LAN IP in BeatCrate Mod Settings → Debug Host\n")
    HTTPServer((host, port), Handler).serve_forever()
