#pragma once
#include <stdexcept>
#include <string>

namespace Pretop::Asset
{
    class ModelParseError : public std::runtime_error
    {
    public:
        explicit ModelParseError(const std::string &message)
            : std::runtime_error(message)
        {
        }
    };
} // namespace Pretop::Asset
