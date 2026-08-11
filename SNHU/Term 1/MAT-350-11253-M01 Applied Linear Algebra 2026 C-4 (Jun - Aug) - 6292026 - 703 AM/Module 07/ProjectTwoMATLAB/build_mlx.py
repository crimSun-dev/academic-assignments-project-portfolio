#!/usr/bin/env python3
"""Build completed Project Two live script from template."""

import re
import shutil
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent
TEMPLATE = ROOT / "Project_Two_Template_fixed.mlx"
OUTPUT = ROOT / "Project_Two_Completed.mlx"
CODES = {
    1: """A = [1 2 3; 3 3 4; 5 6 7];
[U, S, V] = svd(A);

A1 = S(1,1) * U(:,1) * V(:,1)';
A1_rounded = round(A1, 4)

RMSE1 = sqrt(mean((A - A1).^2, 'all'))""",
    2: """A2 = A1 + S(2,2) * U(:,2) * V(:,2)';
A2_rounded = round(A2, 4)

RMSE2 = sqrt(mean((A - A2).^2, 'all'))""",
    3: """u1 = U(:,1);
u2 = U(:,2);
u3 = U(:,3);

d1 = dot(u1, u2)
c  = cross(u1, u2)
d2 = dot(c, u3)""",
    4: """rank(U)
det(U)""",
    5: """load('image.mat');
A = double(A);
imshow(uint8(A));
title('Original Image');

[m, n] = size(A);
[U, S, V] = svd(A);

CR_target = 2;
k5 = round((m*n) / (CR_target*(m+n+1)))

Ak5 = U(:,1:k5) * S(1:k5,1:k5) * V(:,1:k5)';
imshow(uint8(Ak5));
title(sprintf('Rank-%d Approximation (CR ~ %d)', k5, CR_target));""",
    6: """RMSE_k5 = sqrt(mean((A - Ak5).^2, 'all'))""",
    7: """CR_list = [10, 25, 75];
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

k_list
RMSE_list""",
}

EXPLANATIONS = {
    2: (
        "A2 is closer to A than A1. Its RMSE (about 0.12) is much smaller than the rank-1 "
        "error (about 0.33) because the second singular term adds back another layer of structure. "
        "Each extra singular pair you keep lowers RMSE; a full rank-3 reconstruction would match A exactly."
    ),
    3: (
        "These results match what we expect from an orthogonal factor U. Distinct columns of U are "
        "orthonormal, so u1 and u2 are perpendicular and d1 is effectively zero. Since U is a proper "
        "rotation (det(U) = 1), the cross product u1 x u2 aligns with u3, which is why c matches u3 "
        "numerically and d2 = dot(c, u3) is 1."
    ),
    4: (
        "rank(U) = 3 and det(U) = 1 show that all three columns of U are linearly independent. "
        "Three independent vectors in R^3 form a basis, so the columns of U span all of R^3. "
        "That is guaranteed by the SVD: U is an orthogonal matrix, so its columns are an orthonormal basis."
    ),
    5: (
        "For an m-by-n image, a rank-k SVD approximation stores k left singular vectors, k singular "
        "values, and k right singular vectors. Solving CR = (m*n)/(k*(m+n+1)) for k with CR = 2 gives "
        "k = 801 for this image."
    ),
    7: (
        "As CR increases from 2 to 75, k drops sharply (801 to 160 to 64 to 21) while RMSE climbs "
        "steadily (3.15 to 8.21 to 12.30 to 18.27). Visually, CR near 2 and CR near 10 are nearly "
        "indistinguishable from the original; by CR near 25 fine texture and edges begin to soften; "
        "by CR near 75 the image is visibly blurred, though large-scale structure remains "
        "recognizable. This happens because the largest singular values capture coarse structure "
        "while smaller ones encode fine detail. CR near 10 (k = 160) offers the best balance of "
        "storage savings and visual fidelity."
    ),
}


def escape_cdata(code: str) -> str:
    return code.replace("]]>", "]]]]><![CDATA[>")


def replace_code_blocks(doc: str) -> str:
    pattern = re.compile(
        r"(<w:p><w:pPr><w:pStyle w:val=\"code\"/></w:pPr><w:r><w:t><!\[CDATA\[)%code(\s*\]\]></w:t></w:r></w:p>)"
    )
    prob = 1

    def repl(match):
        nonlocal prob
        if prob not in CODES:
            raise ValueError(f"No code defined for problem {prob}")
        code = escape_cdata(CODES[prob])
        block = f"{match.group(1)}{code}\n]]></w:t></w:r></w:p>"
        prob += 1
        return block

    return pattern.sub(repl, doc, count=7)


def replace_explanations(doc: str) -> str:
    """Fill Explain boxes in document order.

    Template variants (must all match):
      - Explain: </w:t></w:r></w:p>          (Problem 2)
      - Explain:</w:t></w:r></w:p>           (Problems 3, 5, 7)
      - Explain:</w:t></w:r><w:r><w:t> </w:t></w:r></w:p>  (Problem 4)
    Earlier builds skipped Problem 4's form, shifting P5/P7 text into the wrong boxes.
    """
    explain_re = re.compile(
        r"<w:r><w:rPr><w:b/></w:rPr><w:t>Explain:( )?</w:t></w:r>"
        r"(?:<w:r><w:t> </w:t></w:r>)?</w:p>"
    )
    ordered = [EXPLANATIONS[p] for p in (2, 3, 4, 5, 7)]
    idx = 0

    def repl(_match: re.Match) -> str:
        nonlocal idx
        if idx >= len(ordered):
            raise ValueError(f"More Explain boxes than expected (at index {idx})")
        text = ordered[idx]
        idx += 1
        return (
            f"<w:r><w:rPr><w:b/></w:rPr><w:t>Explain: </w:t></w:r>"
            f"<w:r><w:t>{text}</w:t></w:r></w:p>"
        )

    new_doc, n = explain_re.subn(repl, doc)
    if n != len(ordered):
        raise ValueError(f"Expected {len(ordered)} Explain boxes, replaced {n}")
    return new_doc


def replace_header(doc: str) -> str:
    doc = doc.replace(
        "<w:r><w:rPr><w:i/></w:rPr><w:t>Student Name</w:t></w:r>",
        "<w:r><w:rPr><w:i/></w:rPr><w:t>Draven Chen</w:t></w:r>",
        1,
    )
    doc = doc.replace(
        "<w:r><w:rPr><w:i/></w:rPr><w:t>Date</w:t></w:r>",
        "<w:r><w:rPr><w:i/></w:rPr><w:t>July 29, 2026</w:t></w:r>",
        1,
    )
    return doc


def build():
    work = ROOT / "mlx_build"
    if work.exists():
        shutil.rmtree(work)
    work.mkdir()
    with zipfile.ZipFile(TEMPLATE, "r") as zin:
        zin.extractall(work)
    doc_path = work / "matlab" / "document.xml"
    doc = doc_path.read_text(encoding="utf-8")
    doc = replace_header(doc)
    doc = replace_code_blocks(doc)
    doc = replace_explanations(doc)
    doc_path.write_text(doc, encoding="utf-8")
    if OUTPUT.exists():
        OUTPUT.unlink()
    shutil.make_archive(str(OUTPUT.with_suffix("")), "zip", work)
    built = OUTPUT.with_suffix(".zip")
    built.rename(OUTPUT)
    print(f"Built {OUTPUT}")


if __name__ == "__main__":
    build()
