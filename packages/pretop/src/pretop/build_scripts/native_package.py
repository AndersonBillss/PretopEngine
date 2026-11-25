import sysconfig

from pretop.build_scripts.cmake_tools import native_cmake_tools
from pretop.utils.cmd import cmd


PYTHON_PACKAGE_OUT = "build/python-cli"
PYTHON_PACKAGE_TARGET = "pretop_python"


def build_shared_python_package():
    """Build the Python bridge and shared C++ utilities without the engine."""
    cmake, ninja, compiler_args = native_cmake_tools()
    cmd(
        [
            cmake,
            "-S",
            ".",
            "-B",
            PYTHON_PACKAGE_OUT,
            "-G",
            "Ninja",
            *compiler_args,
            f"-DCMAKE_MAKE_PROGRAM={ninja}",
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DBUILD_TESTING=OFF",
            "-DPRETOP_BUILD_ENGINE=OFF",
        ]
    )
    cmd([cmake, "--build", PYTHON_PACKAGE_OUT, "--target", PYTHON_PACKAGE_TARGET])
    cmd(
        [
            cmake,
            "--install",
            PYTHON_PACKAGE_OUT,
            "--prefix",
            sysconfig.get_paths()["platlib"],
            "--component",
            "pretop",
            "--config",
            "Debug",
        ]
    )
