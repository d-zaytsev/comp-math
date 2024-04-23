import time

import numpy as np
import image_utils as iu
import saver


def svd_compression(color_matrix, n):
    """
    Реализация SVD по-простому
    Args:
        color_matrix: Матрица со значениями цвета
        n: Ранг матрицы (чем меньше, тем больше жмых)
    """
    evals, evcs = np.linalg.eig(np.dot(color_matrix.T, color_matrix))

    singular_values_sorted = np.sort(np.sqrt(evals))[::-1]

    V = evcs

    U = np.dot(color_matrix, V) / singular_values_sorted

    S = np.dot(U.T, np.dot(color_matrix, V))

    return U[:, :n], np.round(S[:n, :n], 5), V.T[:n, :]


def get_matrix(u, s, v):
    return np.real(np.dot(u, np.dot(s, v)))


N = 1
path = "/home/dmitriy/Desktop"
image = path + '/default.bmp'
compressed_image = path + '/compressed.bmp'
compressed_file = path + '/data.rofl'

# Получаем цвета и размеры из файла
red, green, blue = iu.matrices_from_file(image)
width, height = iu.file_size(image)

# Жмыхаем цвета (каждую матрицу отдельно обрабатываем)
start_time = time.perf_counter()
compressed_red = get_matrix(*svd_compression(red, N))
compressed_green = get_matrix(*svd_compression(green, N))
compressed_blue = get_matrix(*svd_compression(blue, N))

iu.file_from_matrices(compressed_image, width, height, compressed_red, compressed_green, compressed_blue)
print(f'time:{time.perf_counter() - start_time}')

# Сохраняем матрицы в файл
saver.create(compressed_file, width, height, N)
saver.write_matrices(compressed_file, svd_compression(red, N), svd_compression(green, N), svd_compression(blue, N))

print('N:', N, '\nwidth:', width, '\nheight:', height)
print('compressed data:', saver.file_size(compressed_file), '\nimage:', saver.file_size(image))
