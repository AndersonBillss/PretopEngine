from pretop.utils.cmd import cmd
from pretop.shared.constants import ENGINE_WEB_OUT, NATIVE_TARGET
from pretop.codegen.gen_sources import gen_sources
from pretop.server.server import run_server
import sys

EMCMAKE = "emcmake"
if sys.platform == "win32":
    EMCMAKE = "emcmake.bat"


# Web engine commands
def build_web_debug():
    gen_sources()
    if (
        cmd(
            [
                EMCMAKE,
                "cmake",
                "-S",
                ".",
                "-B",
                ENGINE_WEB_OUT,
                "-G",
                "Ninja",
                "-DCMAKE_BUILD_TYPE=Debug",
            ]
        ).returncode
        != 0
    ):
        print("Web Build failed")
        exit(1)
    return cmd(["cmake", "--build", ENGINE_WEB_OUT])


def run_web_debug():
    build_web_debug()
    print(f"Running engine at http://localhost:8000/{NATIVE_TARGET}.html")
    run_server()
