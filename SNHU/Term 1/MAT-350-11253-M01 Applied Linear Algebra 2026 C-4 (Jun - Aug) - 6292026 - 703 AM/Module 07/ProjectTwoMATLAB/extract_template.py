import zipfile
from pathlib import Path

p = Path(__file__).resolve().parent / "Project_Two_Template.mlx"
with zipfile.ZipFile(p) as z:
    print(z.namelist())
    doc = z.read("matlab/document.xml").decode("utf-8")
    out = Path(__file__).resolve().parent / "template_document.xml"
    out.write_text(doc, encoding="utf-8")
    print(f"Wrote {out} ({len(doc)} chars)")
