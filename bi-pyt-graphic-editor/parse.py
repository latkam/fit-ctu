import sys
import argparse

"""
Inspiration for class OrderedNamespace:
https://stackoverflow.com/questions/7737769/python-argparse-how-to-get-order-of-optional-arguments-from-command-line
"""


class OrderedNamespace(argparse.Namespace):
    def __init__(self, **kwargs):
        self.__dict__["_order"] = []
        super().__init__(**kwargs)

    def __setattr__(self, attr, value):
        super().__setattr__(attr, value)
        if getattr(self, attr):
            self.__dict__["_order"].append(attr)

    def ordered(self):
        return list(((attr, getattr(self, attr)) for attr in self._order))


def parse():
    parser = argparse.ArgumentParser()

    parser.add_argument("--rotate",
                        action="store_true",
                        default=False,
                        help="rotate image 90 degrees to the right")
    parser.add_argument("--mirror",
                        action="store_true",
                        default=False,
                        help="mirror image")
    parser.add_argument("--inverse",
                        action="store_true",
                        default=False,
                        help="inverse image (create a negative)")
    parser.add_argument("--bw",
                        action="store_true",
                        default=False,
                        help="transform image to grayscale, does nothing \
with black and white image")
    parser.add_argument("--lighten",
                        type=int,
                        choices=range(0, 101),
                        action="store",
                        default=False,
                        help="lighten image")
    parser.add_argument("--darken",
                        type=int,
                        choices=range(0, 101),
                        action="store",
                        default=False,
                        help="darken image")
    parser.add_argument("--sharpen",
                        action="store_true",
                        default=False,
                        help="apply unsharp mask")
    parser.add_argument("INPUT_IMAGE_PATH",
                        action="store",
                        default=False,
                        help="Input image path")
    parser.add_argument("OUTPUT_IMAGE_PATH",
                        action="store",
                        default=False,
                        help="Output image path")

    args = parser.parse_args(namespace=OrderedNamespace())
    return args.ordered()
