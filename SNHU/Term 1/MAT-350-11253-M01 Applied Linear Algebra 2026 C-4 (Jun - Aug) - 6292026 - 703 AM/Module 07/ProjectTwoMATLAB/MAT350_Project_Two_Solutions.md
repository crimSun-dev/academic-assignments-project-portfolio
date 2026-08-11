# MAT 350 Project Two — SVD and Image Compression
## Worked Solutions & MATLAB Code

Matrix used throughout Problems 1–4:

```
A = [1 2 3;
     3 3 4;
     5 6 7];
```

---

## Problem 1 — Rank-1 Approximation

**MATLAB code:**
```matlab
A = [1 2 3; 3 3 4; 5 6 7];
[U, S, V] = svd(A);

A1 = S(1,1) * U(:,1) * V(:,1)';
A1_rounded = round(A1, 4)

RMSE1 = sqrt(mean((A - A1).^2, 'all'))
```

**Result:**

A₁ (rounded to 4 decimal places):
```
A1 = [1.7039  2.0313  2.4935
      2.7243  3.2477  3.9867
      4.9087  5.8517  7.1832]
```

RMSE(A, A₁) = **0.3257**

**Explanation:** A₁ is built from only the largest singular value (σ₁ ≈ 12.5318) and its corresponding left/right singular vectors, so it captures the single dominant "direction" of variation in A. Because so much information is discarded, the reconstruction error is fairly large.

---

## Problem 2 — Rank-2 Approximation

**MATLAB code:**
```matlab
A2 = A1 + S(2,2) * U(:,2) * V(:,2)';
A2_rounded = round(A2, 4)

RMSE2 = sqrt(mean((A - A2).^2, 'all'))
```

**Result:**

A₂ (rounded to 4 decimal places):
```
A2 = [0.9878  2.0324  2.9820
      2.9065  3.2474  3.8624
      5.0561  5.8515  7.0826]
```

RMSE(A, A₂) = **0.1166**

**Which is better?** A₂ is the better approximation — its RMSE (0.1166) is much smaller than A₁'s (0.3257). This makes sense: A₂ adds back the second singular value/vector pair (σ₂ ≈ 0.9122), recovering more of the structure of the original matrix. In general, RMSE decreases monotonically as you add more singular value terms, since each additional term explains more of A's total variance — the rank-3 reconstruction would reproduce A exactly (RMSE = 0), since A is a 3×3 full-rank matrix.

---

## Problem 3 — Dot and Cross Products of Columns of U

**MATLAB code:**
```matlab
u1 = U(:,1);
u2 = U(:,2);
u3 = U(:,3);

d1 = dot(u1, u2)
c  = cross(u1, u2)
d2 = dot(c, u3)
```

**Results:**
- d1 = dot(u1, u2) ≈ **0** (numerically ~9×10⁻¹⁷, i.e., zero within floating-point error)
- c = cross(u1, u2) ≈ **[-0.1114, -0.8520, 0.5115]**
- d2 = dot(c, u3) ≈ **1.0000**

**Do these values make sense?** Yes. U is an orthogonal matrix produced by the SVD, so its columns form an orthonormal basis — every pair of distinct columns is perpendicular, which is exactly why d1 = 0. Because U is also a proper rotation (det(U) = 1, i.e., a right-handed orthonormal basis), the cross product of the first two columns must equal the third column exactly: c = u1 × u2 = u3. That's confirmed two ways here — c numerically matches u3, and d2 = dot(c, u3) = 1, which is only possible if c is a unit vector pointing in exactly the same direction as u3.

---

## Problem 4 — Does U Span ℝ³?

**MATLAB code:**
```matlab
rank(U)
det(U)
```

**Result:** rank(U) = 3, det(U) = 1 (nonzero)

**Explanation:** U is a 3×3 matrix with 3 linearly independent columns (confirmed by rank(U) = 3, and by det(U) ≠ 0). Any set of n linearly independent vectors in ℝⁿ forms a basis for ℝⁿ — so the columns of U don't just span ℝ³, they form an orthonormal basis for it. This isn't a coincidence: SVD always produces U and V as orthogonal matrices, so their columns are guaranteed to be linearly independent and to span the corresponding space.

---

## Problems 5–7 — Image Compression

