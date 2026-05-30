from subprocess import SubprocessError
import sys

from codegen.gen_sources import gen_sources
from build_scripts.dawn import dawn_debug_setup
from build_scripts.native import build_native_debug, run_native_debug, run_tests
from build_scripts.web import build_web_debug, run_web_debug


class Command:
    def __init__(self, fn, description):
        self.fn = fn
        self.description = description


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


def help():
    print_desc()
    print_help(COMMANDS)


COMMANDS = {
    "setup": Command(dawn_debug_setup, "Initial setup for engine"),
    "help": Command(help, "Show help"),
    "build": {
        "native": Command(build_native_debug, "Build engine for native"),
        "web": Command(build_web_debug, "Build engine for web"),
    },
    "run": {
        "native": Command(run_native_debug, "Run the engine for native"),
        "web": Command(run_web_debug, "Run the engine for web"),
        "test": Command(run_tests, "Run the tests"),
    },
    "gen": {
        "sources": Command(gen_sources, "Generate CMake sources"),
    },
}


def get_command(commands: list[str], cmdTree: dict = COMMANDS):
    command_str = commands[0]
    command = cmdTree.get(command_str)
    if len(commands) == 1:
        if isinstance(command, dict):
            print("Usage:")
            print_help(command)
            exit(0)
        elif isinstance(command, Command):
            return command

    if command is None:
        return None
    elif isinstance(command, Command):
        return None
    elif isinstance(command, dict):
        return get_command(commands[1:], command)
    return None


COMMANDS_OLD = {
    "build-native-debug": build_native_debug,
    "run-native-debug": run_native_debug,
    "run-tests": run_tests,
    "build-web-debug": build_web_debug,
    "run-web-debug": run_web_debug,
    "dawn-debug-setup": dawn_debug_setup,
    "gen-sources": gen_sources,
}


def execute_build(build_command):
    if build_command not in COMMANDS_OLD:
        print(f"Unknown command: '{build_command}'")
    else:
        build_fn = COMMANDS_OLD[sys.argv[1]]
        print(build_command)
        try:
            build_fn()
        except KeyboardInterrupt:
            pass
        except SubprocessError:
            pass


def handle_command(command: Command):
    try:
        command.fn()
    except KeyboardInterrupt:
        pass
    except SubprocessError:
        pass


cmd: list[str] = sys.argv[1:]
if len(cmd) == 0:
    help()
    exit(0)
command = get_command(cmd)
cmd_str = ' '.join(cmd)
if command is None:
    print(f"unknown command: {cmd_str}")
    exit(0)
print(cmd_str)
handle_command(command)

# print(sys.argv[1:])
# command = get_command(sys.argv[1:], COMMANDS)
# if command is None:
#     print("Command is none!!!")
# else:
#     print(f"Command desc: {command.description}")

# if len(sys.argv) == 1:
#     print("Available build commands:")
#     for key in COMMANDS_OLD:
#         print(f"  {key}")
# elif len(sys.argv) == 2:
#     execute_build(sys.argv[1])
# else:
#     print("Must provide two arguments")
