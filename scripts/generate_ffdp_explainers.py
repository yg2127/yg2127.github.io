"""Generate standalone SVG teaching diagrams; no paper figures are modified."""

from html import escape
from importlib.util import module_from_spec, spec_from_file_location
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'assets' / 'ffdp'
OUT.mkdir(parents=True, exist_ok=True)
spec = spec_from_file_location('examples', ROOT / 'assets/code/ffdp-study/verify_examples.py')
examples = module_from_spec(spec)
spec.loader.exec_module(examples)
examples.verify()


def text(x, y, value, size=22, color='#193644', anchor='start'):
    return f'<text x="{x}" y="{y}" font-size="{size}" fill="{color}" text-anchor="{anchor}" font-family="Arial, sans-serif">{escape(str(value))}</text>'


def rect(x, y, w, h, color, stroke='none'):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="5" fill="{color}" stroke="{stroke}"/>'


def matrix(x, y, data, cell=52, tint='#edf4f7'):
    chunks = []
    for i, row in enumerate(data):
        for j, value in enumerate(row):
            chunks.append(rect(x+j*cell, y+i*cell, cell-3, cell-3, tint))
            chunks.append(text(x+j*cell+(cell-3)/2, y+i*cell+33, f'{value:g}', 24, anchor='middle'))
    return ''.join(chunks)


def save(name, width, height, body, title, desc):
    svg = (f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}" role="img">'
           f'<title>{escape(title)}</title><desc>{escape(desc)}</desc>'
           f'<rect width="100%" height="100%" fill="#ffffff"/>{body}</svg>')
    (OUT / name).write_text(svg)


bands = examples.haar_2d(examples.EXAMPLE)
parts = [text(36, 42, 'A small 2D DWT calculation', 28),
         text(36, 76, 'First letter: time   /   Second letter: coordinate order', 20, '#516873'),
         text(42, 122, 'Input P (4 x 4)', 23), matrix(42, 147, examples.EXAMPLE),
         text(42, 392, 'Rows: 4 frames', 21), text(42, 422, 'Columns: 4 coordinate entries', 19),
         text(295, 255, 'DWT', 23, '#0067c7', 'middle'),
         '<path d="M265 272 H330" stroke="#0067c7" stroke-width="3"/><path d="M322 265 L330 272 L322 279" fill="none" stroke="#0067c7" stroke-width="3"/>']
for label, x, y, tint in [('LL',365,140,'#e5f1ec'),('LH',555,140,'#e7effb'),('HL',365,326,'#fff0d8'),('HH',555,326,'#f7e6ee')]:
    parts += [text(x, y-16, label+' (2 x 2)', 23), matrix(x, y, bands[label], tint=tint)]
parts += [text(36, 510, 'Average / half-difference filters; no padding.', 20),
          text(36, 541, 'Teaching example, not the published FFDP tensor layout.', 20, '#516873')]
save('explainer-dwt-grid.svg',760,568,''.join(parts),'Two-dimensional average and difference calculation',
     'A 4 by 4 input produces LL, LH, HL and HH arrays of size 2 by 2. All numbers are checked by the accompanying script.')

parts=[text(34,42,'36 frames at every denoising step',28),
       text(178,79,'Video frame index',21),text(34,79,'State',21)]
for n in [1,6,12,18,24,30,36]:
    parts.append(text(180+(n-1)*16+7,105,n,16,anchor='middle'))
for state in range(6):
    y=123+state*78
    parts.append(text(34,y+22,'Initial' if state==0 else f'Update {state}',22))
    for frame in range(36):
        color=['#d9e2ed','#c8dced','#b8d7e8','#9bcbbb','#7cbaa7','#55a889'][state]
        parts.append(rect(180+frame*16,y,14,30,color))
    if state<5:
        parts += ['<path d="M467 '+str(y+35)+' V'+str(y+68)+'" stroke="#516873" stroke-width="2"/>',
                  '<path d="M462 '+str(y+61)+' L467 '+str(y+68)+' L472 '+str(y+61)+'" fill="none" stroke="#516873" stroke-width="2"/>',
                  text(480,y+59,'Model + update',17,'#516873')]
parts += [text(34,597,'Five updates of the whole sequence; not five video frames.',21),
          text(34,629,'Conceptual diagram; colors are not model predictions.',19,'#516873')]
save('explainer-diffusion-axes.svg',800,650,''.join(parts),'Video frames and denoising steps are separate axes',
     'Each row contains 36 video frames. Five model-and-update operations transform the entire sequence; they do not generate one video frame at a time.')
print('Wrote two FFDP teaching SVGs to',OUT)
