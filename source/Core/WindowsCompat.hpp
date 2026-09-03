#pragma once

#include "Platform.hpp"

#ifdef PRETOP_PLATFORM_WINDOWS

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#ifdef CreateWindow
#undef CreateWindow
#endif

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

#endif