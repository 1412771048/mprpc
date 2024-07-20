#!/usr/bin/env python3
from http.server import BaseHTTPRequestHandler, HTTPServer

class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length).decode('utf-8')
        print(f"Received POST data: {post_data}")
        self.send_response(200)
        self.end_headers()
        self.wfile.write(b'POST request received')

if __name__ == '__main__':
    server_address = ('', 8080)
    httpd = HTTPServer(server_address, SimpleHTTPRequestHandler)
    print("Server running on port 8080...")
    httpd.serve_forever()
