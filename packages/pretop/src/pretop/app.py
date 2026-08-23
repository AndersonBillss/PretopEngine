from pretop.assets.file_import import CopyImport, FileImport


class ImportConfig:
    def __init__(self):
        self.__file_imports: list[FileImport] = []

    def import_copy(self, path: str):
        self.__file_imports.append(CopyImport(path))

    def get_imports(self) -> list[FileImport]:
        return self.__file_imports


class App:
    def __init__(self):
        self.imports = ImportConfig()
