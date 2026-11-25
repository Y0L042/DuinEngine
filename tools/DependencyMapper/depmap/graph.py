from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path

from .scanner import Resolver, extract_includes, read_text
from .vcxproj import Project


@dataclass
class Node:
    id: str
    kind: str
    lines: int = 0
    tu: bool = False
    pch: bool = False
    expanded: bool = False


@dataclass
class Graph:
    project: Project
    root: Path
    nodes: dict[str, Node] = field(default_factory=dict)
    edges: dict[str, list[tuple[str, int, str]]] = field(default_factory=dict)
    units: list[str] = field(default_factory=list)
    closure: dict[str, set[str]] = field(default_factory=dict)
    cycles: list[list[str]] = field(default_factory=list)


def _node_id(path: Path, root: Path) -> str:
    try:
        return path.relative_to(root).as_posix()
    except ValueError:
        return path.as_posix()


def _count_lines(text: str) -> int:
    if not text:
        return 0
    return text.count("\n") + (0 if text.endswith("\n") else 1)


def build_graph(project: Project, root: Path, expand_external: bool = False) -> Graph:
    resolver = Resolver(project.include_dirs, project.source_root)
    g = Graph(project, root)
    paths: dict[str, Path] = {}

    def add(path: Path | None, kind: str, spelling: str) -> str:
        nid = _node_id(path, root) if path else f"<{spelling}>"
        if nid not in g.nodes:
            g.nodes[nid] = Node(nid, kind)
            if path:
                paths[nid] = path
        return nid

    stack = []
    for tu in project.units:
        nid = add(tu.path, "project", tu.path.name)
        node = g.nodes[nid]
        node.tu = True
        node.pch = tu.uses_pch
        g.units.append(nid)
        stack.append(nid)

    while stack:
        nid = stack.pop()
        node = g.nodes[nid]
        if node.expanded:
            continue
        node.expanded = True
        path = paths[nid]
        text = read_text(path)
        node.lines = _count_lines(text)
        out = []
        for inc in extract_includes(text):
            r = resolver.resolve(inc, path)
            cid = add(r.path, r.kind, r.spelling)
            out.append((cid, r.line, r.spelling))
            child = g.nodes[cid]
            if child.expanded or r.path is None:
                continue
            if r.kind == "project" or expand_external:
                stack.append(cid)
            elif not child.lines:
                child.lines = _count_lines(read_text(r.path))
        g.edges[nid] = out

    for tu in g.units:
        seen: set[str] = set()
        work = [tu]
        while work:
            cur = work.pop()
            for cid, _, _ in g.edges.get(cur, ()):
                if cid not in seen:
                    seen.add(cid)
                    work.append(cid)
        seen.discard(tu)
        g.closure[tu] = seen

    g.cycles = _find_cycles(g)
    return g


def _find_cycles(g: Graph) -> list[list[str]]:
    index: dict[str, int] = {}
    low: dict[str, int] = {}
    on_stack: set[str] = set()
    stack: list[str] = []
    result: list[list[str]] = []
    counter = 0

    def succ(n: str) -> list[str]:
        return [c for c, _, _ in g.edges.get(n, ()) if g.nodes[c].kind == "project"]

    for start in g.edges:
        if start in index:
            continue
        index[start] = low[start] = counter
        counter += 1
        stack.append(start)
        on_stack.add(start)
        work = [(start, iter(succ(start)))]
        while work:
            v, it = work[-1]
            advanced = False
            for w in it:
                if w not in index:
                    index[w] = low[w] = counter
                    counter += 1
                    stack.append(w)
                    on_stack.add(w)
                    work.append((w, iter(succ(w))))
                    advanced = True
                    break
                if w in on_stack:
                    low[v] = min(low[v], index[w])
            if advanced:
                continue
            work.pop()
            if work:
                parent = work[-1][0]
                low[parent] = min(low[parent], low[v])
            if low[v] == index[v]:
                comp = []
                while True:
                    w = stack.pop()
                    on_stack.discard(w)
                    comp.append(w)
                    if w == v:
                        break
                if len(comp) > 1 or v in succ(v):
                    result.append(sorted(comp))
    return result


def to_json(g: Graph) -> dict:
    ids = list(g.nodes)
    idx = {nid: i for i, nid in enumerate(ids)}
    reverse: dict[str, list[int]] = defaultdict(list)
    for t, members in g.closure.items():
        for m in members:
            reverse[m].append(idx[t])
    nodes = []
    for nid in ids:
        n = g.nodes[nid]
        nodes.append({
            "id": nid,
            "k": n.kind,
            "l": n.lines,
            "tu": n.tu,
            "pch": n.pch,
            "x": n.expanded,
            "r": reverse.get(nid, []),
        })
    edges = {idx[k]: [[idx[c], line, sp] for c, line, sp in v] for k, v in g.edges.items()}
    units = []
    for t in g.units:
        members = g.closure[t]
        units.append({
            "n": idx[t],
            "c": len(members),
            "l": sum(g.nodes[m].lines for m in members) + g.nodes[t].lines,
        })
    return {
        "project": g.project.name,
        "config": g.project.config,
        "pch": g.project.pch_header,
        "nodes": nodes,
        "edges": edges,
        "units": units,
        "cycles": [[idx[c] for c in cyc] for cyc in g.cycles],
    }
