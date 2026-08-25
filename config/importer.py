from pretop.app import App


def import_assets(app: App):
    app.imports.use_asset_dir("assets")

    app.imports.import_copy("models")
    app.imports.import_copy("shaders").use_vfs()
    app.imports.import_copy("textures")
