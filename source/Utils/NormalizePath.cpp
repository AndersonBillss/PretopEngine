#include "NormalizePath.hpp"
#include <algorithm>
#include <iostream>
#include <cctype>

bool isWhitespace(char ch)
{
    return std::isspace(static_cast<unsigned char>(ch));
}

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

bool endMatch(std::string_view a, int offset, std::string_view b)
{
    if (a.size() - offset != b.size())
    {
        return false;
    }
    return strMatch(a, offset, b);
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
    if (path.size() == 0)
    {
        return {};
    }

    size_t pathBegin = 0;
    for (size_t i = 0; i < path.size(); i++)
    {
        if (!isWhitespace(path[i]))
        {
            pathBegin = i;
            break;
        }
    }

    size_t pathEnd = 0;
    for (size_t i = path.size() - 1; i > pathBegin; i--)
    {
        if (!isWhitespace(path[i]))
        {
            pathEnd = i;
            break;
        }
    }

    if (pathEnd < pathBegin)
    {
        return {};
    }

    if (pathBegin == pathEnd && isWhitespace(path[pathBegin]))
    {
        return {};
    }

    std::string sanitizedPath;
    sanitizedPath.reserve(pathEnd - pathBegin);

    std::string normalized;
    normalized.reserve(sanitizedPath.size());

    for (size_t i = pathBegin; i <= pathEnd; i++)
    {
        if (path[i] == '\\')
        {
            sanitizedPath += '/';
        }
        else
        {
            sanitizedPath += path[i];
        }
    }

    for (size_t i = 0; i < sanitizedPath.size(); i++)
    {
        bool isLastChar = i == sanitizedPath.size() - 1;
        if (isLastChar && sanitizedPath[i] == '/' && normalized.size() > 0)
        {
            continue;
        }

        char lastAddedChar = '\0';
        if (normalized.size() > 0)
        {
            lastAddedChar = normalized[normalized.size() - 1];
        }

        if (strMatch(sanitizedPath, i, "../") || endMatch(sanitizedPath, i, ".."))
        {
            if (lastAddedChar != '/')
            {
                normalized += "../";
                i += 2;
                continue;
            }
            int beforeBacktrack = strFindBackwards(normalized, normalized.size() - 2, "/");
            size_t eraseIndex = beforeBacktrack + 1;
            if (eraseIndex == 0 && normalized[0] == '/')
            {
                continue;
            }
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
        else if (lastAddedChar == '/' && sanitizedPath[i] == '/')
        {
            continue;
        }
        else if (strMatch(sanitizedPath, i, "./") || endMatch(sanitizedPath, i, "."))
        {
            i++;
        }
        else
        {
            normalized += sanitizedPath[i];
        }
    }
    return normalized;
}