Image loaded from `image.mat`: variable **A**, size **2583 × 4220** (grayscale, uint8).

**Load and display the image, and compute the SVD:**
```matlab
load('image.mat');       % loads variable A
A = double(A);            % convert to double for SVD
imshow(uint8(A));
title('Original Image');

[m, n] = size(A);
[U, S, V] = svd(A);
```

### Problem 5 — Finding k for a target compression ratio

The compression ratio for a rank-k approximation of an m×n image is:

```
CR = (m*n) / (k*(m + n + 1))
```

Solve for k:

```
k = round( (m*n) / (CR*(m + n + 1)) )
```

For CR ≈ 2, with m = 2583, n = 4220: **k = 801** (this gives an actual CR of 2.000).

```matlab
CR_target = 2;
k5 = round((m*n) / (CR_target*(m+n+1)))     % k5 = 801

Ak5 = U(:,1:k5) * S(1:k5,1:k5) * V(:,1:k5)';
imshow(uint8(Ak5));
title(sprintf('Rank-%d Approximation (CR ~ %d)', k5, CR_target));
```

### Problem 6 — Display and RMSE

```matlab
RMSE_k5 = sqrt(mean((A - Ak5).^2, 'all'))
```

**Result: RMSE = 3.1539** for the rank-801 (CR ≈ 2) approximation. See `rank_801_CR2.png` below — at this compression level the image is visually indistinguishable from the original.

### Problem 7 — Repeat for CR ≈ 10, 25, 75

```matlab
CR_list = [10, 25, 75];
k_list = zeros(size(CR_list));
RMSE_list = zeros(size(CR_list));

for i = 1:length(CR_list)
    CR_target = CR_list(i);
    k = round((m*n) / (CR_target*(m+n+1)));
    k_list(i) = k;

    Ak = U(:,1:k) * S(1:k,1:k) * V(:,1:k)';
    RMSE_list(i) = sqrt(mean((A - Ak).^2, 'all'));

    figure;
    imshow(uint8(Ak));
    title(sprintf('Rank-%d Approximation (CR ~ %d)', k, CR_target));
end

k_list      % [160, 64, 21]
RMSE_list   % [8.2118, 12.3039, 18.2656]
```

**Full results table (all four CR targets):**

| CR target | k | RMSE | Actual CR achieved |
|---|---|---|---|
| ≈2 | 801 | 3.1539 | 2.000 |
| ≈10 | 160 | 8.2118 | 10.013 |
| ≈25 | 64 | 12.3039 | 25.032 |
| ≈75 | 21 | 18.2656 | 76.287 |

**Trend observed:** As CR increases, k (the number of singular values kept) decreases sharply — from 801 down to just 21 — and RMSE climbs steadily from 3.15 to 18.27. Visually, at CR ≈ 2 and CR ≈ 10 the reconstructed image is nearly indistinguishable from the original; by CR ≈ 25 fine texture and sharp edges begin to soften noticeably; by CR ≈ 75 the image is clearly blurred, though the overall subject and large-scale structure are still recognizable. This happens because the largest singular values capture the coarse, high-energy structure of the image (overall shapes, lighting, contrast), while the smaller singular values — the first ones dropped as k shrinks — encode fine detail, texture, and noise.

**Recommendation:** Based on these results, CR ≈ 10 (k = 160) offers the best balance — a 10× reduction in storage with RMSE still under 10 and no visually obvious quality loss. CR ≈ 25 is a reasonable option if more aggressive savings are needed and some softening is acceptable, but CR ≈ 75 sacrifices too much visual fidelity for most practical use cases.

**Generated images** (included alongside this document — insert into your report/Live Script):
- `original.png` — original image
- `rank_801_CR2.png` — CR ≈ 2 approximation
- `rank_160_CR10.png` — CR ≈ 10 approximation
- `rank_64_CR25.png` — CR ≈ 25 approximation
- `rank_21_CR75.png` — CR ≈ 75 approximation

---

### Note on `A1`/`A2` variable naming
Your template uses `A_{1}` and `A_{2}` as the math notation for the rank-1/rank-2 approximations — in MATLAB, just use variable names `A1` and `A2` (MATLAB doesn't allow subscript syntax in variable names).
