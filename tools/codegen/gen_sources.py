# Generate sources for CMake
import os

SRC_DIR = "src"
OUT_FILE = "sources.cmake"
STARTING_COMMENT = "# Auto-generated code, do not edit"
MAIN_RULE_PREFIX = "PRETOP_ENGINE_MAIN"
RULE_PREFIX = "PRETOP_ENGINE_SOURCES"
TEST_RULE_PREFIX = "PRETOP_ENGINE_TEST_SOURCES"


class SourceTracker:
    def __init__(self):
        self.src_files = []
        self.src_targets = {}

    def add_file(self, filepath):
        has_underscore = filepath.find("_") != -1
        if has_underscore:
            # Grab the file/directory that has an underscore. Whatever is before the underscore will be the rule
            split_filepath_underscore = filepath.split("_")
            until_underscore = "_".join(split_filepath_underscore[:-1])
            split_slash = until_underscore.split("/")
            target_name = split_slash[-1]
            self._add_target_file(target_name, filepath)

        else:
            self._add_src_file(filepath)

    def _add_src_file(self, filepath):
        self.src_files += [filepath]

    def _add_target_file(self, target, filepath):
        if target in self.src_targets:
            self.src_targets[target] += [filepath]
        else:
            self.src_targets[target] = [filepath]

def save_rules(rules: list[str]):
    file_content = STARTING_COMMENT + "\n"
    for rule in rules:
        file_content += f"{rule}\n"

    with open(OUT_FILE, "w") as output:
        output.write(file_content)

def generate_rule(sources: list[str], label) -> str:
    sources.sort()
    concat_files = ""
    for i, filepath in enumerate(sources):
        if i != 0:
            concat_files += "\n"
        concat_files += f"  {filepath}"
    result = f"set({label}\n{concat_files}\n)"
    return result

def get_tests():
    test_files = []
    for root, dirs, files in os.walk("tests"):
        for file in files:
            filepath = os.path.join(root, file).replace("\\", "/")
            if file.endswith(".cpp"):
                test_files += [filepath]
    
    return test_files

def gen_sources():
    tracker = SourceTracker()
    for root, dirs, files in os.walk("src"):
        for file in files:
            filepath = os.path.join(root, file).replace("\\", "/")
            if file.endswith(".cpp") and file != "main.cpp":
                tracker.add_file(filepath)

    target_files_rules = [
        generate_rule(get_tests(), TEST_RULE_PREFIX),
        generate_rule(["src/main.cpp"], MAIN_RULE_PREFIX),
        generate_rule(tracker.src_files, RULE_PREFIX),
    ]
    for key in tracker.src_targets:
        target_files_rules += [generate_rule(tracker.src_targets[key], f"{RULE_PREFIX}_" + key)]
        
    target_files_rules.sort()
    save_rules(target_files_rules)
