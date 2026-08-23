from pathlib import Path

VFS_FILES_DIR = "cmake/VfsFiles.cmake"
STARTING_COMMENT = "# Auto-generated code, do not edit"
VFS_SOURCES_RULE_NAME = "VFS_FILES"


def gen_vfs(asset_dir: Path, files: list[Path | str]):
    file_start = f"{STARTING_COMMENT}\nset({VFS_SOURCES_RULE_NAME}\n"
    file_end = ")\n"

    file_contents = file_start
    for file in files:
        file_contents += f"  {asset_dir / Path(file)}\n"

    file_contents += file_end

    with open(VFS_FILES_DIR, "w") as output_file:
        output_file.write(file_contents)
