from pretop import cli
from pretop import get_asset_id
from config.importer import import_assets

print(get_asset_id("1234"))
import_assets()
cli()
