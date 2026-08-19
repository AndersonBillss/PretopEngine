#include "NormalizePath.hpp"
#include <algorithm>
#include <iostream>

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

int strFind(std::string_view haystack, size_t offset, std::string_view needle)
{
    for (size_t i = offset; i < haystack.size(); i++)
    {
        if (strMatch(haystack, i, needle))
        {
            return i;
        }
    }
    return -1;
}
int strFindBackwards(std::string_view haystack, size_t offset, std::string_view needle)
{
    for (size_t i = offset; i >= 0; i--)
    {
        if (strMatch(haystack, i, needle))
        {
            return i;
        }
    }
    return -1;
}

std::string Pretop::Utils::NormalizePath(std::string_view path)
{
    std::string pathCpy(path);
    std::replace(pathCpy.begin(), pathCpy.end(), '\\', '/');
    std::string normalized;
    normalized.reserve(path.size());
    for (size_t i = 0; i < pathCpy.size(); i++)
    {
        if (strMatch(pathCpy, i, "../"))
        {
            int beforeBacktrack = strFindBackwards(normalized, normalized.size() - 1, "/");
            if (beforeBacktrack == normalized.size() - 1)
            {
                beforeBacktrack = strFindBackwards(normalized, normalized.size() - 2, "/");
            }
            size_t numCharsToErase = i - beforeBacktrack;
            normalized.erase(normalized.size() - numCharsToErase);
            i += 2;
        }
        else if (strMatch(pathCpy, i, "./"))
        {
            i++;
        }
        else
        {
            normalized += pathCpy[i];
        }
    }
    return normalized;
}
