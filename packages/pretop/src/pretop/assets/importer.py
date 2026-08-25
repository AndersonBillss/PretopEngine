from pathlib import Path
from typing import Iterator

from pretop.app import App
from pretop.codegen.gen_vfs import gen_vfs
from pretop.shared.constants import ASSET_INPUT_DIR, ASSET_OUTPUT_DIR


def import_assets(app: App):
    Path(ASSET_OUTPUT_DIR).parent.mkdir(parents=True, exist_ok=True)
    for file_import in app.imports.get_imports():
        files_to_import = [
            f for f in file_import.iter_files(ASSET_INPUT_DIR)
        ]
        if file_import.is_vfs:
            gen_vfs(files_to_import)

        for impt in files_to_import:
            source = Path(ASSET_INPUT_DIR) / impt
            destination = Path(ASSET_OUTPUT_DIR) / impt
            destination.parent.mkdir(parents=True, exist_ok=True)
            file_import.import_file(source, destination)
