#!/usr/bin/env python3
"""Batch-convert .das files to .das.inc hex arrays, with hash-based change detection.

Skips gen_adapter.das and gen_bind.das.
Only regenerates .inc files when the source .das has changed since last run.
Cache stored in <script_dir>/.xxd_cache.json.

Usage: python generate_engine_script_xxd.py <script_dir>
"""

import sys
import os
import json
import hashlib

SKIP_FOLDERS = {"modules"}
SKIP_FILES = {"gen_adapter.das", "gen_bind.das"}
CACHE_FILE = ".xxd_cache.json"


def hash_file(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        h.update(f.read())
    return h.hexdigest()


def das_to_inc(input_path, output_path):
    with open(input_path, "rb") as f:
        data = f.read()

    var_name = os.path.basename(input_path).replace(".", "_").replace("-", "_")

    with open(output_path, "w", newline="\n") as f:
        f.write(f"static unsigned char {var_name}[] = {{\n")
        for i, byte in enumerate(data):
            f.write(f"0x{byte:02x},")
            if i % 8 == 7 or i == len(data) - 1:
                f.write("\n")
        f.write("};\n")


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <script_dir>")
        sys.exit(1)

    script_dir = sys.argv[1]
    cache_path = os.path.join(script_dir, CACHE_FILE)

    try:
        with open(cache_path, "r") as f:
            cache = json.load(f)
    except (FileNotFoundError, json.JSONDecodeError):
        cache = {}

    das_files = []
    for root, dirs, files in os.walk(script_dir):
        dirs[:] = [d for d in dirs if d not in SKIP_FOLDERS]
        for fname in files:
            if fname.endswith(".das"):
                das_files.append(os.path.join(root, fname))

    generated = 0
    skipped = 0
    up_to_date = 0

    for das_file in sorted(das_files):
        fname = os.path.basename(das_file)
        
        if fname in SKIP_FILES:
            skipped += 1
            continue

        inc_file = das_file + ".inc"
        current_hash = hash_file(das_file)
        cached_hash = cache.get(das_file)

        if cached_hash == current_hash and os.path.exists(inc_file):
            up_to_date += 1
            continue

        rel = os.path.relpath(das_file, script_dir)
        print(f"  {rel} -> {os.path.basename(inc_file)}")
        das_to_inc(das_file, inc_file)
        cache[das_file] = current_hash
        generated += 1

    with open(cache_path, "w") as f:
        json.dump(cache, f, indent=2)

    print(f"Generated {generated} file(s), {up_to_date} up-to-date, {skipped} skipped.")


if __name__ == "__main__":
    main()
