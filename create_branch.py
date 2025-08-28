#!/usr/bin/env python3
"""
create_branch.py
Usage:
    python create_branch.py feature program_exit_winui
    python create_branch.py fix crash-handler
    python create_branch.py feature sceneworld_mvp --push
"""

import subprocess
import sys
import re

types = (
            "feature",
            "fix",
            "refactor"
        )

def run_git(args):
    """Run a git command and return stdout as text."""
    result = subprocess.run(["git"] + args,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            text=True,
                            check=True)
    return result.stdout.strip()

def slugify(name: str) -> str:
    """Lowercase, replace spaces with '-', and strip invalid chars."""
    name = name.lower().replace(" ", "-")
    name = re.sub(r"[^a-z0-9._-]+", "-", name)
    name = re.sub(r"-+", "-", name)
    return name.strip("-_")

def main():
    if len(sys.argv) < 3:
        print("Usage: create_branch.py {", types, "} <name> [--push]", file=sys.stderr)
        sys.exit(1)

    branch_type = sys.argv[1]
    raw_name = sys.argv[2]
    pushflag = sys.argv[3] if len(sys.argv) > 3 else None

    if branch_type not in types:
        print("Type must be ", type, file=sys.stderr)
        sys.exit(1)

    slug = slugify(raw_name)

    # list all branches (local + remote)
    refs = run_git(["for-each-ref", "--format=%(refname:short)", "refs/heads", "refs/remotes"]).splitlines()

    # find max number already used for this type
    pattern = re.compile(rf"^(origin/)?{branch_type}/#(\d+)_")
    numbers = []
    for ref in refs:
        m = pattern.match(ref)
        if m:
            numbers.append(int(m.group(2)))
    next_num = max(numbers, default=0) + 1

    branch_name = f"{branch_type}/#{next_num:04d}_{slug}"

    # create branch
    subprocess.run(["git", "checkout", "-b", branch_name], check=True)
    print(f"Created branch: {branch_name}")

    # optionally push
    if pushflag == "--push":
        subprocess.run(["git", "push", "-u", "origin", branch_name], check=True)
        print(f"Pushed branch: {branch_name}")

if __name__ == "__main__":
    main()
