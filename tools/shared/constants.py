import platform
BUILD_DIR = "build"
SOURCE_DIR = "Source"
COMPILER_C = "cl"
COMPILER_CPP = "cl"
ENGINE_NAME = "pretop_engine"
if platform.system() == "Linux":
    COMPILER_C = "gcc"
    COMPILER_CPP = "g++"

EXE_SUFFIX = ".exe"
if platform.system() == "Linux":
    EXE_SUFFIX = ""