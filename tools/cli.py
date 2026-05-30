from subprocess import SubprocessError
from enum import Enum
import sys

from codegen.gen_sources import gen_sources
from build_scripts.dawn import dawn_debug_setup
from build_scripts.native import build_native_debug, run_native_debug, run_tests
from build_scripts.web import build_web_debug, run_web_debug


class ArgType(Enum):
    NONE = 0
    STR_LIST = 1


class Command:
    def __init__(self, fn, description: str, arg_type: ArgType = ArgType.NONE):
        self.fn = fn
        self.description = description
        self.arg_type = arg_type

    def run(self, args: list[str]):
        if self.arg_type == ArgType.NONE:
            self.fn()
        else:
            self.fn(args)


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
        "test": Command(run_tests, "Run the tests", ArgType.STR_LIST),
    },
    "gen": {
        "sources": Command(gen_sources, "Generate CMake sources"),
    },
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


def handle_command(command: Command, args: list[str]):
    try:
        command.run(args)
    except KeyboardInterrupt:
        pass
    except SubprocessError:
        pass


def validate_command(command: Command, cmd_name: list[str], args: list[str]):
    is_valid_none = len(args) == 0
    if command.arg_type == ArgType.NONE and not is_valid_none:
        print(f"{' '.join(cmd_name)}: Zero arguments required, {len(args)} provided")
        exit(0)


cmd: list[str] = sys.argv[1:]
if len(cmd) == 0:
    help()
    exit(0)
result = get_command(cmd)
cmd_str = " ".join(cmd)
if result is None:
    print(f"unknown command: {cmd_str}")
    exit(0)
(command, cmd_split_index) = result
cmd_name = cmd[:cmd_split_index]
cmd_args = cmd[cmd_split_index:]
print(cmd_str)
validate_command(command, cmd_name, cmd_args)
handle_command(command, cmd_args)
