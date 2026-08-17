import platform

ENGINE_NAME = "pretop_engine"
BUILD_DIR = "build"
ENGINE_DIR = ".pretop"
ASSET_DIR = "assets"
SOURCE_DIR = "source"
COMPILER_C = "cl"
COMPILER_CPP = "cl"
if platform.system() == "Linux":
    COMPILER_C = "gcc"
    COMPILER_CPP = "g++"

EXE_SUFFIX = ".exe"
if platform.system() == "Linux":
    EXE_SUFFIX = ""