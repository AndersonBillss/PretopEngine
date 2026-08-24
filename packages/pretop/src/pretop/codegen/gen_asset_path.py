from pathlib import Path

from pretop.shared.constants import ASSET_OUTPUT_DIR, WEB_ASSET_PATH_PREFIX, GEN_DIR


def createAssetBaseDeclaration(asset_path):
    return f'constexpr std::string_view AssetBase = "{asset_path}";\n'


NATIVE_ASSET_PATH = ASSET_OUTPUT_DIR
WEB_ASSET_PATH = WEB_ASSET_PATH_PREFIX
FILE_COMMENT = "// Generate by Pretop CLI, do not touch\n"
FILE_HEADER = (
    '#include <iostream>\n#include <string_view>\n#include "../Core/Platform.hpp"\n\n'
)
FILE_CONTENTS = (
    FILE_COMMENT
    + FILE_HEADER
    + "#ifdef PRETOP_PLATFORM_WEB\n"
    + createAssetBaseDeclaration(WEB_ASSET_PATH_PREFIX)
    + "#else\n"
    + createAssetBaseDeclaration(ASSET_OUTPUT_DIR)
    + "#endif\n"
)


def gen_asset_path():
    output_file = Path(GEN_DIR) / Path("AssetBase.hpp")
    output_file.parent.mkdir(parents=True, exist_ok=True)
    with open(output_file, "w") as file:
        file.write(FILE_CONTENTS)
