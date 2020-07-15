import numpy as np


def convolve2D(image, kernel, kernel_x, to_pad):
    padded_image = np.pad(image, [to_pad, to_pad], mode='constant')
    padded_image_x = padded_image.shape[0]
    padded_image_y = padded_image.shape[1]
    result = np.empty(image.shape, dtype=int)
    for i in range(to_pad, padded_image_x - to_pad):
        for j in range(to_pad, padded_image_y - to_pad):
            result[i - to_pad][j - to_pad] = np.sum(
                np.multiply(padded_image[i - to_pad:i - to_pad + kernel_x, j - to_pad:j - to_pad + kernel_x], kernel))
    return result.clip(0, 255)


def apply_filter(image: np.array, kernel: np.array) -> np.array:
    # A given image has to have either 2 (grayscale) or 3 (RGB) dimensions
    assert image.ndim in [2, 3]
    # A given filter has to be 2 dimensional and square
    assert kernel.ndim == 2
    assert kernel.shape[0] == kernel.shape[1]

    kernel_x = kernel.shape[0]
    to_pad = int(kernel_x/2)
    kernel = np.flip(kernel, [0, 1])

    if image.ndim == 2:
        return convolve2D(image, kernel, kernel_x, to_pad)

    elif image.ndim == 3:
        res = np.zeros(image.shape, dtype=np.int64)
        for i in range(image.shape[2]):
            res[:, :, i] = convolve2D(image[:, :, i], kernel, kernel_x, to_pad)
        return res
    else:
        pass
