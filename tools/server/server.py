import http.server
import socketserver
import threading
import webbrowser
import sys
import os

PORT = 8000
BUILD_DIRECTORY = "build/web-engine"
ASSET_DIRECTORY = "assets"


class Handler(http.server.SimpleHTTPRequestHandler):
    def translate_path(self, path):
        if path.startswith("/assets/"):
            relative_path = path[len("/assets/"):]
            return os.path.join(os.getcwd(), ASSET_DIRECTORY, relative_path)

        return super().translate_path(path)

    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=BUILD_DIRECTORY, **kwargs)

    def end_headers(self):
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        super().end_headers()


def run_server():
    print("Enter 'o' to open engine in browser, enter 'q' to quit")
    threading.Thread(target=server, daemon=True).start()
    key_listener()


def server():
    with socketserver.TCPServer(("", PORT), Handler) as httpd:
        httpd.serve_forever()


def key_listener():
    while True:
        key = sys.stdin.read(1)

        if key.lower() == "o":
            webbrowser.open(f"http://localhost:{PORT}/ab_engine.html")

        elif key.lower() == "q":
            print("Exiting.")
            sys.exit(0)
