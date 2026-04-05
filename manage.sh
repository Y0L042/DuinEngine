#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GENERATE_XXD="$SCRIPT_DIR/Duin/src/Duin/Script/generate_xxd.py"
DASLANG="$SCRIPT_DIR/Duin/vendor/daslang/bin/RelWithDebInfo/daslang_static.exe"
GEN_ADAPTER_DAS="$SCRIPT_DIR/Duin/src/Duin/Script/gen_adapter.das"
GEN_BIND_DAS="$SCRIPT_DIR/Duin/src/Duin/Script/gen_bind.das"
SCRIPT_INC="$SCRIPT_DIR/Duin/src/Duin/Script"
DAS_FORMATTER="$SCRIPT_DIR/Duin/vendor/daslang/utils/dasCodeFormatter/main.das"
DASROOT="$SCRIPT_DIR/Duin/vendor/daslang"

usage() {
    echo "Usage: $0 <command> [args]"
    echo ""
    echo "Commands:"
    echo "  gen-inc         Regenerate all .das.inc files from their .das sources"
    echo "  gen-adapter     Regenerate DnGameObjectAdapter_gen.inc via cpp_bind"
    echo "  fmt <file.das>  Format a .das file in-place (backs up, formats, verifies)"
    echo "  fmt-all         Format all .das files under Duin/src/Duin/Script"
    echo "  codegen         Run the full pipeline: fmt-all, gen-inc, gen-adapter"
    echo "  help            Show this help message"
}

cmd_gen_inc() {
    local script_dir="$SCRIPT_DIR/Duin/src/Duin/Script"
    local count=0

    while IFS= read -r -d '' das_file; do
        local inc_file="${das_file}.inc"
        echo "  ${das_file##*/} -> ${inc_file##*/}"
        python "$GENERATE_XXD" "$das_file" "$inc_file"
        (( count++ )) || true
    done < <(find "$script_dir" -name "*.das" -print0)

    echo "Generated $count file(s)."
}

cmd_gen_adapter() {
    local out="$SCRIPT_DIR/Duin/src/Duin/Script/DnGameObjectAdapter_gen.inc"
    local dasroot="$SCRIPT_DIR/Duin/vendor/daslang"
    local project="$SCRIPT_INC/gen_bind.das_project"
    echo "Running gen_bind.das via daslang..."
    "$DASLANG" -dasroot "$dasroot" -no-dynamic-modules -project "$project" "$GEN_BIND_DAS" -- "$out"
}

cmd_fmt() {
    local das_file="${1:-}"
    if [[ -z "$das_file" ]]; then
        echo "Usage: $0 fmt <file.das>"
        exit 1
    fi
    local bak="${das_file}.bak"
    echo "  Formatting ${das_file##*/}..."
    cp "$das_file" "$bak"
    if "$DASLANG" -dasroot "$DASROOT" "$DAS_FORMATTER" -- "$das_file"; then
        rm -f "$bak"
        echo "  Done."
    else
        echo "  Formatter failed — restoring backup."
        cp "$bak" "$das_file"
        rm -f "$bak"
        exit 1
    fi
}

cmd_fmt_all() {
    local script_dir="$SCRIPT_DIR/Duin/src/Duin/Script"
    local count=0

    while IFS= read -r -d '' das_file; do
        cmd_fmt "$das_file"
        (( count++ )) || true
    done < <(find "$script_dir" -name "*.das" -print0)

    echo "Formatted $count file(s)."
}

cmd_codegen() {
    echo "==> fmt-all"
    cmd_fmt_all
    echo "==> gen-inc"
    cmd_gen_inc
    echo "==> gen-adapter"
    cmd_gen_adapter
}

case "${1:-}" in
    gen-inc)      cmd_gen_inc ;;
    gen-adapter)  cmd_gen_adapter ;;
    fmt)          cmd_fmt "${2:-}" ;;
    fmt-all)      cmd_fmt_all ;;
    codegen)      cmd_codegen ;;
    help|"")      usage ;;
    *)            echo "Unknown command: $1"; usage; exit 1 ;;
esac
