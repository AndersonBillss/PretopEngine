import os
from utils.cmd import cmd 
from shared.constants import BUILD_DIR, COMPILER_C, COMPILER_CPP, EXE_SUFFIX
from codegen.gen_sources import gen_sources

ENGINE_OUT = f"{BUILD_DIR}/engine"

# Native engine commands
def build_native_debug(target="pretop_engine"):
    gen_sources()
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
                "-DCMAKE_BUILD_TYPE=Debug"
            ]
        ).returncode
        != 0
    ):
        print("Native Build failed")
        exit(1)

    return cmd(["cmake", "--build", ENGINE_OUT, "--target", target])


def run_native_debug():
    build_native_debug()
    return cmd([f"build/engine/pretop_engine{EXE_SUFFIX}"])

def run_tests(args: list[str]):
    build_native_debug("pretop_engine_tests")
    return cmd([f"build/engine/pretop_engine_tests{EXE_SUFFIX}"] + args)
