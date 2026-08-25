from pretop.app import App
from pretop.codegen.gen_asset_metadata import gen_asset_metadata
from pretop.codegen.gen_asset_path import gen_asset_path


def gen_code(app: App):
    gen_asset_path()
    gen_asset_metadata(app.imports.get_imports())
