import time

import numpy as np
import image_utils as iu
import saver


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


N = 2  # во сколько раз сжимать
path = "/home/dmitriy/Desktop"
image = path + '/luntik.bmp'
compressed_image = path + '/compressed.bmp'
compressed_file = path + '/data.rofl'

# Получаем цвета и размеры из файла
red, green, blue = iu.matrices_from_file(image)
width, height = iu.file_size(image)

s = ((width + height) // 34) // N

# Жмыхаем цвета (каждую матрицу отдельно обрабатываем)

start_time = time.perf_counter()
compressed_red = get_matrix(*svd_compression(red, s))
compressed_green = get_matrix(*svd_compression(green, s))
compressed_blue = get_matrix(*svd_compression(blue, s))

iu.file_from_matrices(compressed_image, width, height, compressed_red, compressed_green, compressed_blue)
print(f'time:{time.perf_counter() - start_time}')

# Сохраняем матрицы в файл
saver.create(compressed_file, width, height, s)
saver.write_matrices(compressed_file, svd_compression(red, s), svd_compression(green, s), svd_compression(blue, s))

print('N:', s, '\nwidth:', width, '\nheight:', height)
print('compressed data:', saver.file_size(compressed_file), '\nimage:', saver.file_size(image))