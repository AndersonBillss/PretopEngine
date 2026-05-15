#pragma once

#ifdef __EMSCRIPTEN__
#include "WEB_getBytes.hpp"
#else
#include "NATIVE_getBytes.hpp"
#endif