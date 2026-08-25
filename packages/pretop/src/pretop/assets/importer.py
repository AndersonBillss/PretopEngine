from pathlib import Path

from pretop.app import App
from pretop.codegen.generate_vfs_files import gen_vfs_files
from pretop.shared.constants import ASSET_OUTPUT_DIR


def import_assets(app: App):
    Path(ASSET_OUTPUT_DIR).parent.mkdir(parents=True, exist_ok=True)
    gen_vfs_files(app.imports)
    for asset_import in app.imports.get_imports():
        for impt in asset_import.iter_files(app.imports.get_asset_dir()):
            source = Path(app.imports.get_asset_dir()) / impt
            destination = Path(ASSET_OUTPUT_DIR) / impt
            destination.parent.mkdir(parents=True, exist_ok=True)
            asset_import.import_file(source, destination)
