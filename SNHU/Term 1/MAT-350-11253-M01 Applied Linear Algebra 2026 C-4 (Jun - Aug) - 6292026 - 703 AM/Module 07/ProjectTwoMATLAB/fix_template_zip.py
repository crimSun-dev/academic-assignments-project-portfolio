from pathlib import Path

p = Path(__file__).resolve().parent / "Project_Two_Template.mlx"
data = bytearray(p.read_bytes())
bad = b"matlab/outpux.xml"
good = b"matlab/output.xml"
count = data.count(bad)
print("replacements", count)
data = data.replace(bad, good)
fixed = p.with_name("Project_Two_Template_fixed.mlx")
fixed.write_bytes(data)
print("wrote", fixed)
