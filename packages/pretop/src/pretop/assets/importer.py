from pathlib import Path
import shutil
from typing import Iterator

from pretop.shared.constants import ASSET_INPUT_DIR, ASSET_OUTPUT_DIR


class FileImport:
    def __init__(self, path: str | Path):
        self.path = Path(path)


class CopyImport(FileImport):
    def import_file(self, source: Path, destination: Path):
        shutil.copy(source, destination)


def iter_files(path: str | Path) -> Iterator[Path]:
    path = Path(path)
    if path.is_file():
        yield path
    elif path.is_dir():
        yield from (p for p in path.rglob("*") if p.is_file())
    else:
        raise FileNotFoundError(path)


file_imports: list[FileImport] = []


def import_copy(path: str):
    file_imports.append(CopyImport(path))


def import_assets():
    ASSET_OUTPUT_DIR.parent.mkdir(parents=True, exist_ok=True)
    for file_import in file_imports:
        for impt in iter_files(ASSET_INPUT_DIR / file_import.path):
            relative_impt = impt.relative_to(impt.parts[0])
            source = ASSET_INPUT_DIR / relative_impt
            destination = ASSET_OUTPUT_DIR / relative_impt
            destination.parent.mkdir(parents=True, exist_ok=True)
            file_import.import_file(source, destination)
