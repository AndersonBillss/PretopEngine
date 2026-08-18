import http.server
import errno
from pathlib import Path
import threading
import webbrowser
import sys
import os

from packages.pretop.src.pretop.shared.constants import ENGINE_DIR, ASSET_DIR, ENGINE_NAME

PORT = 8000
BUILD_DIRECTORY = "build/web-engine"


class Handler(http.server.SimpleHTTPRequestHandler):
    def translate_path(self, path):
        if path.startswith("/assets/"):
            relative_path = path[len("/assets/"):]
            return os.path.join(os.getcwd(), Path(ENGINE_DIR) / Path(ASSET_DIR), relative_path)

        return super().translate_path(path)

    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=BUILD_DIRECTORY, **kwargs)

    def end_headers(self):
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        super().end_headers()


def run_server(port=PORT, input_stream=None):
    if input_stream is None:
        input_stream = sys.stdin

    try:
        httpd = http.server.ThreadingHTTPServer(("", port), Handler)
    except OSError as error:
        if error.errno == errno.EADDRINUSE:
            raise SystemExit(
                f"Cannot start web server: port {port} is already in use. "
                "Stop the process using that port and try again."
            ) from None
        raise

    print("Enter 'o' to open engine in browser, enter 'q' to quit")
    server_thread = threading.Thread(target=httpd.serve_forever)
    server_thread.start()

    try:
        key_listener(httpd.server_address[1], input_stream)
    finally:
        httpd.shutdown()
        server_thread.join()
        httpd.server_close()


def key_listener(port=PORT, input_stream=None):
    if input_stream is None:
        input_stream = sys.stdin

    while True:
        key = input_stream.read(1)

        if key == "":
            print("Exiting.")
            return

        if key.lower() == "o":
            webbrowser.open(f"http://localhost:{port}/{ENGINE_NAME}.html")

        elif key.lower() == "q":
            print("Exiting.")
            return
