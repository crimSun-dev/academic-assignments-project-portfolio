import zipfile
from pathlib import Path

p = Path(__file__).resolve().parent / "Project_Two_Template.mlx"
with zipfile.ZipFile(p) as z:
    print(z.namelist())
    for i in z.infolist():
        print(i.filename, i.file_size)
