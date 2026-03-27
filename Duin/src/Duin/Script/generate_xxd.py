#!/usr/bin/env python3
"""Convert a .das file to a C hex byte array (.das.inc) for compileBuiltinModule().

Usage: python generate_xxd.py <input.das> <output.das.inc>

Produces output matching daslang's CMAKE_TEXT_XXD format:
    static unsigned char <varname>[] = { 0xNN, ... };
"""

import sys
import os


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input.das> <output.das.inc>")
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]

    with open(input_path, "rb") as f:
        data = f.read()

    # Derive variable name: replace dots and hyphens with underscores
    var_name = os.path.basename(input_path).replace(".", "_").replace("-", "_")

    with open(output_path, "w", newline="\n") as f:
        f.write(f"static unsigned char {var_name}[] = {{\n")
        for i, byte in enumerate(data):
            if i % 8 == 0:
                f.write("")
            f.write(f"0x{byte:02x},")
            if i % 8 == 7 or i == len(data) - 1:
                f.write("\n")
        f.write("};\n")


if __name__ == "__main__":
    main()
