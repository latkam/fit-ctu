import sys
import numpy as np


def rotate(image: np.array) -> np.array:
    if image.ndim == 2:
        return np.rot90(image, k=1, axes=(1, 0))
    elif image.ndim == 3:
        res = np.zeros((image.shape[1], image.shape[0], image.shape[2]), dtype=np.float64)
        for i in range(image.shape[2]):
            res[:, :, i] = np.rot90(image[:, :, i], k=1, axes=(1, 0))
        return res


def mirror(image: np.array) -> np.array:
    if image.ndim == 2:
        return np.fliplr(image)
    elif image.ndim == 3:
        res = np.zeros((image.shape), dtype=np.int)
        for i in range(image.shape[2]):
            res[:, :, i] = np.fliplr(image[:, :, i])
        return res


def inverse(image: np.array) -> np.array:
    return 255 - image


def bw(image: np.array) -> np.array:
    if image.ndim == 2:
        return image
    else:
        return 0.2989 * image[:, :, 0] + 0.587 * image[:, :, 1] + 0.114 * image[:, :, 2]


def lighten(image: np.array, percentage: float) -> np.array:
    return np.clip(image*(1.0+(percentage/100.0)), 0, 255)


def darken(image: np.array, percentage: float) -> np.array:
    return image*(1.0-(percentage/100.0))


def sharpen(image):
    return apply_filter(image, unsharp_mask_kernel)


def convolve2D(image: np.array, kernel: np.array, kernel_x: int, to_pad: int) -> np.array:
    padded = np.pad(image, [to_pad, to_pad], mode='constant')
    padded_x, padded_y = padded.shape[0], padded.shape[1]
    result = np.empty(image.shape, dtype=np.float64)
    for i in range(to_pad, padded_x - to_pad):
        for j in range(to_pad, padded_y - to_pad):
            result[i - to_pad][j - to_pad] = np.sum(
                np.multiply(padded[i - to_pad:i - to_pad + kernel_x, j - to_pad:j - to_pad + kernel_x], kernel))
    return result.clip(0, 255)


def apply_filter(image: np.array, kernel: np.array) -> np.array:
    kernel_x = kernel.shape[0]
    to_pad = int(kernel_x/2)
    kernel = np.flip(kernel, [0, 1])

    if image.ndim == 2:
        return convolve2D(image, kernel, kernel_x, to_pad)

    elif image.ndim == 3:
        res = np.zeros(image.shape, dtype=np.float64)
        for i in range(image.shape[2]):
            res[:, :, i] = convolve2D(image[:, :, i], kernel, kernel_x, to_pad)
        return res


unsharp_mask_kernel = (-1/256) * np.array([
    [1, 4, 6, 4, 1],
    [4, 16, 24, 16, 4],
    [6, 24, -476, 24, 6],
    [4, 16, 24, 16, 4],
    [1, 4, 6, 4, 1]
])
