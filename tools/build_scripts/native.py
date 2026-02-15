from utils.cmd import cmd 
from shared.constants import BUILD_DIR
from codegen.gen_sources import gen_sources

ENGINE_OUT = f"{BUILD_DIR}/engine"

# Native engine commands
def build_native_debug():
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
                "-DCMAKE_C_COMPILER=cl",
                "-DCMAKE_CXX_COMPILER=cl",
                "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
            ]
        ).returncode
        != 0
    ):
        print("Native Build failed")
        exit(1)

    return cmd(["cmake", "--build", ENGINE_OUT])


def run_native_debug():
    build_native_debug()
    return cmd(["build/engine/ab_engine.exe"])
