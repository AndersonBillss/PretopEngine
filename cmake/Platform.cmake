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
        "-fwasm-exceptions"
    )

    target_compile_options(pretop_engine PRIVATE
        "--use-port=emdawnwebgpu"
        "-pthread"
        "-fwasm-exceptions"
    )

    target_link_options(pretop_engine PRIVATE
        "--shell-file" "../../web/shell.html"
        "--use-port=emdawnwebgpu"
        "--closure=1"
        "-sASSERTIONS=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sFETCH"
        "-fwasm-exceptions"
        "-pthread"
        "-sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency"
    )

    foreach(vfs_file IN LISTS VFS_FILES)
        target_link_options(pretop_engine PRIVATE
            "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/${PRETOP_ASSET_DIR}/${vfs_file}@/${WEB_ASSET_PATH_PREFIX}/${vfs_file}"
        )
    endforeach()

else()

    # -----------------------------------------------------------------------
    # Dawn
    # -----------------------------------------------------------------------

    set(PRETOP_DAWN_INSTALL_DIR "${CMAKE_SOURCE_DIR}/third_party/dawn-install")
    set(PRETOP_DAWN_CONFIG "${PRETOP_DAWN_INSTALL_DIR}/lib/cmake/Dawn/DawnConfig.cmake")
    if(NOT EXISTS "${PRETOP_DAWN_CONFIG}")
        message(FATAL_ERROR
            "Dawn is not installed. Run the Pretop CLI setup command to clone and build it."
        )
    endif()
    find_package(Dawn CONFIG REQUIRED
        PATHS "${PRETOP_DAWN_INSTALL_DIR}/lib/cmake/Dawn"
        NO_DEFAULT_PATH
    )

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
