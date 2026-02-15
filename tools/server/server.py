import http.server
import socketserver
import threading
import webbrowser
import sys

PORT = 8000
DIRECTORY = "build/web-engine"


class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=DIRECTORY, **kwargs)


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
