#!/usr/bin/env python3
"""
embed.py — Convert web/index.html into the HTML[] C string in main/webserver.cpp

Usage:
    python web/embed.py

What it does:
  1. Reads web/index.html
  2. Strips everything between <!-- DEV_ONLY_START --> and <!-- DEV_ONLY_END -->
  3. Minifies whitespace
  4. Converts to a C string array of adjacent string literals
  5. Replaces the HTML[] block in main/webserver.cpp in place
"""

import re
import sys
from pathlib import Path

ROOT        = Path(__file__).parent.parent
HTML_FILE   = ROOT / "web" / "index.html"
CPP_FILE    = ROOT / "main" / "webserver.cpp"

# ── 1. Read HTML ──────────────────────────────────────────────────────────────
html = HTML_FILE.read_text(encoding="utf-8")

# ── 2. Strip dev-only sections ────────────────────────────────────────────────
html = re.sub(
    r"<!-- DEV_ONLY_START -->.*?<!-- DEV_ONLY_END -->",
    "",
    html,
    flags=re.DOTALL,
)

# ── 3. Minify ─────────────────────────────────────────────────────────────────
# Collapse whitespace between tags, remove leading/trailing whitespace per line
lines = [line.strip() for line in html.splitlines()]
html = "".join(lines)
# Collapse multiple spaces inside tags
html = re.sub(r"  +", " ", html)

# ── 4. Build C string literal ─────────────────────────────────────────────────
def to_c_string(s: str) -> str:
    """Split into ~100-char chunks as adjacent C string literals."""
    # Escape backslashes and double-quotes
    s = s.replace("\\", "\\\\").replace('"', '\\"')
    chunks = []
    while s:
        chunks.append(s[:100])
        s = s[100:]
    lines = [f'"{chunk}"' for chunk in chunks]
    return "\n".join(lines)

c_str = to_c_string(html)
new_block = f"static const char HTML[] =\n{c_str};\n"

# ── 5. Replace HTML[] block in webserver.cpp ──────────────────────────────────
cpp = CPP_FILE.read_text(encoding="utf-8")

pattern = re.compile(
    r"// ── HTML page ─+\n"
    r"static const char HTML\[\] =\n"
    r'(?:"[^\n]*"\n)+'
    r";\n",
    re.MULTILINE,
)

if not pattern.search(cpp):
    print("ERROR: Could not find HTML[] block in webserver.cpp", file=sys.stderr)
    print("Make sure the block starts with: // ── HTML page", file=sys.stderr)
    sys.exit(1)

new_cpp = pattern.sub(
    "// ── HTML page " + "─" * 45 + "\n" + new_block,
    cpp,
)

CPP_FILE.write_text(new_cpp, encoding="utf-8")
print(f"✓ Embedded {len(html)} chars of HTML into {CPP_FILE.relative_to(ROOT)}")
