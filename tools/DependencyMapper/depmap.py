import argparse
import json
import sys
import time
import webbrowser
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from depmap.graph import build_graph, to_json
from depmap.render import render
from depmap.vcxproj import load_project

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent


def main() -> int:
    ap = argparse.ArgumentParser(
        description="Map header inclusion trees for each translation unit of a .vcxproj project.",
        epilog="The Graph tab loads Cytoscape.js from a CDN and needs internet access; the other tabs work offline.",
    )
    ap.add_argument("--project", type=Path, default=ROOT / "Duin" / "Duin.vcxproj")
    ap.add_argument("--config", default="Debug|x64")
    ap.add_argument("--source-root", type=Path, default=None, help="headers under this dir are expanded (default: <project dir>/src)")
    ap.add_argument("--expand-external", action="store_true", help="also follow includes inside vendor headers")
    ap.add_argument("--out", type=Path, default=HERE / "out" / "depmap.html")
    ap.add_argument("--json", action="store_true", help="also write the raw graph next to the HTML")
    ap.add_argument("--open", action="store_true", help="open the result in the default browser")
    args = ap.parse_args()

    start = time.perf_counter()
    project = load_project(args.project, args.config, args.source_root)
    graph = build_graph(project, ROOT, args.expand_external)
    data = to_json(graph)
    render(data, args.out)
    if args.json:
        args.out.with_suffix(".json").write_text(json.dumps(data, indent=1), encoding="utf-8")

    kinds: dict[str, int] = {}
    for n in graph.nodes.values():
        if not n.tu:
            kinds[n.kind] = kinds.get(n.kind, 0) + 1
    summary = ", ".join(f"{v} {k}" for k, v in sorted(kinds.items()))
    print(f"{project.name} [{args.config}]: {len(graph.units)} TUs, {summary}, "
          f"{len(graph.cycles)} cycles ({time.perf_counter() - start:.2f}s)")
    print(f"wrote {args.out}")
    if args.open:
        webbrowser.open(args.out.resolve().as_uri())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
