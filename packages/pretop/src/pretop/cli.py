from subprocess import SubprocessError
from enum import Enum
import importlib
import sys

from pretop.app import App


class ArgType(Enum):
    NONE = 0
    STR_LIST = 1


class Command:
    def __init__(
        self,
        fn,
        description: str,
        arg_type: ArgType = ArgType.NONE,
        requires_cpp_package: bool = True,
    ):
        self.fn = fn
        self.description = description
        self.arg_type = arg_type
        self.requires_cpp_package = requires_cpp_package

    def run(self, app: App, args: list[str]):
        if self.arg_type == ArgType.NONE:
            self.fn(app=app)
        else:
            self.fn(app=app, args=args)


DESCRIPTION = "AB Engine: A performant game engine built for web and native"
separation_line = ""
for _ in DESCRIPTION:
    separation_line += "-"


def print_desc():
    print(separation_line)
    print(DESCRIPTION)
    print(separation_line)


def print_help(cmdTree: dict, tabs: int = 1):
    tab_prefix = ""
    for _ in range(tabs):
        tab_prefix += "  "

    max_key_size = 0
    for key, val in cmdTree.items():
        if isinstance(val, Command) and (len(key) > max_key_size):
            max_key_size = len(key)

    for key, val in cmdTree.items():
        padding = ""
        for _ in range(max_key_size - len(key)):
            padding += " "

        if isinstance(val, Command):
            print(f"{tab_prefix}{key}:  {padding}{val.description}")
        else:
            print(separation_line)
            print(f"{tab_prefix}{key}:")
            print_help(val, tabs + 1)


def help(app=None):
    print_desc()
    print_help(COMMANDS)

def setup(app: App):
    from pretop.build_scripts.dawn import setup_dawn
    from pretop.build_scripts.native_package import build_shared_python_package

    setup_dawn()
    build_shared_python_package()

    import pretop

    pretop._refresh_package_path()
    importlib.invalidate_caches()
    importlib.import_module("pretop._native")


def build_native_debug(app=None):
    from pretop.assets.importer import import_assets
    from pretop.codegen.generate_all import gen_all
    from pretop.build_scripts.native import build_native_debug as build

    import_assets(app)
    gen_all(app)
    return build(app=app)


def build_python(app=None):
    from pretop.build_scripts.native import build_python as build

    return build(app=app)


def run_native_debug(app=None):
    from pretop.assets.importer import import_assets
    from pretop.codegen.generate_all import gen_all
    from pretop.build_scripts.native import run_native_debug as run

    import_assets(app)
    gen_all(app)
    return run(app=app)


def run_tests(args: list[str], app=None):
    from pretop.assets.importer import import_assets
    from pretop.codegen.generate_all import gen_all
    from pretop.build_scripts.native import run_tests as run

    import_assets(app)
    gen_all(app)
    return run(args=args, app=app)


def build_web_debug(app=None):
    from pretop.build_scripts.web import build_web_debug as build

    return build(app=app)


def run_web_debug(app=None):
    from pretop.build_scripts.web import run_web_debug as run

    return run(app=app)


def gen_all(app: App):
    from pretop.assets.importer import import_assets
    from pretop.codegen.generate_all import gen_all as generate

    import_assets(app)
    return generate(app=app)


def gen_cmake(app: App):
    from pretop.codegen.generate_cmake import gen_cmake as generate

    return generate(app=app)


def gen_code(app: App):
    from pretop.assets.importer import import_assets
    from pretop.codegen.generate_code import gen_code as generate

    import_assets(app)
    return generate(app=app)


def gen_sources(app=None):
    from pretop.codegen.generate_sources import gen_sources as generate

    return generate(app=app)


def import_assets(app: App):
    from pretop.assets.importer import import_assets as import_files

    return import_files(app=app)

COMMANDS = {
    "setup": Command(setup, "Initial setup for engine", requires_cpp_package=False),
    "help": Command(help, "Show help", requires_cpp_package=False),
    "build": {
        "native": Command(build_native_debug, "Build engine for native"),
        "web": Command(build_web_debug, "Build engine for web"),
        "python": Command(build_python, "Build C++ python module"),
    },
    "run": {
        "native": Command(run_native_debug, "Run the engine for native"),
        "web": Command(run_web_debug, "Run the engine for web"),
        "test": Command(run_tests, "Run the tests", ArgType.STR_LIST),
    },
    "gen": {
        "all": Command(gen_all, "Generate all CMake and C++ code"),
        "sources": Command(gen_sources, "Generate CMake sources"),
        "code": Command(gen_code, "Generate C++ code"),
        "cmake": Command(gen_cmake, "Generate CMake files and sources"),
    },
    "import": Command(import_assets, "Import assets"),
}


def get_command(
    commands: list[str], index: int = 0, cmdTree: dict = COMMANDS
) -> tuple[Command, int] | None:
    command_str = commands[index]
    command = cmdTree.get(command_str)
    if len(commands) - 1 == index:
        if isinstance(command, dict):
            print("Usage:")
            print_help(command)
            exit(0)
        elif isinstance(command, Command):
            return (command, index + 1)

    if command is None:
        return None
    elif isinstance(command, Command):
        return (command, index + 1)
    elif isinstance(command, dict):
        return get_command(commands, index + 1, command)
    return None


def handle_command(command: Command, app: App, args: list[str]):
    try:
        command.run(app=app, args=args)
    except KeyboardInterrupt:
        pass
    except SubprocessError:
        pass


def require_cpp_package():
    try:
        importlib.import_module("pretop._native")
    except (ImportError, OSError) as error:
        print("The Pretop C++ Python package is not set up.")
        print("Run `uv run cli.py setup` and retry.")
        print(f"Import error: {error}")
        raise SystemExit(1)


def validate_command(command: Command, cmd_name: list[str], args: list[str]):
    is_valid_none = len(args) == 0
    if command.arg_type == ArgType.NONE and not is_valid_none:
        print(f"{' '.join(cmd_name)}: Zero arguments required, {len(args)} provided")
        exit(0)


def cli(app: App):
    cmd: list[str] = sys.argv[1:]
    if len(cmd) == 0:
        help()
        exit(0)
    result = get_command(cmd)
    cmd_str = " ".join(cmd)
    if result is None:
        print(f"unknown command: {cmd_str}")
        exit(0)
    command, cmd_split_index = result
    cmd_name = cmd[:cmd_split_index]
    cmd_args = cmd[cmd_split_index:]
    print(cmd_str)
    validate_command(command, cmd_name, cmd_args)
    if command.requires_cpp_package:
        require_cpp_package()
        from config.importer import import_assets as load_project_imports

        load_project_imports(app)
    handle_command(command, app, cmd_args)
