from PIL import Image
import numpy as np


def matrices_from_file(path):
    im = Image.open(path, 'r')
    width, height = im.size

    # Превращаем в три разных списка из пикселей
    red, green, blue = zip(*im.getdata())

    # Создаём матрицу по параметрам width, height
    red_matrix = [red[i * width:i * width + width] for i in range(height)]
    green_matrix = [green[i * width:i * width + width] for i in range(height)]
    blue_matrix = [blue[i * width:i * width + width] for i in range(height)]

    return np.array(red_matrix), np.array(green_matrix), np.array(blue_matrix)


def file_size(path):
    return Image.open(path, 'r').size


def file_from_matrices(path, width, height, red, green, blue):
    im = Image.new('RGB', (width, height))

    red = red.flatten()
    green = green.flatten()
    blue = blue.flatten()

    # Сначала нужно привести данные к int (иначе не засунет в файл)
    im.putdata(list(zip(np.rint(red).astype(int), np.rint(green).astype(int), np.rint(blue).astype(int))))
    im.save(path)
