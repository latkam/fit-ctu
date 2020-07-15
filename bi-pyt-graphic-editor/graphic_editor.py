from PIL import Image
import numpy as np
import sys
import parse
import operations


def manage_operations():
    opts = parse.parse()

    try:
        img = Image.open(opts[-2][1])
    except FileNotFoundError:
        print("Invalid argument.")
        sys.exit(1)

    data = np.array(img, dtype=np.float64)

    for opt in opts[:-2]:
        if opt[0] == "rotate":
            data = operations.rotate(data)
        elif opt[0] == "mirror":
            data = operations.mirror(data)
        elif opt[0] == "inverse":
            data = operations.inverse(data)
        elif opt[0] == "bw":
            data = operations.bw(data)
        elif opt[0] == "lighten":
            data = operations.lighten(data, float(opt[1]))
        elif opt[0] == "darken":
            data = operations.darken(data, float(opt[1]))
        elif opt[0] == "sharpen":
            data = operations.sharpen(data)

    res = Image.fromarray(data.astype(np.uint8))
    res.save(opts[-1][1])


if __name__ == "__main__":
    manage_operations()
