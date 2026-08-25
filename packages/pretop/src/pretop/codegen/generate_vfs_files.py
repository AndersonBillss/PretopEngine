from pathlib import Path

from pretop.app import ImportConfig
from pretop.shared.constants import (
    ASSET_OUTPUT_DIR,
    CMAKE_COMMENT,
    GENERATED_CODE_COMMENT,
)

VFS_FILES_DIR = "cmake/VfsFiles.cmake"
VFS_SOURCES_RULE_NAME = "VFS_FILES"


def gen_vfs_files(import_config: ImportConfig):
    file_start = (
        f"{CMAKE_COMMENT} {GENERATED_CODE_COMMENT}\n"
        + f"set(ASSET_DIR\n"
        + f"  {ASSET_OUTPUT_DIR}\n)\nset({VFS_SOURCES_RULE_NAME}\n"
    )
    file_end = ")\n"
    file_contents = file_start
    for asset_import in import_config.get_imports():
        for file_import in asset_import.iter_files(import_config.get_asset_dir()):
            file_contents += f"  {file_import}\n"

    file_contents += file_end
    with open(VFS_FILES_DIR, "w") as output_file:
        output_file.write(file_contents)
