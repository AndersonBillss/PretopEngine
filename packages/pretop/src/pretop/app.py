from pathlib import Path

from pretop.assets.file_import import CopyImport, FileImport


class ImportConfig:
    def __init__(self):
        self.__file_imports: list[FileImport] = []
    
    def use_asset_dir(self, path: Path | str):
        self.__asset_path = Path(path)

    def import_copy(self, path: Path | str):
        file_import = CopyImport(path)
        self.__file_imports.append(file_import)
        return file_import

    def get_asset_dir(self) -> list[FileImport]:
        return self.__asset_path

    def get_imports(self) -> list[FileImport]:
        return self.__file_imports


class App:
    def __init__(self):
        self.imports = ImportConfig()
