from pathlib import Path

from pretop.assets.file_import import FileImport
from pretop.shared.constants import (
    ASSET_INPUT_DIR,
    ASSET_METADATA_DIR,
    ASSET_METADATA_OUT,
)
from pretop._native import get_asset_id

HEADER_CONTENTS = (
    "#pragma once\n"
    + "#include <unordered_map>\n"
    + "#include <string>\n"
    + "#include <cstdint>\n"
    + '#include "../Core/Platform.hpp"\n'
    + "\n"
    + "namespace Pretop::Gen\n"
    + "{\n"
    + "    enum SourceType\n"
    + "    {\n"
    + "        VFS,\n"
    + "        HTTP,\n"
    + "    };\n"
    + "\n"
    + "    struct FileMetadata\n"
    + "    {\n"
    + "        std::string path;\n"
    + "#ifdef PRETOP_PLATFORM_WEB\n"
    + "        Pretop::Gen::SourceType SourceType;\n"
    + "#endif\n"
    + "    };"
    + "\n"
    + "    extern const std::unordered_map<uint64_t, FileMetadata> AssetMetadata;\n"
    + "}\n"
)


def create_metadata(asset_import: FileImport, file_import: Path):
    return (
        "    {\n"
        + f"        {get_asset_id(str(file_import))},\n"
        + "        {\n"
        + f'            "{file_import}",\n'
        + "#ifdef PRETOP_PLATFORM_WEB\n"
        + f"            Pretop::Gen::SourceType::{"VFS" if asset_import.is_vfs else "HTTP"},\n"
        + "#endif\n"
        + "        },\n"
        + "    },\n"
    )


def gen_asset_metadata(file_imports: list[FileImport]):
    CPP_CONTENTS_START = (
        f'#include "{ASSET_METADATA_DIR}.hpp"\n'
        + "\n"
        + "const std::unordered_map<uint64_t, Pretop::Gen::FileMetadata> Pretop::Gen::AssetMetadata = {\n"
    )
    CPP_CONTENTS_END = "};\n"

    cpp_contents = CPP_CONTENTS_START
    for asset_import in file_imports:
        for file_import in asset_import.iter_files(ASSET_INPUT_DIR):
            cpp_contents += create_metadata(asset_import, file_import)

    cpp_contents += CPP_CONTENTS_END

    OUT_HPP = ASSET_METADATA_OUT + ".hpp"
    OUT_CPP = ASSET_METADATA_OUT + ".cpp"

    with open(OUT_HPP, "w") as header_output:
        header_output.write(HEADER_CONTENTS)

    with open(OUT_CPP, "w") as cpp_output:
        cpp_output.write(cpp_contents)
