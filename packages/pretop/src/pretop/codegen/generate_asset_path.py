from pathlib import Path

from pretop.shared.constants import (
    ASSET_OUTPUT_DIR,
    CPP_COMMENT,
    GENERATED_CODE_COMMENT,
    PLATFORM_INCLUDE,
    WEB_ASSET_PATH_PREFIX,
    GEN_DIR,
    WEB_PLATFORM_DEFINE,
)


def createAssetBaseDeclaration(asset_path):
    return f'constexpr std::string_view AssetBase = "{asset_path}";\n'


FILE_CONTENTS = (
    f"{CPP_COMMENT} {GENERATED_CODE_COMMENT}\n"
    + "#include <string_view>\n"
    + f"{PLATFORM_INCLUDE}\n"
    + f"#ifdef {WEB_PLATFORM_DEFINE}\n"
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
