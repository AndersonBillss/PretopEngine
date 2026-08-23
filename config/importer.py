from pretop.app import App


def import_assets(app: App):
    app.imports.import_copy("models")
    app.imports.import_copy("shaders")
    app.imports.import_copy("textures")
