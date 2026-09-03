cmake_minimum_required(VERSION 3.21)

project(PretopEngine LANGUAGES C CXX)

include(cmake/Sources.cmake)
include(cmake/VfsFiles.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/Dependencies.cmake)

# ---------------------------------------------------------------------------
# Shared utilities
# ---------------------------------------------------------------------------

add_library(pretop_engine_shared STATIC
    ${PRETOP_ENGINE_SOURCES_SHARED}
)

set_target_properties(pretop_engine_shared PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# ---------------------------------------------------------------------------
# Core library
# ---------------------------------------------------------------------------

add_library(pretop_engine_core STATIC
    ${PRETOP_ENGINE_SOURCES}
)

set_target_properties(pretop_engine_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(pretop_engine_core PUBLIC
    include
)

target_link_libraries(pretop_engine_core PUBLIC
    pretop_engine_shared
    nlohmann_json::nlohmann_json
    magic_enum::magic_enum
    stb_image
)

target_compile_definitions(pretop_engine_core
    PUBLIC
        $<$<CONFIG:Debug>:PRETOP_DEBUG>
)

if(MSVC)
    target_compile_options(pretop_engine_core PRIVATE /Zc:__cplusplus)
endif()

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
# Python bindings
# ---------------------------------------------------------------------------

if(NOT EMSCRIPTEN)
    pybind11_add_module(pretop_python
        ${PRETOP_ENGINE_SOURCES_PYBIND}
    )

    target_link_libraries(pretop_python PRIVATE
        pretop_engine_shared
    )

    set_target_properties(pretop_python PROPERTIES
        OUTPUT_NAME "_native"
    )

    install(
        TARGETS pretop_python
        LIBRARY DESTINATION pretop COMPONENT pretop
        RUNTIME DESTINATION pretop COMPONENT pretop
    )
endif()

include(cmake/Platform.cmake)
include(cmake/Tests.cmake)
