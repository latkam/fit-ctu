import pytest
import numpy as np
from numpy.testing import assert_equal, assert_almost_equal
import operations as op


def test_type():
    res = op.rotate(op.mirror(op.inverse(op.bw(op.lighten(op.darken(op.sharpen(img), 30), 70)))))
    assert isinstance(res, np.ndarray)


def test_rotate1():
    assert_equal(ref1, op.rotate(img))


def test_rotate2():
    assert_equal(img, op.rotate(op.rotate(op.rotate(op.rotate(img)))))


def test_mirror1():
    assert_equal(ref2, op.mirror(img))


def test_mirror2():
    assert_equal(img, op.mirror(op.mirror(img)))


def test_inverse1():
    assert_equal(ref3, op.inverse(img))


def test_inverse2():
    assert_equal(img, op.inverse(op.inverse(img)))


def test_bw():
    assert_almost_equal(ref4, op.bw(img))


def test_lighten():
    assert_almost_equal(ref5, op.lighten(img, 48.97))


def test_darken():
    assert_almost_equal(ref6, op.darken(img, 37.58))


def test_sharpen():
    assert_almost_equal(ref7, op.sharpen(img))



img = np.array([
    [[1, 2, 3],
     [4, 5, 6],
     [7, 8, 9]],

    [[10, 11, 12],
     [13, 14, 15],
     [16, 17, 18]],

    [[19, 20, 21],
     [22, 23, 24],
     [25, 26, 27]]
])

ref1 = np.array([
 [[19, 20, 21],
  [10, 11, 12],
  [1,  2,  3]],

 [[22, 23, 24],
  [13, 14, 15],
  [4,  5,  6]],

 [[25, 26, 27],
  [16, 17, 18],
  [7,  8,  9]]
])

ref2 = np.array([
 [[7,  8,  9],
  [4,  5,  6],
  [1,  2,  3]],

 [[16, 17, 18],
  [13, 14, 15],
  [10, 11, 12]],

 [[25, 26, 27],
  [22, 23, 24],
  [19, 20, 21]]
])

ref3 = np.array([
 [[254, 253, 252],
  [251, 250, 249],
  [248, 247, 246]],

 [[245, 244, 243],
  [242, 241, 240],
  [239, 238, 237]],

 [[236, 235, 234],
  [233, 232, 231],
  [230, 229, 228]]
])

ref4 = np.array([
 [1.8149,  4.8146,  7.8143],
 [10.814,  13.8137, 16.8134],
 [19.8131, 22.8128, 25.8125]]
)

ref5 = np.array([
 [[1.4897,  2.9794,  4.4691],
  [5.9588,  7.4485,  8.9382],
  [10.4279, 11.9176, 13.4073]],

 [[14.897,  16.3867, 17.8764],
  [19.3661, 20.8558, 22.3455],
  [23.8352, 25.3249, 26.8146]],

 [[28.3043, 29.794,  31.2837],
  [32.7734, 34.2631, 35.7528],
  [37.2425, 38.7322, 40.2219]]
])


ref6 = np.array([
 [[0.6242,  1.2484,  1.8726],
  [2.4968,  3.121,   3.7452],
  [4.3694,  4.9936,  5.6178]],

 [[6.242,   6.8662,  7.4904],
  [8.1146,  8.7388,  9.363],
  [9.9872, 10.6114, 11.2356]],

 [[11.8598, 12.484,  13.1082],
  [13.7324, 14.3566, 14.9808],
  [15.605,  16.2292, 16.8534]]
])

ref7 = np.array([
 [[0,          0,           1.48828125],
  [2.640625,   4.0390625,   5.4375],
  [9.14453125, 10.671875,   12.19921875]],

 [[13,          14.3984375,  15.796875],
  [16.046875,   17.28125,    18.515625],
  [23.359375,   24.7578125,  26.15625]],

 [[30.56640625, 32.09375,    33.62109375],
  [33.71875,    35.1171875,  36.515625],
  [41.27734375, 42.8046875,  44.33203125]]
])
