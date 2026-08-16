cmake_minimum_required(VERSION 3.21)

project(PretopEngine LANGUAGES C CXX)

include(sources.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ---------------------------------------------------------------------------
# General dependencies
# ---------------------------------------------------------------------------

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

# ---------------------------------------------------------------------------
# Core library
# ---------------------------------------------------------------------------

add_library(pretop_engine_core STATIC
    ${PRETOP_ENGINE_SOURCES}
)

target_include_directories(pretop_engine_core PUBLIC
    include
)

target_link_libraries(pretop_engine_core PUBLIC
    nlohmann_json::nlohmann_json
    magic_enum::magic_enum
    stb_image
)

target_compile_definitions(pretop_engine_core
    PUBLIC
        $<$<CONFIG:Debug>:PRETOP_DEBUG>
)

# ---------------------------------------------------------------------------
# Main executable
# ---------------------------------------------------------------------------

add_executable(pretop_engine
    ${PRETOP_ENGINE_MAIN}
)

target_link_libraries(pretop_engine PRIVATE
    pretop_engine_core
)

# ---------------------------------------------------------------------------
# Platform-specific setup
# ---------------------------------------------------------------------------

if(EMSCRIPTEN)

    target_sources(pretop_engine PRIVATE
        ${PRETOP_ENGINE_SOURCES_WEB}
    )

    set_target_properties(pretop_engine PROPERTIES
        SUFFIX ".html"
    )

    target_compile_definitions(pretop_engine_core PRIVATE
        WEBGPU_BACKEND_EMDAWNWEBGPU=1
    )

    target_compile_options(pretop_engine_core PRIVATE
        "--use-port=emdawnwebgpu"
        "-pthread"
    )

    target_compile_options(pretop_engine PRIVATE
        "--use-port=emdawnwebgpu"
        "-pthread"
    )

    target_link_options(pretop_engine PRIVATE
        "--shell-file" "../../web/shell.html"
        "--use-port=emdawnwebgpu"
        "--preload-file" "${CMAKE_SOURCE_DIR}/assets/shaders@/assets/shaders"
        "--closure=1"
        "-sASSERTIONS=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sFETCH"
        "-sNO_DISABLE_EXCEPTION_CATCHING"
        "-pthread"
        "-sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency"
    )

else()

    # -----------------------------------------------------------------------
    # Dawn
    # -----------------------------------------------------------------------

    set(DAWN_FETCH_DEPENDENCIES ON CACHE BOOL
        "Fetch Dawn's dependencies automatically"
        FORCE
    )

    set(DAWN_FORCE_SYSTEM_COMPONENT_LOAD ON CACHE BOOL
        "Allow Dawn to load system components"
        FORCE
    )

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(DAWN_USE_WAYLAND ON CACHE BOOL
            "Enable Wayland support in Dawn"
            FORCE
        )
    endif()

    set(DAWN_BUILD_SAMPLES OFF CACHE BOOL
        "Build Dawn samples"
        FORCE
    )

    set(DAWN_BUILD_TESTS OFF CACHE BOOL
        "Build Dawn tests"
        FORCE
    )

    FetchContent_Declare(
        dawn
        GIT_REPOSITORY https://dawn.googlesource.com/dawn
        GIT_TAG a8db5510cc0e32e289f4d977a3a32c2526708bc6
        GIT_SHALLOW TRUE
        GIT_SUBMODULES ""
    )

    FetchContent_MakeAvailable(dawn)

    # -----------------------------------------------------------------------
    # Native platform sources
    # -----------------------------------------------------------------------

    target_compile_definitions(pretop_engine PRIVATE
        PRETOP_ENGINE_USE_GLFW=1
    )

    target_sources(pretop_engine PRIVATE
        ${PRETOP_ENGINE_SOURCES_NATIVE}
    )

    find_package(Threads REQUIRED)

    target_link_libraries(pretop_engine_core PUBLIC
        dawn::webgpu_dawn
        glfw
    )

    target_link_libraries(pretop_engine PRIVATE
        dawn::webgpu_dawn
        glfw
    )

    target_link_options(pretop_engine PRIVATE
        "-pthread"
    )

endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING AND NOT EMSCRIPTEN)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.0
    )

    FetchContent_MakeAvailable(Catch2)

    enable_testing()

    add_executable(pretop_engine_tests
        ${PRETOP_ENGINE_TEST_SOURCES}
        ${PRETOP_ENGINE_SOURCES_NATIVE}
    )

    target_link_libraries(pretop_engine_tests PRIVATE
        pretop_engine_core
        Catch2::Catch2WithMain
    )

    list(APPEND CMAKE_MODULE_PATH
        ${catch2_SOURCE_DIR}/extras
    )

    include(Catch)

    catch_discover_tests(pretop_engine_tests)

endif()