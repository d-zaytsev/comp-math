import time

import numpy as np
import image_utils as iu
import saver


def svd_compression(A, s):
    n = np.shape(A[0])
    m = np.shape(A[1])

    U, sigma, V = np.linalg.svd(A, full_matrices=True)

    U = U[:, :s]
    sigma = np.diag(sigma[:s])
    V = V[:, :s]

    # A = n x m
    # U = n x s
    # V = m x s

    err = 100
    while err > 0.001:
        Q, R = np.linalg.qr(A @ V)
        newU = Q[:, 1:s]
        Q, R = np.linalg.qr(A.T @ U)
        V = Q[:, 1:s]
        U = newU
        sigma = R[1:s, 1:s]
        err = np.linalg.norm(A @ V - U @ sigma)

    return U, sigma, V


def get_matrix(u, s, v):
    print(u.shape, s.shape, v.shape)
    return (u @ s) / v


N = 100
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
