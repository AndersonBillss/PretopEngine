from pathlib import Path
import shutil
from typing import Iterator


class FileImport:
    def __init__(self, path: str | Path):
        self.path = Path(path)
        self.is_vfs = False

    def use_vfs(self):
        self.is_vfs = True

    def iter_files(self, base_asset_path: str | Path) -> Iterator[Path]:
        base_path = Path(base_asset_path)
        path = base_path / self.path
        if path.is_file():
            yield path
        elif path.is_dir():
            yield from (
                p.relative_to(base_path) for p in path.rglob("*") if p.is_file()
            )
        else:
            raise FileNotFoundError(path)


class CopyImport(FileImport):
    def import_file(self, source: Path, destination: Path):
        shutil.copy(source, destination)
