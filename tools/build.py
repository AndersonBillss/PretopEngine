from subprocess import run, SubprocessError
import sys
from gen_sources import gen_sources

BUILD_DIR = "build"
ENGINE_OUT = f"{BUILD_DIR}/engine"
ENGINE_WEB_OUT = f"{BUILD_DIR}/web-engine"

DAWN_SRC = "third_party/dawn"
DAWN_OUT_DEBUG = f"{BUILD_DIR}/dawn/debug"
DAWN_INSTALL_DEBUG = f"{BUILD_DIR}/_install/dawn/debug"

EMCMAKE = "emcmake"
if sys.platform == "win32":
    EMCMAKE = "emcmake.bat"


def cmd(args, *, working_directory=None):
    print(">>", " ".join(args))
    return run(args, check=True, cwd=working_directory)


# Native engine commands
def build_engine_debug():
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


def run_engine_debug():
    build_engine_debug()
    return cmd(["build/engine/ab_engine.exe"])


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
    try:
        result = cmd(
            [sys.executable, "-m", "http.server", "--directory", ENGINE_WEB_OUT, "8000"]
        )
        return result
    except KeyboardInterrupt:
        print("Closing down server")
        return None


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


COMMANDS = {
    "build-engine-debug": build_engine_debug,
    "run-engine-debug": run_engine_debug,
    "build-web-debug": build_web_debug,
    "run-web-debug": run_web_debug,
    "dawn-debug-setup": dawn_debug_setup,
    "gen-sources": gen_sources,
}


def execute_build(build_command):
    if build_command not in COMMANDS:
        print(f"Unknown command: '{build_command}'")
    else:
        build_fn = COMMANDS[sys.argv[1]]
        print(build_command)
        try:
            build_fn()
        except KeyboardInterrupt:
            pass
        except SubprocessError:
            pass


if len(sys.argv) == 1:
    print("Available build commands:")
    for key in COMMANDS:
        print(f"  {key}")
elif len(sys.argv) == 2:
    execute_build(sys.argv[1])
else:
    print("Must provide two arguments")
