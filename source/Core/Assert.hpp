#pragma once
#include "Logger.hpp"

#ifdef PRETOP_DEBUG
#define PRETOP_ASSERT(expr, msg)                                                          \
    do                                                                                    \
    {                                                                                     \
        if (!(expr))                                                                      \
        {                                                                                 \
            Pretop::Core::Logger::Error("ASSERT FAILED: ", msg, " (", __FILE__, ":", __LINE__, ")"); \
            std::abort();                                                                 \
        }                                                                                 \
    } while (false)
#else
#define PRETOP_ASSERT(expr, msg) ((void)0)
#endif
