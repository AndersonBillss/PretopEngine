from subprocess import SubprocessError
import sys

from codegen.gen_sources import gen_sources
from build_scripts.dawn import dawn_debug_setup
from build_scripts.native import build_native_debug, run_native_debug, run_tests
from build_scripts.web import build_web_debug, run_web_debug


COMMANDS = {
    "build-native-debug": build_native_debug,
    "run-native-debug": run_native_debug,
    "run-tests": run_tests,
    "build-web-debug": build_web_debug,
    "run-web-debug": run_web_debug,
    "dawn-debug-setup": dawn_debug_setup,
    "gen-sources": gen_sources,
}


def execute_build(build_command):
    if build_command not in COMMANDS:
        print(f"Unknown command: '{build_command}'")
    else:
        build_fn = COMMANDS[sys.argv[1]]
        print(build_command)
        try:
            build_fn()
        except KeyboardInterrupt:
            pass
        except SubprocessError:
            pass


if len(sys.argv) == 1:
    print("Available build commands:")
    for key in COMMANDS:
        print(f"  {key}")
elif len(sys.argv) == 2:
    execute_build(sys.argv[1])
else:
    print("Must provide two arguments")
