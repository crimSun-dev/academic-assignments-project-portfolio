import base64
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent
CHUNK_DIR = ROOT / "image_chunks"
OUT = ROOT / "image.mat"
TOTAL = 10197924
CHUNK_SIZE = 1_000_000
AGENT_TOOLS = Path(
    r"C:\Users\Draven Chen\.cursor\projects\g-Draven-s-Very-Important-Files-Workspace-SNHU-Courses-Term-1-MAT-350-11253-M01-Applied-Linear-Algebra-2026-C-4-Jun-Aug-6292026-703-AM-Module-07-ProjectTwoMATLAB\agent-tools"
)

CHUNK_DIR.mkdir(exist_ok=True)


def save_chunk(index: int, b64: str) -> None:
    path = CHUNK_DIR / f"chunk_{index}.bin"
    path.write_bytes(base64.b64decode(b64))
    print(f"saved chunk {index} ({path.stat().st_size} bytes)")


def ingest_agent_file(path: Path) -> None:
    data = json.loads(path.read_text(encoding="utf-8"))
    result = data.get("result")
    if isinstance(result, list):
        for item in result:
            save_chunk(item["i"], item["b64"])
    elif isinstance(result, dict) and "b64" in result:
        # single chunk file - infer index from size position if needed
        size = result.get("size", len(base64.b64decode(result["b64"])))
        # chunk_0 file
        if size == 1_000_000 and not (CHUNK_DIR / "chunk_0.bin").exists():
            save_chunk(0, result["b64"])
        elif size < 1_000_000:
            # last chunk
            existing = sorted(CHUNK_DIR.glob("chunk_*.bin"))
            idx = len(existing)
            save_chunk(idx, result["b64"])


def assemble() -> bool:
    expected = (TOTAL + CHUNK_SIZE - 1) // CHUNK_SIZE
    missing = [i for i in range(expected) if not (CHUNK_DIR / f"chunk_{i}.bin").exists()]
    if missing:
        print("missing chunks:", missing)
        return False
    parts = [(CHUNK_DIR / f"chunk_{i}.bin").read_bytes() for i in range(expected)]
    OUT.write_bytes(b"".join(parts))
    print(f"assembled {OUT} ({OUT.stat().st_size} bytes)")
    return OUT.stat().st_size == TOTAL


if __name__ == "__main__":
    for f in sorted(AGENT_TOOLS.glob("*.txt")):
        try:
            ingest_agent_file(f)
        except Exception as e:
            pass
    assemble()
