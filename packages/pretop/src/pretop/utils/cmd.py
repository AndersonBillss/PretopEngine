from subprocess import run

def cmd(args, *, working_directory=None):
    print(">>", " ".join(args))
    return run(args, check=True, cwd=working_directory)
