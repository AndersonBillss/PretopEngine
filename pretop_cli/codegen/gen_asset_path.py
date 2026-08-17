from pathlib import Path

from pretop_cli.shared.constants import ASSET_DIR, ENGINE_DIR, GEN_DIR

FILE_COMMENT = "// Generate by Pretop CLI, do not touch\n"
FILE_HEADER = "#include <iostream>\n#include <string_view>\n\n"
ASSET_BASE_DECLARATION = f"constexpr std::string_view AssetBase = \"{ENGINE_DIR}/{ASSET_DIR}\";\n"
FILE_CONTENTS = FILE_COMMENT + FILE_HEADER + ASSET_BASE_DECLARATION

def gen_asset_path():
    output_file = Path(GEN_DIR) / Path("AssetBase.hpp") 
    output_file.parent.mkdir(parents=True, exist_ok=True)
    with open(output_file, "w") as file:
        file.write(FILE_CONTENTS)
