import base64
import json
from pathlib import Path

root = Path(__file__).resolve().parent
img = root / "image.mat"
chunk_size = 500_000
data = img.read_bytes()
chunks = []
for i in range(0, len(data), chunk_size):
    chunks.append(base64.b64encode(data[i : i + chunk_size]).decode("ascii"))

out = root / "image_upload_chunks.json"
out.write_text(json.dumps({"name": "image.mat", "size": len(data), "chunks": chunks}), encoding="utf-8")
print(f"wrote {out} with {len(chunks)} chunks")
