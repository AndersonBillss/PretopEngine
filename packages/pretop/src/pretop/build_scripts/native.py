from pretop.utils.cmd import cmd
from pretop.shared.constants import (
    COMPILER_C,
    COMPILER_CPP,
    NATIVE_TARGET,
    ENGINE_OUT,
    EXE_SUFFIX,
    PYTHON_LIBRARY_NAME,
    PYTHON_PACKAGE_TARGET,
    TEST_TARGET,
)
from pretop.codegen.gen_sources import gen_sources


def configure_native_debug(app=None):
    if (
        cmd(
            [
                "cmake",
                "-S",
                ".",
                "-B",
                ENGINE_OUT,
                "-G",
                "Ninja",
                f"-DCMAKE_C_COMPILER={COMPILER_C}",
                f"-DCMAKE_CXX_COMPILER={COMPILER_CPP}",
                "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
                "-DCMAKE_BUILD_TYPE=Debug",
            ]
        ).returncode
        != 0
    ):
        print("Native Build failed")
        exit(1)


def build_native_debug(app=None, target=NATIVE_TARGET):
    gen_sources()
    return cmd(["cmake", "--build", ENGINE_OUT, "--target", target])


def run_native_debug(app=None):
    build_native_debug()
    return cmd([f"{ENGINE_OUT}/{NATIVE_TARGET}{EXE_SUFFIX}"])


def build_python(app=None):
    if (
        cmd(
            [
                "cmake",
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
            "cmake",
            "--install",
            ENGINE_OUT,
            "--prefix",
            ".venv/lib/python3.13/site-packages",
            "--component",
            PYTHON_LIBRARY_NAME,
        ]
    ).returncode


def run_tests(args: list[str], app=None):
    build_native_debug(target=TEST_TARGET)
    return cmd([f"{ENGINE_OUT}/{TEST_TARGET}{EXE_SUFFIX}"] + args)
