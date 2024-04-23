import time

import numpy as np
import image_utils as iu
import saver


def svd_compression(A, s):
    m = A.shape[1]
    P = np.random.randn(m, s)
    Z = A @ P

    iters = 10
    for _ in range(iters):
        Z = A @ (A.T @ Z)

    Q, _ = np.linalg.qr(Z, mode='reduced')
    Y = Q.T @ A
    UY, sigma, V = np.linalg.svd(Y, full_matrices=False)
    U = Q @ UY

    return U, np.diag(sigma), V


def get_matrix(u, s, v):
    return u @ s @ v


N = 10
path = "/home/dmitriy/Desktop"
image = path + '/default1.bmp'
compressed_image = path + '/compressed.bmp'
compressed_file = path + '/data.rofl'

# Получаем цвета и размеры из файла
red, green, blue = iu.matrices_from_file(image)
width, height = iu.file_size(image)

# Жмыхаем цвета (каждую матрицу отдельно обрабатываем)

compressed_red = get_matrix(*svd_compression(red, N))
compressed_green = get_matrix(*svd_compression(green, N))
compressed_blue = get_matrix(*svd_compression(blue, N))

iu.file_from_matrices(compressed_image, width, height, compressed_red, compressed_green, compressed_blue)

# Сохраняем матрицы в файл
saver.create(compressed_file, width, height, N)
saver.write_matrices(compressed_file, svd_compression(red, N), svd_compression(green, N), svd_compression(blue, N))

print('N:', N, '\nwidth:', width, '\nheight:', height)
print('compressed data:', saver.file_size(compressed_file), '\nimage:', saver.file_size(image))
