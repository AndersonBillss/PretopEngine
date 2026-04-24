import platform
BUILD_DIR = "build"
COMPILER_C = "cl"
COMPILER_CPP = "cl"
if platform.system() == "Linux":
    COMPILER_C = "gcc"
    COMPILER_CPP = "g++"

EXE_SUFFIX = ".exe"
if platform.system() == "Linux":
    EXE_SUFFIX = ""