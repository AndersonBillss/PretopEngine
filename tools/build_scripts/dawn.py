from utils.cmd import cmd 
from shared.constants import BUILD_DIR, COMPILER_C, COMPILER_CPP
import sys
import platform

DAWN_SRC = "third_party/dawn"
DAWN_OUT_DEBUG = f"{BUILD_DIR}/dawn/debug"
DAWN_INSTALL_DEBUG = f"{BUILD_DIR}/_install/dawn/debug"

PLATFORM_SPECIFIC_FLAGS = []
if platform.system() == "Linux":
    PLATFORM_SPECIFIC_FLAGS = ["-DDAWN_USE_WAYLAND=ON"]


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
                f"-DCMAKE_C_COMPILER={COMPILER_C}",
                f"-DCMAKE_CXX_COMPILER={COMPILER_CPP}",
                "-DCMAKE_BUILD_TYPE=Debug",
                "-DDAWN_ENABLE_INSTALL=ON",
                "-DDAWN_FORCE_SYSTEM_COMPONENT_LOAD=ON",
            ] + PLATFORM_SPECIFIC_FLAGS
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

