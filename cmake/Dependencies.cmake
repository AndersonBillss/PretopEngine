include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG master
)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

# Avoid building GLFW extras that PretopEngine does not need.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL
    "Build GLFW examples"
    FORCE
)

set(GLFW_BUILD_TESTS OFF CACHE BOOL
    "Build GLFW tests"
    FORCE
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL
    "Build GLFW documentation"
    FORCE
)

FetchContent_MakeAvailable(
    nlohmann_json
    magic_enum
    stb
    glfw
)

add_library(stb_image INTERFACE)

target_include_directories(stb_image INTERFACE
    ${stb_SOURCE_DIR}
)
