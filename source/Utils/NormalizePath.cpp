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

int findBacktrack(std::string haystack, size_t offset, int *precedingSlash, int *subsequentSlash)
{
    int nextBacktrack = strFind(haystack, 0, "../");
    if (nextBacktrack == -1)
    {
        *precedingSlash = -1;
        *subsequentSlash = -1;
    }
    else
    {
        *precedingSlash = strFindBackwards(haystack, offset - 1, "/");
        *subsequentSlash = strFind(haystack, offset + 3, "/");
    }
    return nextBacktrack;
}

std::string Pretop::Utils::NormalizePath(std::string_view path)
{
    std::string pathCpy(path);
    std::replace(pathCpy.begin(), pathCpy.end(), '\\', '/');
    std::string normalized;
    normalized.reserve(path.size());
    // int slashBeforeBacktrack = -1;
    // int slashAfterBacktrack = -1;
    // int nextBacktrack = strFind(pathCpy, 0, "../");
    for (size_t i = 0; i < pathCpy.size(); i++)
    {
        if (strMatch(pathCpy, i, "../"))
        {
            int beforeBacktrack = strFindBackwards(pathCpy, i - 1, "/");
            size_t numCharsToErase = i - beforeBacktrack;
            normalized.erase(normalized.size() - numCharsToErase);
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
