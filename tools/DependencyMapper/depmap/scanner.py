import re
from dataclasses import dataclass
from pathlib import Path

from .vcxproj import IncludeDir, is_under

_INCLUDE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*(?:([<"])([^>"\n]+)[>"]|([A-Za-z_]\w*))', re.MULTILINE)
_COMMENTS = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', re.DOTALL)


@dataclass(frozen=True)
class RawInclude:
    spelling: str
    angled: bool
    line: int
    macro: bool = False


@dataclass(frozen=True)
class Resolved:
    spelling: str
    line: int
    path: Path | None
    kind: str


def _strip_comments(text: str) -> str:
    def repl(m: re.Match) -> str:
        s = m.group(0)
        if s.startswith("/"):
            return re.sub(r"[^\n]", " ", s)
        return s
    return _COMMENTS.sub(repl, text)


def read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def extract_includes(text: str) -> list[RawInclude]:
    clean = _strip_comments(text)
    out = []
    for m in _INCLUDE.finditer(clean):
        line = clean.count("\n", 0, m.start()) + 1
        if m.group(3):
            out.append(RawInclude(m.group(3), False, line, macro=True))
        else:
            out.append(RawInclude(m.group(2).strip(), m.group(1) == "<", line))
    return out


class Resolver:
    def __init__(self, include_dirs: list[IncludeDir], source_root: Path):
        self.include_dirs = include_dirs
        self.source_root = source_root
        self._exists: dict[str, Path | None] = {}

    def _probe(self, candidate: Path) -> Path | None:
        key = str(candidate).lower()
        if key not in self._exists:
            self._exists[key] = candidate.resolve() if candidate.is_file() else None
        return self._exists[key]

    def classify(self, path: Path) -> str:
        return "project" if is_under(path, self.source_root) else "external"

    def resolve(self, inc: RawInclude, includer: Path) -> Resolved:
        if inc.macro:
            return Resolved(inc.spelling, inc.line, None, "unresolved")
        rel = inc.spelling.replace("\\", "/")
        if not inc.angled:
            hit = self._probe(includer.parent / rel)
            if hit:
                return Resolved(inc.spelling, inc.line, hit, self.classify(hit))
        for d in self.include_dirs:
            hit = self._probe(d.path / rel)
            if hit:
                return Resolved(inc.spelling, inc.line, hit, self.classify(hit))
        return Resolved(inc.spelling, inc.line, None, "unresolved")
