from pathlib import Path
import platform

PYTHON_LIBRARY_NAME = "pretop"
ENGINE_NAME = "pretop_engine"
NATIVE_TARGET = "pretop_engine"
TEST_TARGET = "pretop_engine_tests"
PYTHON_PACKAGE_TARGET = "pretop_python"

BUILD_DIR = "build"
ENGINE_OUT = f"{BUILD_DIR}/engine"
ENGINE_WEB_OUT = f"{BUILD_DIR}/web-engine"
WEB_SERVER_PORT = 8000

ENGINE_DIR = ".pretop"

ASSET_OUTPUT_DIR = f"{ENGINE_DIR}/assets"
WEB_ASSET_PATH_PREFIX = "assets"

SOURCE_DIR = "source"
GEN_DIR = Path(SOURCE_DIR) / "Gen"
ASSET_METADATA_DIR = "AssetMetadata"
ASSET_METADATA_OUT = f"{GEN_DIR}/AssetMetadata"

COMPILER_C = "cl"
COMPILER_CPP = "cl"

if platform.system() == "Linux":
    COMPILER_C = "gcc"
    COMPILER_CPP = "g++"

EXE_SUFFIX = ".exe"
if platform.system() == "Linux":
    EXE_SUFFIX = ""
