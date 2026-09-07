#!/usr/bin/env python3
"""Copy source figures and generate OcclusionGateNet blog SVGs.

Usage:
  python3 scripts/generate_occlusiongatenet_figures.py --source-repo /path/to/OcclusionGateNet
"""

from __future__ import annotations

import argparse
import csv
import html
import shutil
from pathlib import Path


TASKS = ("action", "gaze", "hands", "talk")
COLORS = ("#2f6fb3", "#d2691e", "#2e8b57", "#8a5fb5")
INK = "#172b3a"
MUTED = "#617181"
GRID = "#dce3e8"
BG = "#ffffff"


def esc(value: object) -> str:
    return html.escape(str(value), quote=True)


def svg_start(width: int, height: int, title: str, desc: str) -> list[str]:
    return [
        f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {width} {height}" role="img" aria-labelledby="title desc" font-family="Arial, Helvetica, sans-serif">',
        f"<title id=\"title\">{esc(title)}</title>",
        f"<desc id=\"desc\">{esc(desc)}</desc>",
        '<defs><marker id="arrow" markerWidth="10" markerHeight="10" refX="8" refY="5" orient="auto"><path d="M0,0 L10,5 L0,10 z" fill="#47606f"/></marker></defs>',
        f'<rect width="{width}" height="{height}" fill="{BG}"/>',
    ]


def text(x: float, y: float, value: str, size: int = 18, weight: int = 400,
         anchor: str = "middle", fill: str = INK) -> str:
    return (f'<text x="{x}" y="{y}" text-anchor="{anchor}" font-size="{size}" '
            f'font-weight="{weight}" fill="{fill}">{esc(value)}</text>')


