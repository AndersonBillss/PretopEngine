
from pretop.app import App
from pretop.codegen import generate_sources
from pretop.codegen.generate_vfs_files import gen_vfs_files


def gen_cmake(app: App):
    gen_vfs_files(app.imports)
    generate_sources.gen_sources()
