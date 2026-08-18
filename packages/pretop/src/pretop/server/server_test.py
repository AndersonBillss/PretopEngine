import io
import unittest
from unittest import mock

from pretop.server.server import server as server_module


class InterruptingInput:
    def read(self, _size):
        raise KeyboardInterrupt


class ServerTest(unittest.TestCase):
    def test_http_server_allows_immediate_address_reuse(self):
        self.assertTrue(server_module.http.server.ThreadingHTTPServer.allow_reuse_address)

    def test_quit_closes_server_and_allows_immediate_rebind(self):
        server_class = server_module.http.server.ThreadingHTTPServer
        servers = []

        def create_server(address, handler):
            httpd = server_class(address, handler)
            servers.append(httpd)
            return httpd

        with mock.patch.object(
            server_module.http.server,
            "ThreadingHTTPServer",
            side_effect=create_server,
        ):
            server_module.run_server(port=0, input_stream=io.StringIO("q"))

        port = servers[0].server_address[1]
        self.assertEqual(servers[0].fileno(), -1)

        rebound_server = server_class(("", port), server_module.Handler)
        rebound_server.server_close()

    def test_keyboard_interrupt_still_closes_server(self):
        server_class = server_module.http.server.ThreadingHTTPServer
        servers = []

        def create_server(address, handler):
            httpd = server_class(address, handler)
            servers.append(httpd)
            return httpd

        with mock.patch.object(
            server_module.http.server,
            "ThreadingHTTPServer",
            side_effect=create_server,
        ):
            with self.assertRaises(KeyboardInterrupt):
                server_module.run_server(port=0, input_stream=InterruptingInput())

        self.assertEqual(servers[0].fileno(), -1)

    def test_end_of_input_stops_server(self):
        server_module.run_server(port=0, input_stream=io.StringIO(""))

    def test_open_uses_the_bound_port(self):
        with mock.patch.object(server_module.webbrowser, "open") as open_browser:
            server_module.run_server(port=0, input_stream=io.StringIO("oq"))

        opened_url = open_browser.call_args.args[0]
        self.assertRegex(opened_url, r"^http://localhost:\d+/.+\.html$")

    def test_live_port_conflict_exits_with_clear_error(self):
        active_server = server_module.http.server.ThreadingHTTPServer(
            ("", 0), server_module.Handler
        )
        port = active_server.server_address[1]
        self.addCleanup(active_server.server_close)

        with self.assertRaises(SystemExit) as raised:
            server_module.run_server(port=port, input_stream=io.StringIO("q"))

        self.assertIn(f"port {port} is already in use", str(raised.exception))


if __name__ == "__main__":
    unittest.main()
