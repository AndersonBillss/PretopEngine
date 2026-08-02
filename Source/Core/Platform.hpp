#pragma once

#if defined(__EMSCRIPTEN__)
#define PRETOP_PLATFORM_WEB
#elif defined(_WIN32)
#define PRETOP_PLATFORM_WINDOWS
#elif defined(__linux__)
#define PRETOP_PLATFORM_LINUX
#else
#error Unsupported platform
#endif