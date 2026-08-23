
from pathlib import Path
import shutil


class FileImport:
    def __init__(self, path: str | Path):
        self.path = Path(path)
        self.is_vfs = False

    def use_vfs(self):
        self.is_vfs = True


class CopyImport(FileImport):
    def import_file(self, source: Path, destination: Path):
        shutil.copy(source, destination)
