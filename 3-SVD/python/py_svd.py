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


N = 10
path = '/home/dmitriy/Desktop'
image = path + '/default.bmp'
compressed_image = path + '/compressed.bmp'
compressed_file = path + '/data.rofl'

# Получаем цвета и размеры из файла
red, green, blue = iu.matrices_from_file(image)
width, height = iu.file_size(image)

# Жмыхаем цвета (каждую матрицу отдельно обрабатываем)

red = get_matrix(*svd_compression(red, N))
green = get_matrix(*svd_compression(green, N))
blue = get_matrix(*svd_compression(blue, N))

iu.file_from_matrices(compressed_image, width, height, red, green, blue)

# Сохраняем матрицы в файл
saver.create(compressed_file, width, height, N)
saver.write_matrices(compressed_file, svd_compression(red, N), svd_compression(green, N), svd_compression(blue, N))

print(svd_compression(red, N)[1])

print('N:', N, '\nwidth:', width, '\nheight:', height)
print('compressed data:', saver.file_size(compressed_file), '\nimage:', saver.file_size(image))
