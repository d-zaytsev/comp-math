"""
WIDTH:INT
HEIGHT:INT
N:INT
U:Array<INT>
SIGMA:Array<INT>
V:Array<INT>
"""
import numpy as np


def create(path, width, height, n):
    meta = [width.to_bytes(2, 'little'), height.to_bytes(2, 'little'), n.to_bytes(2, 'little')]

    with open(path, "wb") as file:
        file.write(b''.join(meta))


def write_matrices(path, red, green, blue):
    data_r = [x.tobytes() for x in red]
    data_g = [x.tobytes() for x in green]
    data_b = [x.tobytes() for x in blue]

    with open(path, "ab") as file:
        for item in [data_r, data_g, data_b]:
            for x in item:
                file.write(x)
