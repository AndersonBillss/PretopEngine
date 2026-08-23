from pathlib import Path

from pretop.shared.constants import ASSET_OUTPUT_DIR

VFS_FILES_DIR = "cmake/VfsFiles.cmake"
STARTING_COMMENT = "# Auto-generated code, do not edit"
VFS_SOURCES_RULE_NAME = "VFS_FILES"


def gen_vfs(files: list[Path | str]):
    file_start = f"{STARTING_COMMENT}\nset(ASSET_DIR\n  {ASSET_OUTPUT_DIR}\n)\nset({VFS_SOURCES_RULE_NAME}\n"
    file_end = ")\n"

    file_contents = file_start
    for file in files:
        file_contents += f"  {file}\n"

    file_contents += file_end

    with open(VFS_FILES_DIR, "w") as output_file:
        output_file.write(file_contents)
