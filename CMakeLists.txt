cmake_minimum_required(VERSION 3.21)
project(AbEngine LANGUAGES C CXX)

include(sources.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(nlohmann_json)

add_library(ab_engine_core STATIC ${AB_ENGINE_SOURCES})
target_include_directories(ab_engine_core PUBLIC include)
target_link_libraries(ab_engine_core PUBLIC nlohmann_json::nlohmann_json)

add_executable(ab_engine ${AB_ENGINE_MAIN})
target_link_libraries(ab_engine PRIVATE ab_engine_core)

if(EMSCRIPTEN)
  target_sources(ab_engine PRIVATE
    ${AB_ENGINE_SOURCES_WEB}
  )

  set_target_properties(ab_engine PROPERTIES SUFFIX ".html")

  target_compile_definitions(ab_engine_core PRIVATE WEBGPU_BACKEND_EMDAWNWEBGPU=1)
  target_compile_options(ab_engine_core PRIVATE 
    "--use-port=emdawnwebgpu"
  )
  target_compile_options(ab_engine PRIVATE 
    "--use-port=emdawnwebgpu"
  )

  target_link_options(ab_engine PRIVATE
    "--shell-file" "../../web/shell.html"
    "--use-port=emdawnwebgpu"
    "--preload-file" "${CMAKE_SOURCE_DIR}/assets/shaders@/assets/shaders"
    "--closure=1"
    "-sASSERTIONS=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sASYNCIFY"
    "-sFETCH"
    "-sNO_DISABLE_EXCEPTION_CATCHING"
  )
else()
  target_compile_definitions(ab_engine PRIVATE AB_ENGINE_USE_GLFW=1)
  target_sources(ab_engine PRIVATE
    ${AB_ENGINE_SOURCES_NATIVE}
  )

  add_subdirectory(third_party/dawn/third_party/glfw)

  list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/build/_install/dawn/debug")
  find_package(Dawn CONFIG REQUIRED)
  find_package(Threads REQUIRED)

  target_link_libraries(ab_engine_core PUBLIC
    dawn::webgpu_dawn
    glfw
  )

  target_link_libraries(ab_engine PRIVATE
    dawn::webgpu_dawn
    glfw
  )
endif()

option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING AND NOT EMSCRIPTEN)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.0
  )
  FetchContent_MakeAvailable(Catch2)

  enable_testing()

  add_executable(ab_engine_tests
    ${AB_ENGINE_TEST_SOURCES}
  )

  target_link_libraries(ab_engine_tests PRIVATE
    ab_engine_core
    Catch2::Catch2WithMain
  )

  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
  include(Catch)
  catch_discover_tests(ab_engine_tests)
endif()