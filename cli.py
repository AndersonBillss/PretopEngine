from pretop import cli
from pretop import App
from config.importer import import_assets

app = App()
import_assets(app)
cli(app)
