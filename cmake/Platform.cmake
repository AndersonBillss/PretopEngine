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
        GIT_SUBMODULES ""
    )

    FetchContent_MakeAvailable(dawn)

    # Dawn is a pinned third-party dependency that is not maintained as part of
    # this project. Suppress its warnings so engine warnings remain visible.
    if(TARGET dawn_warnings_config)
        if(MSVC)
            target_compile_options(dawn_warnings_config INTERFACE /w)
        else()
            target_compile_options(dawn_warnings_config INTERFACE -w)
        endif()
    endif()

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
