import os
import platform
import subprocess
from functools import lru_cache
from pathlib import Path

from pretop.shared.constants import COMPILER_C, COMPILER_CPP


def _find_visual_studio_installation() -> Path:
    configured_path = os.environ.get("VSINSTALLDIR")
    if configured_path:
        return Path(configured_path)

    program_files_x86 = os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")
    vswhere = Path(program_files_x86) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
    if not vswhere.is_file():
        raise RuntimeError("Could not find vswhere.exe to locate the Visual Studio C++ tools.")

    result = subprocess.run(
        [
            str(vswhere),
            "-latest",
            "-products",
            "*",
            "-requires",
            "Microsoft.VisualStudio.Component.VC.CMake.Project",
            "-property",
            "installationPath",
        ],
        check=True,
        capture_output=True,
        text=True,
    )
    installation_path = result.stdout.strip()
    if not installation_path:
        raise RuntimeError(
            "Could not find a Visual Studio installation with the CMake and C++ tools."
        )
    return Path(installation_path)


@lru_cache(maxsize=1)
def native_cmake_tools() -> tuple[str, str, list[str]]:
    """Return the CMake executable, Ninja executable, and compiler toolchain args."""
    if platform.system() != "Windows":
        return (
            "cmake",
            "ninja",
            [
                f"-DCMAKE_C_COMPILER={COMPILER_C}",
                f"-DCMAKE_CXX_COMPILER={COMPILER_CPP}",
            ],
        )

    visual_studio = _find_visual_studio_installation()
    cmake = visual_studio / "Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"
    ninja = visual_studio / "Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe"
    compiler_directories = list((visual_studio / "VC/Tools/MSVC").glob("*/bin/Hostx64/x64"))
    if not cmake.is_file() or not ninja.is_file() or not compiler_directories:
        raise RuntimeError(
            "Visual Studio's CMake, Ninja, and MSVC C++ tools are required for native setup."
        )

    def msvc_version(path: Path) -> tuple[int, ...]:
        return tuple(int(part) for part in path.parents[2].name.split("."))

    compiler_directory = max(compiler_directories, key=msvc_version)
    linker = compiler_directory / "link.exe"
    librarian = compiler_directory / "lib.exe"
    compiler = compiler_directory / "cl.exe"
    if not linker.is_file() or not librarian.is_file() or not compiler.is_file():
        raise RuntimeError("The selected Visual Studio MSVC toolchain is incomplete.")

    compiler_args = [
        f"-DCMAKE_C_COMPILER={compiler}",
        f"-DCMAKE_CXX_COMPILER={compiler}",
        f"-DCMAKE_LINKER={linker}",
        f"-DCMAKE_AR={librarian}",
    ]
    return str(cmake), str(ninja), compiler_args
