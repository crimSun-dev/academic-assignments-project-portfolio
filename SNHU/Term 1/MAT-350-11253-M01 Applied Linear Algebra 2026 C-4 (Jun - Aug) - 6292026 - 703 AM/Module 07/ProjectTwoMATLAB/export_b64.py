import base64
import json
from pathlib import Path

root = Path(__file__).resolve().parent
for name in ["Project_Two_Completed.mlx", "Project_Two_Template_fixed.mlx"]:
    p = root / name
    if p.exists():
        data = base64.b64encode(p.read_bytes()).decode("ascii")
        (root / f"{p.stem}.b64.json").write_text(json.dumps({"name": p.name, "b64": data}), encoding="utf-8")
        print(p.name, len(data))
