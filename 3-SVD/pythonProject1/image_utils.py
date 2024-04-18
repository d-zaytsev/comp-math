from PIL import Image
import numpy as np


def matrices_from_file(path):
    im = Image.open(path, 'r')
    width, height = im.size
    red, green, blue = zip(*im.getdata())

    red_matrix = [red[i * width:i * width + width] for i in range(height)]
    green_matrix = [green[i * width:i * width + width] for i in range(height)]
    blue_matrix = [blue[i * width:i * width + width] for i in range(height)]
    print(width, height)

    return np.array(red_matrix), np.array(green_matrix), np.array(blue_matrix)


def file_from_matrices(path, width, height, red, green, blue):
    im = Image.new('RGB', (width, height))

    red = red.flatten()
    green = green.flatten()
    blue = blue.flatten()

    im.putdata(list(zip(red, green, blue)))
    im.save(path)
