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
