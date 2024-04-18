import numpy as np
import image_utils as iu


# Реализация SVD из numpy
def svd_compression(A, N):
    U, S, V = np.linalg.svd(A, full_matrices=False)
    compressed_U = U[:, :N]
    compressed_S = np.diag(S[:N])
    compressed_V = V[:N, :]

    return (compressed_U, compressed_S, compressed_V)


red, green, blue = iu.matrices_from_file('/home/dmitriy/Desktop/123.bmp')

iu.file_from_matrices("/home/dmitriy/Desktop/124.jpeg", 640, 426, red, green, blue)
# from numpy.linalg import eigh, norm

# A = np.array([[-5, 3, 8], [2, 5, 1], [-1, 8, 15]])  # матрица из изображения
# ev, V = eigh(A.T @ A)  # собственные числа и вектора
#
# # переворот жоски
# v0 = A @ V[:, 0]
# v1 = A @ V[:, 1]
# v2 = A @ V[:, 2]
#
# # нормируем вектора
# u0 = v0 / norm(v0)
# u1 = v1 / norm(v1)
# u2 = v2 / norm(v2)
#
# # Новая матрица из отнормированных собственных векторов
# U = np.array([u0, u1, u2]).T
#
# # Диагональная матрица
# D = np.round(U.T @ A @ V, 5)
