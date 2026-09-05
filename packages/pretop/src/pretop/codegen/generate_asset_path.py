from pathlib import Path

from pretop.shared.constants import (
    ASSET_OUTPUT_DIR,
    CPP_COMMENT,
    GENERATED_CODE_COMMENT,
    PLATFORM_INCLUDE_PATH,
    WEB_ASSET_PATH_PREFIX,
    GEN_DIR,
    WEB_PLATFORM_DEFINE,
)
from pretop.utils.file_utls import save_if_changed
from pretop._native import normalize_path


def createAssetBaseDeclaration(asset_path):
    return (
        f'constexpr std::string_view AssetBase = "{normalize_path(str(asset_path))}";\n'
    )

PLATFORM_INCLUDE = f'#include "{PLATFORM_INCLUDE_PATH}"'

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

    save_if_changed(output_file, FILE_CONTENTS)
