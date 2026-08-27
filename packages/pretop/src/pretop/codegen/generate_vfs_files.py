from pathlib import Path

from pretop.app import ImportConfig
from pretop.shared.constants import (
    ASSET_OUTPUT_DIR,
    CMAKE_COMMENT,
    GENERATED_CODE_COMMENT,
    WEB_ASSET_PATH_PREFIX,
)
from pretop.utils.file_utls import save_if_changed

VFS_FILES_DIR = "cmake/VfsFiles.cmake"
VFS_SOURCES_RULE_NAME = "VFS_FILES"


def gen_vfs_files(import_config: ImportConfig):
    file_start = (
        f"{CMAKE_COMMENT} {GENERATED_CODE_COMMENT}\n"
        + f"set(PRETOP_ASSET_DIR\n"
        + f"  {ASSET_OUTPUT_DIR}\n"
        + ")\n"
        + f"set(WEB_ASSET_PATH_PREFIX\n"
        + f"  {WEB_ASSET_PATH_PREFIX}"
        + "\n)"
        + f"\nset({VFS_SOURCES_RULE_NAME}\n"
    )
    file_end = ")\n"
    file_contents = file_start
    for asset_import in import_config.get_imports():
        if asset_import.is_vfs:
            for file_import in asset_import.iter_files(import_config.get_asset_dir()):
                file_contents += f"  {file_import}\n"

    file_contents += file_end

    save_if_changed(VFS_FILES_DIR, file_contents)
