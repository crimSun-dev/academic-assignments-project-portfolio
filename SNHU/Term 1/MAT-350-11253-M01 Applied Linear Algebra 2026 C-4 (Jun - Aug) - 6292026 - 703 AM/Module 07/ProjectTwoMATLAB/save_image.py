import base64
import json
from pathlib import Path

src = Path(r"C:\Users\Draven Chen\.cursor\projects\g-Draven-s-Very-Important-Files-Workspace-SNHU-Courses-Term-1-MAT-350-11253-M01-Applied-Linear-Algebra-2026-C-4-Jun-Aug-6292026-703-AM-Module-07-ProjectTwoMATLAB\agent-tools\1ec2739a-325b-475a-8ced-89c7604575c6.txt")
out = Path(__file__).resolve().parent / "image.mat"

data = json.loads(src.read_text(encoding="utf-8"))
result = data["result"]
parts = [base64.b64decode(chunk) for chunk in result["chunks"]]
out.write_bytes(b"".join(parts))
print(f"saved {out} ({out.stat().st_size} bytes, expected {result['size']})")
