from pkgutil import extend_path

# The pure Python CLI may be loaded from the source tree while the optional
# C++ extension is installed into site-packages.
__path__ = extend_path(__path__, __name__)


def _refresh_package_path() -> None:
    global __path__
    __path__ = extend_path(__path__, __name__)

from pretop.cli import cli
from pretop.app import App


def add(a: int, b: int) -> int:
    from pretop._native import add as native_add

    return native_add(a, b)


def get_asset_id(path: str) -> int:
    from pretop._native import get_asset_id as native_get_asset_id

    return native_get_asset_id(path)

__all__ = ["cli", "App", "get_asset_id"]
