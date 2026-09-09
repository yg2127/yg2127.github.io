"""Recheck the educational FFDP article examples using only Python's standard library.

These average/half-difference filters and eta=0 DDIM calculations are
explanatory choices, not a reconstruction of the unavailable FFDP code.
Run: python3 verify_examples.py
"""

from math import isclose, sqrt


def haar_1d(values):
    """Use the article's 1/2 convention, not orthonormal Haar's 1/sqrt(2)."""
    if len(values) % 2:
        raise ValueError('The example requires an even length; no boundary padding.')
    pairs = list(zip(values[::2], values[1::2]))
    return ([sum(pair) / 2 for pair in pairs], [(a - b) / 2 for a, b in pairs])


def haar_2d(values):
    """First letter: row/time filter. Second letter: column/coordinate filter."""
    rows, cols = len(values), len(values[0])
    if rows % 2 or cols % 2 or any(len(row) != cols for row in values):
        raise ValueError('Use a rectangular matrix with even dimensions.')
    bands = {name: [] for name in ('LL', 'LH', 'HL', 'HH')}
    for i in range(0, rows, 2):
        output = {name: [] for name in bands}
        for j in range(0, cols, 2):
            a, b = values[i][j:j + 2]
            c, d = values[i + 1][j:j + 2]
            output['LL'].append((a + b + c + d) / 4)
            output['LH'].append((a - b + c - d) / 4)
            output['HL'].append((a + b - c - d) / 4)
            output['HH'].append((a - b - c + d) / 4)
        for name in bands:
            bands[name].append(output[name])
    return bands


def inverse_haar_2d(bands):
    rows, cols = len(bands['LL']), len(bands['LL'][0])
    out = [[0.0] * (cols * 2) for _ in range(rows * 2)]
    for i in range(rows):
        for j in range(cols):
            ll, lh, hl, hh = (bands[name][i][j] for name in ('LL', 'LH', 'HL', 'HH'))
            out[2 * i][2 * j] = ll + lh + hl + hh
            out[2 * i][2 * j + 1] = ll - lh + hl - hh
            out[2 * i + 1][2 * j] = ll + lh - hl - hh
            out[2 * i + 1][2 * j + 1] = ll - lh - hl + hh
    return out


EXAMPLE = [[10, 12, 20, 22], [14, 16, 24, 26], [18, 20, 28, 30], [30, 32, 40, 50]]


def verify():
    low, high = haar_1d([10, 12, 14, 30, 31, 32, 33, 34])
    assert low == [11, 22, 31.5, 33.5]
    assert high == [-1, -8, -0.5, -0.5]
    assert haar_1d(low) == ([16.5, 32.5], [-5.5, -1])
    bands = haar_2d(EXAMPLE)
    expected = {'LL': [[13, 23], [25, 37]], 'LH': [[-1, -1], [-1, -3]],
                'HL': [[-2, -2], [-6, -8]], 'HH': [[0, 0], [0, 2]]}
    assert bands == expected
    assert inverse_haar_2d(bands) == EXAMPLE

    # Detector-centered residual coordinates; hypothetical eta=0 schedule.
    detector, clean, noise = [100, 200], [104, 192], [.25, -.5]
    abar_t, abar_s = .25, .64
    noisy = [p + sqrt(abar_t) * (x0 - p) + sqrt(1 - abar_t) * e
             for p, x0, e in zip(detector, clean, noise)]
    recovered_noise = [(xt - p - sqrt(abar_t) * (x0 - p)) / sqrt(1 - abar_t)
                       for xt, p, x0 in zip(noisy, detector, clean)]
    assert all(isclose(a, b, abs_tol=1e-12) for a, b in zip(recovered_noise, noise))
    next_state = [p + sqrt(abar_s) * (x0 - p) + sqrt(1 - abar_s) * e
                  for p, x0, e in zip(detector, clean, recovered_noise)]
    assert all(isclose(a, b, abs_tol=1e-12) for a, b in zip(next_state, [103.35, 193.3]))
    print('1D L1:', low, 'H1:', high)
    print('2D bands:', bands)
    print('2D inverse reconstruction: exact')
    print('Residual DDIM example:', next_state, '(not an FFDP configuration)')


if __name__ == '__main__':
    verify()
