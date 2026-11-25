from pathlib import Path
import sys

PACKAGE_SOURCE = Path(__file__).resolve().parent / "packages" / "pretop" / "src"
sys.path.insert(0, str(PACKAGE_SOURCE))

from pretop import cli
from pretop import App

app = App()
cli(app)
