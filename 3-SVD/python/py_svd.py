import numpy as np
import image_utils as iu


def svd_compression(color_matrix, n):
    """
    Реализация SVD из numpy
    Args:
        color_matrix: Матрица со значениями цвета
        n: Ранг матрицы (чем меньше, тем больше жмых)
    """
    u, sigma, v = np.linalg.svd(color_matrix, full_matrices=False)
    u = u[:, :n]
    sigma = np.diag(sigma[:n])
    v = v[:n, :]

    return u, sigma, v


def get_matrix(u, s, v):
    return np.dot(u, np.dot(s, v))


N = 10
path = '/home/dmitriy/Desktop'
file = path + '/default.bmp'
cfile = path + '/compressed.bmp'

# Получаем цвета и размеры из файла
red, green, blue = iu.matrices_from_file(file)
width, height = iu.file_size(file)

# Жмыхаем цвета (каждую матрицу отдельно обрабатываем)

# height, max(N, height), max(N, height)

red = get_matrix(*svd_compression(red, N))
green = get_matrix(*svd_compression(green, N))
blue = get_matrix(*svd_compression(blue, N))

# Засовываем матрицы обратно в файл
iu.file_from_matrices(cfile, width, height, red, green, blue)

import saver

saver.create('/home/dmitriy/Desktop/data.rofl', width, height, N)
saver.write_matrices('/home/dmitriy/Desktop/data.rofl', svd_compression(red, N), svd_compression(green, N),
                     svd_compression(blue, N))