def box(x: float, y: float, w: float, h: float, label: str, sub: str = "",
        fill: str = "#f5f8fa", stroke: str = "#8aa0ad") -> list[str]:
    out = [f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="12" fill="{fill}" stroke="{stroke}" stroke-width="2"/>']
    yy = y + h / 2 - (7 if sub else -6)
    out.append(text(x + w / 2, yy, label, 18, 700))
    if sub:
        out.append(text(x + w / 2, yy + 25, sub, 14, 400, fill=MUTED))
    return out


def arrow(x1: float, y1: float, x2: float, y2: float, dashed: bool = False) -> str:
    dash = ' stroke-dasharray="7 5"' if dashed else ""
    return f'<line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" stroke="#47606f" stroke-width="2.4" marker-end="url(#arrow)"{dash}/>'


def write_svg(path: Path, lines: list[str]) -> None:
    path.write_text("\n".join(lines + ["</svg>", ""]), encoding="utf-8")


def load_csv(path: Path) -> dict[str, dict[str, str]]:
    with path.open(newline="", encoding="utf-8") as handle:
        rows = {row["variant"]: row for row in csv.DictReader(handle)}
    expected = {"full", "no_body", "no_face", "no_occ", "no_hgnet", "no_gate"}
    if set(rows) != expected:
        raise ValueError(f"Unexpected variants: {sorted(rows)}")
    return rows


def main_f1(path: Path, rows: dict[str, dict[str, str]]) -> None:
    w, h = 1060, 650
    s = svg_start(w, h, "Clean vs masked task F1", "Full model macro-F1 under clean and masked evaluation.")
    s += [text(530, 42, "Full system: clean vs masked F1", 27, 700),
          text(530, 70, "Absolute drop (pp) and relative drop (%) are reported separately", 15, fill=MUTED)]
    left, right, top, bottom = 100, 1015, 115, 520
    ymin, ymax = 0.0, 1.0
    for tick in (0.0, .20, .40, .60, .80, 1.0):
        y = bottom - (tick - ymin) / (ymax - ymin) * (bottom - top)
        s += [f'<line x1="{left}" y1="{y}" x2="{right}" y2="{y}" stroke="{GRID}"/>',
              text(left - 14, y + 6, f"{tick:.2f}", 14, anchor="end", fill=MUTED)]
    s.append(text(28, 320, "Macro-F1", 16, 700, fill=INK).replace('<text ', '<text transform="rotate(-90 28 320)" '))
    full = rows["full"]
    centers = [210, 435, 660, 885]
    barw = 64
    for idx, (task, cx, color) in enumerate(zip(TASKS, centers, COLORS)):
        clean = float(full[f"test_clean_{task}"])
        masked = float(full[f"test_masked_{task}"])
        drop_pp = (clean - masked) * 100
        rel = (clean - masked) / clean * 100
        for val, x, opacity in ((clean, cx - 68, 1), (masked, cx + 4, .62)):
            y = bottom - (val - ymin) / (ymax - ymin) * (bottom - top)
            s += [f'<rect x="{x}" y="{y}" width="{barw}" height="{bottom-y}" rx="4" fill="{color}" fill-opacity="{opacity}"/>',
                  text(x + barw / 2, y - 9, f"{val:.4f}", 14, 700)]
        s += [text(cx, 552, task.capitalize(), 17, 700),
              text(cx, 578, f"drop {drop_pp:.2f} pp", 14, 700, fill="#a24d20"),
              text(cx, 600, f"relative {rel:.2f}%", 13, fill=MUTED)]
    s += [f'<rect x="360" y="620" width="16" height="16" fill="#2f6fb3"/>', text(385, 634, "Clean", 14, anchor="start"),
          f'<rect x="520" y="620" width="16" height="16" fill="#2f6fb3" fill-opacity=".62"/>', text(545, 634, "Masked", 14, anchor="start"),
          text(1018, 637, "Fixed split · seed 42 · clip macro-F1", 12, anchor="end", fill=MUTED)]
    write_svg(path, s)


def module_ablation(path: Path, rows: dict[str, dict[str, str]]) -> None:
    w, h = 1120, 690
    s = svg_start(w, h, "Module ablation masked F1", "Raw masked macro-F1 by task and model variant; signed deltas are relative to the full model.")
    s += [text(560, 42, "Module ablation on masked clips", 27, 700),
          text(560, 70, "Points show raw F1; labels show signed change from full model", 15, fill=MUTED)]
    variants = ("full", "no_body", "no_face", "no_occ", "no_hgnet", "no_gate")
    labels = (("Full", ""), ("No body", ""), ("No face", ""),
              ("No", "occlusion"), ("No", "HGNet"), ("No gate", ""))
    left, right, top, bottom = 165, 1070, 115, 565
    ymin, ymax = .20, .90
    for tick in (.20, .30, .40, .50, .60, .70, .80, .90):
        y = bottom - (tick - ymin) / (ymax - ymin) * (bottom - top)
        s += [f'<line x1="{left}" y1="{y}" x2="{right}" y2="{y}" stroke="{GRID}"/>',
              text(left - 14, y + 6, f"{tick:.2f}", 14, anchor="end", fill=MUTED)]
    xs = [220 + i * 165 for i in range(6)]
    offsets = (-27, -9, 9, 27)
    for i, (variant, label, x) in enumerate(zip(variants, labels, xs)):
        s.append(text(x, 590, label[0], 15, 700))
        if label[1]:
            s.append(text(x, 608, label[1], 14, 700))
        for task, color, dx in zip(TASKS, COLORS, offsets):
            value = float(rows[variant][f"test_masked_{task}"])
            y = bottom - (value - ymin) / (ymax - ymin) * (bottom - top)
            s.append(f'<circle cx="{x+dx}" cy="{y}" r="7" fill="{color}" stroke="#fff" stroke-width="2"/>')
            if variant != "full":
                base = float(rows["full"][f"test_masked_{task}"])
                delta = (value - base) * 100
                s.append(text(x + dx, y - 12, f"{delta:+.1f}", 10, 700, fill=color))
    for j, (task, color) in enumerate(zip(TASKS, COLORS)):
        lx = 250 + j * 175
        s += [f'<circle cx="{lx}" cy="635" r="7" fill="{color}"/>', text(lx + 14, 640, task.capitalize(), 14, anchor="start")]
    s += [text(1080, 640, "Delta labels: percentage points", 12, anchor="end", fill=MUTED),
          text(1080, 663, "Fixed split · seed 42 · clip macro-F1", 12, anchor="end", fill=MUTED)]
    write_svg(path, s)


def restoration_pipeline(path: Path) -> None:
    s = svg_start(1200, 500, "Landmark restoration pipeline", "ORFormer creates reference edge heatmaps and HGNet predicts landmarks; it does not restore a photograph.")
    s += [text(600, 42, "Landmark restoration pipeline", 27, 700),
          text(600, 72, "Reference generation and landmark prediction are separate stages", 15, fill=MUTED)]
    s += box(45, 175, 170, 92, "Face crop", "masked input")
    s.append(arrow(215, 221, 275, 221))
    s += box(275, 155, 235, 132, "ORFormer", "VQ-VAE + ViT", "#eef5fb", "#2f6fb3")
    s.append(arrow(510, 221, 570, 221))
    s += box(570, 175, 230, 92, "Reference", "edge heatmaps", "#eef5fb", "#2f6fb3")
    s.append(arrow(800, 221, 870, 221))
    s += box(870, 155, 180, 132, "HGNet", "crop + reference", "#fff4e9", "#d2691e")
    s.append(arrow(1050, 221, 1095, 221))
    s += box(1095, 175, 90, 92, "478", "landmarks", "#eef8f1", "#2e8b57")
    s += [arrow(130, 267, 130, 362), arrow(130, 362, 905, 362), arrow(905, 362, 905, 287),
          text(510, 350, "masked face crop", 14, 700, fill=MUTED),
          text(600, 440, "Output is landmark coordinates — no photorealistic face is reconstructed", 18, 700, fill="#a24d20")]
    write_svg(path, s)


def two_gates(path: Path) -> None:
    s = svg_start(1250, 650, "Two distinct gates", "Coordinate-level landmark selection occurs before task-level feature fusion.")
    s += [text(625, 42, "Two distinct gates", 27, 700), text(625, 71, "Gate 1 selects coordinates; Gate 2 fuses task features", 15, fill=MUTED)]
    s += [f'<rect x="35" y="100" width="1180" height="205" rx="18" fill="#eef5fb" stroke="#b8d1e5"/>',
          text(60, 132, "GATE 1 · Coordinate merge", 18, 700, anchor="start", fill="#2f6fb3")]
    s += box(75, 165, 180, 82, "Visibility", "per face region")
    s += box(320, 145, 210, 72, "Visible region", "MediaPipe coords", "#fff", "#2f6fb3")
    s += box(320, 225, 210, 72, "Occluded region", "restored coords", "#fff", "#d2691e")
    s += [arrow(255, 195, 320, 180), arrow(255, 218, 320, 260)]
    s += box(610, 175, 260, 92, "Coordinate merge", "region-wise selection", "#fff4e9", "#d2691e")
    s += [arrow(530, 180, 610, 200), arrow(530, 260, 610, 240)]
    s += box(950, 175, 210, 92, "Merged face", "478 landmarks", "#eef8f1", "#2e8b57")
    s.append(arrow(870, 221, 950, 221))
    s += [f'<rect x="35" y="340" width="1180" height="260" rx="18" fill="#f4f7f4" stroke="#c4d8c9"/>',
          text(60, 372, "GATE 2 · Task feature fusion", 18, 700, anchor="start", fill="#2e8b57")]
    s += box(75, 405, 185, 52, "Face regions", "")
    s += box(75, 475, 185, 52, "Visibility", "")
    s += box(75, 545, 185, 52, "Pose", "")
    s += [arrow(260, 431, 350, 453), arrow(260, 501, 350, 493)]
    s += box(350, 430, 225, 92, "Task face vector", "region gate + reliability", "#fff", "#2e8b57")
    s += [arrow(575, 476, 665, 476),
          '<polyline points="260,571 620,571 620,505 665,505" fill="none" stroke="#47606f" stroke-width="2.4" marker-end="url(#arrow)"/>',
          '<polyline points="260,501 300,501 300,395 700,395 700,430" fill="none" stroke="#47606f" stroke-width="2.1" stroke-dasharray="7 5" marker-end="url(#arrow)"/>']
    s += box(665, 430, 225, 92, "Scalar pose–face gate", "g·Pose + (1−g)·Face", "#fff4e9", "#d2691e")
    s.append(arrow(890, 476, 945, 476))
    for i, label in enumerate(("Action", "Gaze", "Hands", "Talk")):
        x, y = 945 + (i % 2) * 120, 405 + (i // 2) * 85
        s += box(x, y, 100, 58, label, "head", "#fff", "#8aa0ad")
    write_svg(path, s)


def data_supervision(path: Path) -> None:
    s = svg_start(1250, 560, "Pseudo-target supervision", "Clean MediaPipe landmarks supervise predictions from synthetically masked inputs.")
    s += [text(625, 42, "Training data and supervision", 27, 700),
          text(625, 71, "Clean-frame pseudo-targets supervise masked-input landmark prediction", 15, fill=MUTED)]
    s += box(55, 185, 180, 88, "Clean crop", "source frame", "#eef5fb", "#2f6fb3")
    s += [arrow(235, 208, 325, 145), arrow(235, 250, 325, 350)]
    s += box(325, 105, 230, 100, "MediaPipe", "on clean crop", "#eef8f1", "#2e8b57")
    s += box(325, 310, 230, 100, "Synthetic mask", "applied to clean crop", "#fff4e9", "#d2691e")
    s.append(arrow(555, 360, 650, 360))
    s += box(650, 310, 205, 100, "Masked input", "model input", "#fff4e9", "#d2691e")
    s.append(arrow(855, 360, 940, 360))
    s += box(940, 310, 240, 100, "Restorer prediction", "478 landmarks", "#f5f1fa", "#8a5fb5")
    s.append(arrow(555, 155, 785, 155))
    s += box(785, 105, 270, 100, "Clean pseudo-target", "MediaPipe landmarks", "#eef8f1", "#2e8b57")
    s += [arrow(1055, 155, 1100, 280), arrow(1060, 310, 1100, 230),
          f'<circle cx="1105" cy="255" r="36" fill="#fff" stroke="#47606f" stroke-width="2"/>', text(1105, 262, "loss", 17, 700),
          text(625, 495, "Pseudo-target ≠ manually annotated ground truth", 20, 700, fill="#a24d20"),
          text(625, 525, "The prediction is compared with landmarks inferred from the clean crop", 15, fill=MUTED)]
    write_svg(path, s)


def architecture_blog(source_path: Path, output_path: Path) -> None:
    """Create an evidence-neutral blog copy while preserving the source SVG."""
    content = source_path.read_text(encoding="utf-8")
    replacements = {
        "부위별 가시성 (Macro-F1 0.97)": "눈·코·입의 가시성",
        "차폐 좌표 복원 (NME ≤ 5%)": "reference heatmap → 478 landmarks",
        "IR fine-tuned · mAP 0.79": "NIR 영상의 17 keypoints",
    }
    for old, new in replacements.items():
        if content.count(old) != 1:
            raise ValueError(f"Expected one architecture label: {old}")
        content = content.replace(old, new)
    output_path.write_text(content, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-repo", required=True, type=Path)
    parser.add_argument("--output-dir", type=Path, default=Path(__file__).resolve().parents[1] / "assets" / "occlusiongatenet")
    args = parser.parse_args()
    source = args.source_repo.resolve()
    output = args.output_dir.resolve()
    output.mkdir(parents=True, exist_ok=True)

    copies = {
        "docs/assets/architecture.svg": "architecture.svg",
        "docs/assets/hgnet_reconstruction_grid.png": "hgnet_reconstruction_grid.png",
        "docs/assets/confusion_cases.png": "confusion_cases.png",
        "docs/assets/demo.gif": "demo.gif",
        "data/occlusion_subset/samples/occlusion_region_appearance_grid.png": "occlusion_region_appearance_grid.png",
    }
    for relative, name in copies.items():
        src = source / relative
        if not src.is_file():
            raise FileNotFoundError(src)
        shutil.copyfile(src, output / name)

    architecture_blog(source / "docs/assets/architecture.svg", output / "architecture-blog.svg")

    csv_path = source / "full_system/experiments/retrain_ablation/retrain_ablation_f1.csv"
    rows = load_csv(csv_path)
    main_f1(output / "main-f1.svg", rows)
    module_ablation(output / "module-ablation.svg", rows)
    restoration_pipeline(output / "restoration-pipeline.svg")
    two_gates(output / "two-gates.svg")
    data_supervision(output / "data-supervision.svg")
    print(f"Generated {len(copies) + 6} assets in {output}")


if __name__ == "__main__":
    main()
