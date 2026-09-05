#pragma once
#include <string>

namespace Pretop::Asset
{
    enum SourceType
    {
        VFS,
        HTTP,
    };

    struct FileMetadata
    {
        std::string path;
#ifdef PRETOP_PLATFORM_WEB
        SourceType SourceType;
#endif
    };
}
