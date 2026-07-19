#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace Pretop::Core
{
    class Logger
    {
    public:
        enum class Level
        {
            Info,
            Warning,
            Error,
        };

        template <typename... Args>
        static void Info(Args&&... args)
        {
            Log(Level::Info, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void Warning(Args&&... args)
        {
            Log(Level::Warning, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void Error(Args&&... args)
        {
            Log(Level::Error, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void Log(Level level, Args&&... args)
        {
            std::ostringstream oss;
            oss << Timestamp() << " [" << LevelToString(level) << "] ";
            Append(oss, std::forward<Args>(args)...);
            oss << '\n';

            const std::string message = oss.str();

            std::lock_guard<std::mutex> lock(s_mutex);
            std::cerr << message;
            std::cerr.flush();

#ifdef _WIN32
            OutputDebugStringA(message.c_str());
#endif
        }

    private:
        inline static std::mutex s_mutex;

        static const char* LevelToString(Level level)
        {
            switch (level)
            {
            case Level::Info:    return "Info";
            case Level::Warning: return "Warning";
            case Level::Error:   return "Error";
            default:             return "Unknown";
            }
        }

        static std::string Timestamp()
        {
            using clock = std::chrono::system_clock;
            const auto now = clock::now();
            const std::time_t t = clock::to_time_t(now);

            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &t);
#else
            localtime_r(&t, &tm);
#endif

            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
            return oss.str();
        }

        static void Append(std::ostringstream&)
        {
        }

        template <typename T, typename... Rest>
        static void Append(std::ostringstream& oss, T&& value, Rest&&... rest)
        {
            oss << std::forward<T>(value);
            Append(oss, std::forward<Rest>(rest)...);
        }
    };
}
