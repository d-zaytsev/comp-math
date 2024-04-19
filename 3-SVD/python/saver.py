"""
-- meta --
WIDTH: 2 байта
HEIGHT: 2 байта
N: 2 байта
-- для каждой матрицы --
U: по 1 байту каждое значение, len(U) = height
SIGMA: по 1 байту каждое значение, len(SIGMA) = N
V: по 1 байту каждое значение, len(V) = N
"""
import os


def create(path, width, height, n):
    meta = [width.to_bytes(2, 'little'), height.to_bytes(2, 'little'), n.to_bytes(2, 'little')]

    with open(path, "wb") as file:
        file.write(b''.join(meta))


def write_matrices(path, red, green, blue):
    data_r = [x.tobytes() for x in red]
    data_g = [x.tobytes() for x in green]
    data_b = [x.tobytes() for x in blue]

    with open(path, "ab") as file:
        for item in [data_r, data_g, data_b]:  # r g b
            for x in item:  # u s v
                file.write(x)


def file_size(path):
    return str((os.path.getsize(path) // 1024)) + ' Kbs'

def read_matrices(path):
    with open(path, "rb") as file:
        data = [x.to_bytes() for x in file.read()]

    # Читаем заголовок (little end)
    width = int.from_bytes(b'' + data[1] + data[0])
    height = int.from_bytes(b'' + data[3] + data[2])
    n = int.from_bytes(b'' + data[5] + data[4])

    res = []

    # Дальше читаем матрицы (у нас их 9 штук 🤣🤣🤣)
    for i in range(3):
        f = 6 + height * i  # сдвиг относительно матриц r g b
        u = data[f:height + f]
        s = data[height + f + 1:height + f + 1 + n]
        v = data[height + f + 2 + n:height + f + 2 + 2 * n]
        res.append((u, s, v))

    return res
