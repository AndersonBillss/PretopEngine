from pathlib import Path
from types import ModuleType
import importlib.util


def load_module(path: str | Path) -> ModuleType:
    path = Path(path)

    spec = importlib.util.spec_from_file_location(path.stem, path)

    if spec is None or spec.loader is None:
        raise ImportError(f"Could not load module: {path}")

    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)

    return module
