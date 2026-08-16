cmake_minimum_required(VERSION 3.21)

project(PretopEngine LANGUAGES C CXX)

include(cmake/Sources.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/Dependencies.cmake)

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

include(cmake/Platform.cmake)
include(cmake/Tests.cmake)
