import json
from pathlib import Path

_TEMPLATE = Path(__file__).with_name("template.html")


def render(data: dict, out: Path) -> None:
    payload = json.dumps(data, separators=(",", ":")).replace("</", "<\\/")
    html = _TEMPLATE.read_text(encoding="utf-8").replace("/*__DATA__*/null", payload)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(html, encoding="utf-8")
