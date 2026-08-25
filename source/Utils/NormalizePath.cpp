#include "NormalizePath.hpp"
#include <algorithm>
#include <iostream>

bool strMatch(std::string_view a, int offset, std::string_view b)
{
    for (int i = 0; i < b.size(); i++)
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

int strFind(std::string_view haystack, int offset, std::string_view needle)
{
    for (int i = offset; i < haystack.size(); i++)
    {
        if (strMatch(haystack, i, needle))
        {
            return i;
        }
    }
    return -1;
}
int strFindBackwards(std::string_view haystack, int offset, std::string_view needle)
{
    for (int i = offset; i >= 0; i--)
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
        bool isLastChar = i == pathCpy.size() - 1;
        if (isLastChar && pathCpy[i] == '/')
        {
            continue;
        }
        if (strMatch(pathCpy, i, "../"))
        {
            int beforeBacktrack = strFindBackwards(normalized, normalized.size() - 1, "/");
            if (beforeBacktrack == normalized.size() - 1)
            {
                beforeBacktrack = strFindBackwards(normalized, normalized.size() - 2, "/");
            }
            size_t eraseIndex = beforeBacktrack + 1;
            if (beforeBacktrack == -1)
            {
                if (normalized.size() > 0)
                {
                    eraseIndex = 0;
                    beforeBacktrack = 0;
                }
                else
                {
                    normalized += "../";
                    i += 2;
                    continue;
                }
            }
            if (strMatch(normalized, eraseIndex, "../"))
            {
                normalized += "../";
                i += 2;
                continue;
            }
            normalized.erase(eraseIndex);
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
