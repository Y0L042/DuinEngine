import re
import xml.etree.ElementTree as ET
from dataclasses import dataclass, field
from pathlib import Path

_NS = {"m": "http://schemas.microsoft.com/developer/msbuild/2003"}
_MACRO = re.compile(r"[$%]\([^)]*\)")


@dataclass
class IncludeDir:
    path: Path
    kind: str


@dataclass
class TranslationUnit:
    path: Path
    uses_pch: bool


@dataclass
class Project:
    name: str
    vcxproj: Path
    config: str
    source_root: Path
    units: list[TranslationUnit] = field(default_factory=list)
    headers: list[Path] = field(default_factory=list)
    include_dirs: list[IncludeDir] = field(default_factory=list)
    pch_header: str | None = None


def _matches(elem: ET.Element, config: str) -> bool:
    cond = elem.get("Condition")
    return cond is None or f"=='{config}'" in cond


def _split_paths(value: str, base: Path) -> list[Path]:
    out = []
    for part in value.split(";"):
        part = _MACRO.sub("", part).strip()
        if not part:
            continue
        out.append((base / part.replace("\\", "/")).resolve())
    return out


def _text(elem: ET.Element, tag: str, config: str) -> str | None:
    for child in elem.findall(f"m:{tag}", _NS):
        if _matches(child, config):
            return (child.text or "").strip()
    return None


def is_under(path: Path, root: Path) -> bool:
    return path.is_relative_to(root)


def load_project(vcxproj: Path, config: str = "Debug|x64", source_root: Path | None = None) -> Project:
    vcxproj = vcxproj.resolve()
    base = vcxproj.parent
    root = ET.parse(vcxproj).getroot()
    project = Project(
        name=vcxproj.stem,
        vcxproj=vcxproj,
        config=config,
        source_root=(source_root or base / "src").resolve(),
    )

    dirs: list[Path] = []
    external: list[Path] = []
    for group in root.findall("m:ItemDefinitionGroup", _NS):
        if not _matches(group, config):
            continue
        cl = group.find("m:ClCompile", _NS)
        if cl is None:
            continue
        value = _text(cl, "AdditionalIncludeDirectories", config)
        if value:
            dirs += _split_paths(value, base)
        pch = _text(cl, "PrecompiledHeaderFile", config)
        if pch:
            project.pch_header = pch
    for group in root.findall("m:PropertyGroup", _NS):
        if not _matches(group, config):
            continue
        value = _text(group, "ExternalIncludePath", config)
        if value:
            external += _split_paths(value, base)

    seen = set()
    for d in dirs + external:
        key = str(d).lower()
        if key in seen:
            continue
        seen.add(key)
        kind = "project" if is_under(d, project.source_root) else "external"
        project.include_dirs.append(IncludeDir(d, kind))

    for group in root.findall("m:ItemGroup", _NS):
        for item in group.findall("m:ClCompile", _NS):
            excluded = _text(item, "ExcludedFromBuild", config)
            if excluded and excluded.lower() == "true":
                continue
            pch = _text(item, "PrecompiledHeader", config)
            path = (base / item.get("Include").replace("\\", "/")).resolve()
            project.units.append(TranslationUnit(path, uses_pch=(pch or "Use") != "NotUsing"))
        for item in group.findall("m:ClInclude", _NS):
            project.headers.append((base / item.get("Include").replace("\\", "/")).resolve())

    return project
