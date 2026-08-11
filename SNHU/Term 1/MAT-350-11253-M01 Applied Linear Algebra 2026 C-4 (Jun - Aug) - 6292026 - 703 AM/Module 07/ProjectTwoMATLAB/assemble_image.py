import base64
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
CHUNK_DIR = ROOT / "image_chunks"
OUT = ROOT / "image.mat"
TOTAL = 10197924
CHUNK_SIZE = 1_000_000

CHUNK_DIR.mkdir(exist_ok=True)

# Save chunk 0 from agent-tools export
src0 = Path(
    r"C:\Users\Draven Chen\.cursor\projects\g-Draven-s-Very-Important-Files-Workspace-SNHU-Courses-Term-1-MAT-350-11253-M01-Applied-Linear-Algebra-2026-C-4-Jun-Aug-6292026-703-AM-Module-07-ProjectTwoMATLAB\agent-tools\b4599b1c-f758-43ea-9cd1-03d01cecf1c0.txt"
)
if src0.exists() and not (CHUNK_DIR / "chunk_0.bin").exists():
    data = json.loads(src0.read_text(encoding="utf-8"))
    (CHUNK_DIR / "chunk_0.bin").write_bytes(base64.b64decode(data["result"]["b64"]))
    print("saved chunk 0")


def save_chunk(index: int, b64: str) -> None:
    path = CHUNK_DIR / f"chunk_{index}.bin"
    path.write_bytes(base64.b64decode(b64))
    print(f"saved chunk {index} ({path.stat().st_size} bytes)")


def assemble() -> None:
    parts = []
    for i in range((TOTAL + CHUNK_SIZE - 1) // CHUNK_SIZE):
        path = CHUNK_DIR / f"chunk_{i}.bin"
        if not path.exists():
            print(f"missing {path}")
            return
        parts.append(path.read_bytes())
    OUT.write_bytes(b"".join(parts))
    print(f"assembled {OUT} ({OUT.stat().st_size} bytes)")


if __name__ == "__main__":
    assemble()
