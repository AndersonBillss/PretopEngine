from utils.cmd import cmd 
from constants.build import BUILD_DIR
import sys

DAWN_SRC = "third_party/dawn"
DAWN_OUT_DEBUG = f"{BUILD_DIR}/dawn/debug"
DAWN_INSTALL_DEBUG = f"{BUILD_DIR}/_install/dawn/debug"


# Dawn build commands
def dawn_deps():
    if (
        cmd(
            [sys.executable, "tools/fetch_dawn_dependencies.py"],
            working_directory=DAWN_SRC,
        ).returncode
        != 0
    ):
        print("Failed building Dawn dependencies")
        exit(0)


def dawn_debug_configure():
    if (
        cmd(
            [
                "cmake",
                "-S",
                DAWN_SRC,
                "-B",
                DAWN_OUT_DEBUG,
                "-G",
                "Ninja",
                "-DCMAKE_C_COMPILER=cl",
                "-DCMAKE_CXX_COMPILER=cl",
                "-DCMAKE_BUILD_TYPE=Debug",
                "-DDAWN_ENABLE_INSTALL=ON",
                "-DDAWN_FORCE_SYSTEM_COMPONENT_LOAD=ON",
            ]
        ).returncode
        != 0
    ):
        print("Failed to configure Dawn!")
        exit(0)


def dawn_debug_build():
    dawn_debug_configure()
    if cmd(["cmake", "--build", DAWN_OUT_DEBUG]).returncode != 0:
        print("Dawn debug setup failed!")
        exit(1)
    if (
        cmd(
            ["cmake", "--install", DAWN_OUT_DEBUG, "--prefix", DAWN_INSTALL_DEBUG]
        ).returncode
        != 0
    ):
        print("Dawn debug setup failed!")
        exit(1)


def dawn_debug_setup():
    dawn_deps()
    dawn_debug_build()

