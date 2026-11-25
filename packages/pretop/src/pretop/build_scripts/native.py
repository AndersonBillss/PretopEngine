import sysconfig

from pretop.utils.cmd import cmd
from pretop.shared.constants import (
    NATIVE_TARGET,
    ENGINE_OUT,
    EXE_SUFFIX,
    PYTHON_LIBRARY_NAME,
    PYTHON_PACKAGE_TARGET,
    TEST_TARGET,
)
from pretop.codegen.generate_sources import gen_sources
from pretop.build_scripts.cmake_tools import native_cmake_tools


def configure_native_debug():
    cmake, ninja, compiler_args = native_cmake_tools()
    if (
        cmd(
            [
                cmake,
                "-S",
                ".",
                "-B",
                ENGINE_OUT,
                "-G",
                "Ninja",
                *compiler_args,
                f"-DCMAKE_MAKE_PROGRAM={ninja}",
                "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
                "-DCMAKE_BUILD_TYPE=Debug",
                "-DPRETOP_BUILD_ENGINE=ON",
            ]
        ).returncode
        != 0
    ):
        print("Native Build failed")
        exit(1)


def build_native_debug(app=None, target=NATIVE_TARGET):
    gen_sources()
    configure_native_debug()
    cmake, _, _ = native_cmake_tools()
    return cmd([cmake, "--build", ENGINE_OUT, "--target", target])


def run_native_debug(app=None):
    build_native_debug()
    return cmd([f"{ENGINE_OUT}/{NATIVE_TARGET}{EXE_SUFFIX}"])


def build_python(app=None):
    gen_sources()
    configure_native_debug()
    cmake, _, _ = native_cmake_tools()
    if (
        cmd(
            [
                cmake,
                "--build",
                ENGINE_OUT,
                "--target",
                PYTHON_PACKAGE_TARGET,
            ]
        )
    ).returncode != 0:
        print("Python Build failed")
        exit(1)

    return cmd(
        [
            cmake,
            "--install",
            ENGINE_OUT,
            "--prefix",
            sysconfig.get_paths()["platlib"],
            "--component",
            PYTHON_LIBRARY_NAME,
        ]
    ).returncode


def run_tests(args: list[str], app=None):
    build_native_debug(target=TEST_TARGET)
    return cmd([f"{ENGINE_OUT}/{TEST_TARGET}{EXE_SUFFIX}"] + args)
