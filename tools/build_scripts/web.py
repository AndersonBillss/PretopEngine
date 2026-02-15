from utils.cmd import cmd
from shared.constants import BUILD_DIR
from codegen.gen_sources import gen_sources
from server.server import run_server
import sys

ENGINE_WEB_OUT = f"{BUILD_DIR}/web-engine"

EMCMAKE = "emcmake"
if sys.platform == "win32":
    EMCMAKE = "emcmake.bat"


# Web engine commands
def build_web_debug():
    gen_sources()
    if (
        cmd(
            [EMCMAKE, "cmake", "-S", ".", "-B", ENGINE_WEB_OUT, "-G", "Ninja"]
        ).returncode
        != 0
    ):
        print("Web Build failed")
        exit(1)
    return cmd(["cmake", "--build", ENGINE_WEB_OUT])


def run_web_debug():
    build_web_debug()
    print("Running engine at http://localhost:8000/ab_engine.html")
    run_server()
