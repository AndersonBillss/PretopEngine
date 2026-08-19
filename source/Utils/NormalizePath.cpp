#include "NormalizePath.hpp"
#include <algorithm>

bool strMatch(std::string_view a, size_t offset, std::string_view b)
{
    for (size_t i = 0; i < b.size(); i++)
    {
        if (offset + i >= a.size())
        {
            return false;
        }
        if (a[offset + i] != b[i])
        {
            return false;
        }
    }
    return true;
}

std::string Pretop::Utils::NormalizePath(std::string_view path)
{
    std::string normalized;
    normalized.reserve(path.size());
    for (size_t i = 0; i < path.size(); i++)
    {
        if (strMatch(normalized, i, "./"))
        {
            i += 2;
        }
        else if (path[i] == '\\')
        {
            normalized += '/';
        }
        else
        {
            normalized += path[i];
        }
    }
    return normalized;
}
