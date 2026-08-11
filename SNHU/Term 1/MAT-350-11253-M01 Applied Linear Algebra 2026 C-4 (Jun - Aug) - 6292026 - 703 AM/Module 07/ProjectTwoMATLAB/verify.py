import numpy as np
from scipy.io import loadmat
from pathlib import Path

ROOT = Path(__file__).resolve().parent

# Problems 1-4
A = np.array([[1, 2, 3], [3, 3, 4], [5, 6, 7]], dtype=float)
U, s, Vt = np.linalg.svd(A, full_matrices=True)
S = np.diag(s)
V = Vt.T
A1 = s[0] * np.outer(U[:, 0], V[:, 0])
A2 = A1 + s[1] * np.outer(U[:, 1], V[:, 1])
RMSE1 = np.sqrt(np.mean((A - A1) ** 2))
RMSE2 = np.sqrt(np.mean((A - A2) ** 2))
print("RMSE1", round(RMSE1, 4))
print("RMSE2", round(RMSE2, 4))
print("d1", np.dot(U[:, 0], U[:, 1]))
print("cross", np.cross(U[:, 0], U[:, 1]))
print("d2", np.dot(np.cross(U[:, 0], U[:, 1]), U[:, 2]))
print("rank", np.linalg.matrix_rank(U))
print("det", round(np.linalg.det(U), 4))

# Problems 5-7
mat = loadmat(ROOT / "image.mat")
Aimg = mat["A"].astype(float)
m, n = Aimg.shape
print("image size", m, n)
U, s, Vt = np.linalg.svd(Aimg, full_matrices=False)
V = Vt.T

for cr in [2, 10, 25, 75]:
    k = round((m * n) / (cr * (m + n + 1)))
    Ak = U[:, :k] @ np.diag(s[:k]) @ Vt[:k, :]
    rmse = np.sqrt(np.mean((Aimg - Ak) ** 2))
    print(f"CR={cr} k={k} RMSE={rmse:.4f}")
