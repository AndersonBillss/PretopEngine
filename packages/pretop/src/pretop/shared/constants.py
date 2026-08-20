from pathlib import Path
import platform

ENGINE_NAME = "pretop_engine"

BUILD_DIR = "build"
ENGINE_DIR = ".pretop"

ASSET_INPUT_DIR = "assets"
ASSET_OUTPUT_DIR = Path(ENGINE_DIR) / Path("assets")
WEB_ASSET_PATH_PREFIX = "assets"

SOURCE_DIR = "source"
GEN_DIR = Path(SOURCE_DIR) / "Gen"

COMPILER_C = "cl"
COMPILER_CPP = "cl"

if platform.system() == "Linux":
    COMPILER_C = "gcc"
    COMPILER_CPP = "g++"

EXE_SUFFIX = ".exe"
if platform.system() == "Linux":
    EXE_SUFFIX = ""
