#!/usr/bin/env bash
# Fails if tracked source/config files contain machine-specific absolute paths
# (e.g. "D:/Projects/...", "C:\\Users\\...", "/c/Projects/...").
# Resolve paths at runtime instead: duin::fs::ResolveDasRoot / FindProjectFile,
# or virtual paths (bin://, eng://, wrk://, ...).
#
# Usage: tools/check_abs_paths.sh        (from anywhere inside the repo)
# Exit:  0 = clean, 1 = offenders found

cd "$(git rev-parse --show-toplevel)" || exit 2

# Drive-letter paths (C:/ or C:\) and MSYS-style /c/ paths
pattern='(^|[^A-Za-z0-9_])[A-Za-z]:(\\\\|\\|/)[A-Za-z_][A-Za-z0-9_]|(^|["'"'"' =])/[a-z]/(Projects|Users|Program)'

hits=$(git ls-files -- \
        '*.cpp' '*.h' '*.hpp' '*.c' '*.das' '*.das_project' '*.lua' '*.toml' '*.json' '*.sh' \
        ':!:**/vendor/**' ':!:**/external/**' ':!:**/extern/**' \
        ':!:DuinTests/src/IO/**' ':!:Duin/src/Duin/IO/Filesystem.h' \
        ':!:premakeCfg.lua' ':!:tools/check_abs_paths.sh' \
        ':!:**/*cache*' ':!:**/*Cache*' ':!:**/.jitted_scripts/**' \
    | xargs -d '\n' grep -nE "$pattern" 2>/dev/null)

if [ -n "$hits" ]; then
    echo "Absolute paths found (make them relative / runtime-resolved):"
    echo "$hits"
    exit 1
fi

echo "No absolute paths found."
exit 0
