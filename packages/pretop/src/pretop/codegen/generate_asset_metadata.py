from pathlib import Path

from pretop.app import ImportConfig
from pretop.assets.file_import import FileImport
from pretop.shared.constants import (
    ASSET_METADATA_DIR,
    ASSET_METADATA_OUT,
    CPP_COMMENT,
    GEN_NAMESPACE,
    GENERATED_CODE_COMMENT,
    PLATFORM_INCLUDE,
    WEB_PLATFORM_DEFINE,
)
from pretop._native import get_asset_id

HEADER_CONTENTS = (
    f"{CPP_COMMENT} {GENERATED_CODE_COMMENT}\n"
    + "#pragma once\n"
    + "#include <unordered_map>\n"
    + "#include <string>\n"
    + "#include <cstdint>\n"
    + f"{PLATFORM_INCLUDE}\n"
    + "\n"
    + f"namespace {GEN_NAMESPACE}\n"
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
    + f"#ifdef {WEB_PLATFORM_DEFINE}\n"
    + f"        {GEN_NAMESPACE}::SourceType SourceType;\n"
    + "#endif\n"
    + "    };\n"
    + "\n"
    + "    extern const std::unordered_map<uint64_t, FileMetadata> AssetMetadata;\n"
    + "}\n"
)


def create_metadata(asset_import: FileImport, file_import: Path):
    return (
        "    {\n"
        + f"        {get_asset_id(str(file_import))}ULL,\n"
        + "        {\n"
        + f'            "{file_import}",\n'
        + f"#ifdef {WEB_PLATFORM_DEFINE}\n"
        + f"            {GEN_NAMESPACE}::SourceType::{"VFS" if asset_import.is_vfs else "HTTP"},\n"
        + "#endif\n"
        + "        },\n"
        + "    },\n"
    )


def gen_asset_metadata(import_config: ImportConfig):
    file_imports = import_config.get_imports()
    asset_input_dir = import_config.get_asset_dir()

    CPP_CONTENTS_START = (
        f"{CPP_COMMENT} {GENERATED_CODE_COMMENT}\n"
        + f'#include "{ASSET_METADATA_DIR}.hpp"\n'
        + "\n"
        + f"const std::unordered_map<uint64_t, {GEN_NAMESPACE}::FileMetadata> {GEN_NAMESPACE}::AssetMetadata = "
        + "{\n"
    )
    CPP_CONTENTS_END = "};\n"

    cpp_contents = CPP_CONTENTS_START
    for asset_import in file_imports:
        for file_import in asset_import.iter_files(asset_input_dir):
            cpp_contents += create_metadata(asset_import, file_import)

    cpp_contents += CPP_CONTENTS_END

    OUT_HPP = ASSET_METADATA_OUT + ".hpp"
    OUT_CPP = ASSET_METADATA_OUT + ".cpp"

    with open(OUT_HPP, "w") as header_output:
        header_output.write(HEADER_CONTENTS)

    with open(OUT_CPP, "w") as cpp_output:
        cpp_output.write(cpp_contents)
