
from pretop.app import App
from pretop.codegen.generate_cmake import gen_cmake
from pretop.codegen.generate_code import gen_code


def gen_all(app: App):
    gen_code(app)
    gen_cmake(app)