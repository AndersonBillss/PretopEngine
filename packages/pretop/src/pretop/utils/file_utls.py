from pathlib import Path


def save_if_changed(out_file: Path | str, file_contents: str):
    existing_file_contents = ""
    out_file = Path(out_file)
    if out_file.is_file():
        with open(out_file, "r", encoding="utf-8") as file:
            existing_file_contents = file.read()
        
        if existing_file_contents == file_contents:
            return

    with open(out_file, "w") as output:
        output.write(file_contents)
