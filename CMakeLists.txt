cmake_minimum_required(VERSION 3.21)
include(sources.cmake)
project(AbEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ab_engine ${AB_ENGINE_SOURCES})

if(EMSCRIPTEN)
  target_sources(ab_engine PRIVATE 
    ${AB_ENGINE_SOURCES_WEB}
  )
  set_target_properties(ab_engine PROPERTIES SUFFIX ".html")
  target_compile_definitions(ab_engine PRIVATE WEBGPU_BACKEND_EMDAWNWEBGPU=1)
  
  target_compile_options(ab_engine PRIVATE "--use-port=emdawnwebgpu")
  target_link_options(ab_engine PRIVATE
    "--shell-file" "../../web/shell.html"
    "--use-port=emdawnwebgpu"
    "--closure=1"
    "-sASSERTIONS=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sASYNCIFY"
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

  target_link_libraries(ab_engine PRIVATE
    dawn::webgpu_dawn
    glfw
  )
endif()