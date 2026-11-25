import os
import platform
import subprocess

from pretop.build_scripts.cmake_tools import native_cmake_tools
from pretop.utils.cmd import cmd


DAWN_REPOSITORY = "https://dawn.googlesource.com/dawn"
DAWN_COMMIT = "a8db5510cc0e32e289f4d977a3a32c2526708bc6"
DAWN_SOURCE_DIR = "third_party/dawn"
DAWN_BUILD_DIR = "third_party/dawn-build"
DAWN_INSTALL_DIR = "third_party/dawn-install"


def _current_dawn_commit():
    result = subprocess.run(
        ["git", "-C", DAWN_SOURCE_DIR, "rev-parse", "HEAD"],
        check=True,
        capture_output=True,
        text=True,
    )
    return result.stdout.strip()


def _ensure_dawn_checkout():
    os.makedirs("third_party", exist_ok=True)

    git_directory = os.path.join(DAWN_SOURCE_DIR, ".git")
    if not os.path.isdir(git_directory):
        if os.path.exists(DAWN_SOURCE_DIR):
            raise RuntimeError(
                f"{DAWN_SOURCE_DIR} exists but is not a Dawn git checkout; "
                "move it aside before running setup."
            )
        cmd(["git", "clone", DAWN_REPOSITORY, DAWN_SOURCE_DIR])

    if _current_dawn_commit() != DAWN_COMMIT:
        cmd(["git", "-C", DAWN_SOURCE_DIR, "fetch", "origin", DAWN_COMMIT])
        cmd(["git", "-C", DAWN_SOURCE_DIR, "checkout", "--detach", DAWN_COMMIT])


def setup_dawn():
    """Clone, build, and install the pinned Dawn revision for native builds."""
    _ensure_dawn_checkout()
    cmake, ninja, compiler_args = native_cmake_tools()

    configure_args = [
        cmake,
        "-S",
        DAWN_SOURCE_DIR,
        "-B",
        DAWN_BUILD_DIR,
        "-G",
        "Ninja",
        *compiler_args,
        f"-DCMAKE_MAKE_PROGRAM={ninja}",
        "-DCMAKE_BUILD_TYPE=Debug",
        f"-DCMAKE_INSTALL_PREFIX={os.path.abspath(DAWN_INSTALL_DIR)}",
        "-DCMAKE_INSTALL_LIBDIR=lib",
        "-DBUILD_SHARED_LIBS=OFF",
        "-DDAWN_BUILD_MONOLITHIC_LIBRARY=STATIC",
        "-DDAWN_ENABLE_INSTALL=ON",
        "-DDAWN_FETCH_DEPENDENCIES=ON",
        "-DDAWN_FORCE_SYSTEM_COMPONENT_LOAD=ON",
        "-DDAWN_BUILD_SAMPLES=OFF",
        "-DDAWN_BUILD_TESTS=OFF",
    ]

    if platform.system() == "Windows":
        # Avoid finding MSYS2's zlib when compiling Dawn with MSVC.
        configure_args.append("-Dprotobuf_WITH_ZLIB=OFF")
    elif platform.system() == "Linux":
        configure_args.append("-DDAWN_USE_WAYLAND=ON")

    cmd(configure_args)
    cmd([cmake, "--build", DAWN_BUILD_DIR])
    cmd([cmake, "--install", DAWN_BUILD_DIR, "--config", "Debug"])
