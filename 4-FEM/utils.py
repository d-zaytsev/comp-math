from math import sqrt, cos, sin
from scipy.integrate import quad

import numpy as np


def get_system_right(lam, j, xs, h):
    """
    Задаёт систему, определяющую неизвестные коэффициенты, необходимые для поиска приближённого решения [8.67]
    """

    # [8.76]
    op1 = ((xs[j - 1] * xs[j] * lam + (xs[j] ** 2) * (-lam) + 2) * cos(xs[j] * sqrt(lam)) - sqrt(lam) *
           ((xs[j - 1] - 2 * xs[j]) * sin(xs[j] * sqrt(lam))) - 2 * cos(xs[j - 1] * sqrt(lam))) / (lam ** (3 / 2))

    op2 = (((xs[j] ** 2) * (-lam) + xs[j] * xs[j + 1] * lam + 2) * cos(xs[j] * sqrt(lam)) + sqrt(lam) *
           ((2 * xs[j] - xs[j + 1]) * sin(xs[j] * sqrt(lam)) - xs[j + 1] * sin(xs[j + 1] * sqrt(lam))) - 2 *
           cos(xs[j + 1] * sqrt(lam))) / (lam ** (3 / 2))

    return (1 / h) * (op1 + op2)


def get_system_left(lam, j, k, xs, h):
    """
    Задаёт систему, определяющую неизвестные коэффициенты, необходимые для поиска приближённого решения [8.67]
    """

    if j > k:  # [8.69]
        return get_system_left(lam, k, j, xs, h)

    elif j + 1 == k:  # [8.70]
        return (1 / (h ** 2)) * ((-(1 / 6)) * (lam + 1) * ((xs[j + 1] - xs[j]) ** 3) + xs[j + 1] - xs[j])

    elif j == k:  # [8.71]
        return (1 / (h ** 2)) * (((1 / 3) * (lam + 1) * ((xs[j] - xs[j - 1]) ** 3)) - ((1 / 3) * (lam + 1) * (
                (xs[j] - xs[j + 1]) ** 3)) - xs[j - 1] + xs[j + 1])

    else:
        return 0


def build_system(lam, x, n, h):
    """
        Задаёт всю систему линейных уравнений
    """
    a = np.zeros(n)
    b = np.zeros(n)
    c = np.zeros(n)

    d = np.zeros(n)

    for i in range(1, n + 1):
        j = i - 1

        if i >= 1:
            a[j] = get_system_left(lam, i - 1, i, x, h)
        if i < n:
            c[j] = get_system_left(lam, i + 1, i, x, h)

        b[j] = get_system_left(lam, j, j, x, h)
        d[j] = get_system_right(lam, j, x, h)

    return a, b, c, d


def solve(a, b, c, d):
    """
    Решает систему линейных уравнений алгоритмом Томаса
    """
    n = len(d)
    y = np.zeros(n + 1)
    y[0], y[n] = 0, 0  # краевые условия

    for i in range(1, n):
        tmp = a[i] / b[i - 1]
        b[i] = b[i] - tmp * c[i - 1]
        d[i] = d[i] - tmp * d[i - 1]

    y[n - 1] = d[n - 1] / b[n - 1]

    for i in range(n - 2, -1, -1):
        y[i] = (d[i] - c[i] * y[i + 1]) / b[i]

    return y


def norm(f, l):
    """
    Нормирует функцию
    :param f: Функция которую нормируем
    """
    res, _ = quad(f, 0, l)
    return np.sqrt(res)


def get_basis(j, xs, x, n, h):
    """
    Задаёт кусочно-линейную базисную функцию [8.59]
    :param h: шаг сетки
    :param j: индекс базисной функции
    :param xs: вектор значений узлов сетки
    :param x: точка, в которой вычисляется базисная функция
    :param n: количество узлов сетки
    :return: [8.59] значение кусочно-линейной базисной функции в точке x
    """

    class FunctionNum:
        FIRST = 0
        LAST = n

    match j:
        case FunctionNum.FIRST:  # [8.60]
            if xs[0] <= x <= xs[1]:
                return (xs[1] - x) / h
            else:
                return 0

        case FunctionNum.LAST:  # [8.61]
            if xs[n - 1] <= x <= xs[n]:
                return (x - xs[n - 1]) / h
            else:
                return 0

        case _:  # [8.59]
            if xs[j - 1] <= x <= xs[j]:
                return (x - xs[j - 1]) / h
            elif xs[j] <= x <= xs[j + 1]:
                return (xs[j + 1] - x) / h
            else:
                return 0


def interpol(ys, x, xs, n, h):
    """
    Интерполяция [8.65]

    :param h: шаг сетки
    :param ys: вектор значений неизвестной функции в точках узлов сетки
    :param x: точка для которой ищем значение
    :param xs: вектор значений узлов сетки
    :param n: количество узлов сетки
    :return: примерное значение функции в точке
    """

    l, r = 0, n

    # Определение интервала [x_j,x_j+1], в который попадает x
    while r - l > 1:
        m = (l + r) // 2
        if x > xs[m]:
            l = m
        else:
            r = m

    # Интерполяция значения x неизвестной функции y (с помощью соседних узлов сетки)
    return ys[l] * get_basis(l, xs, x, n, h) + ys[r] * get_basis(r, xs, x, n, h)


def estimate(lam, l, n, h, ys, xs):
    """
    Оценка погрешности приближённого решения [8.84]

    :param lam: фиксированный параметр λ
    :param l: длина интервала
    :param n: размер сетки
    :param h: шаг сетки
    :param ys: приближённое решение (полученное через МКЭ)
    :param xs: точки, в которых вычислялось приближённое значение
    :return: Проходит ли оценка, разница
    """

    f = lambda x, _lam: x * sin(x * sqrt(_lam))  # (p*y')' - q*y = -f <- эта f
    ans = lambda x, _lam: sin(sqrt(_lam) * x)  # реальный ответ, с ним сравниваем

    stroke_c = l / 2  # [8.84]
    c = l / 2 + 1  # [8.42]

    y = np.array([ans(i * h, lam) for i in range(n)])  # значения функции (в узлах сетки)
    y_h = np.array([interpol(ys, i * h, xs, n, h) for i in range(n)])  # приближённые значения функции (в узлах сетки)

    # левая часть из [8.84]
    left = np.sqrt(np.sum((y - y_h) ** 2) * h)

    # правая часть из [8.84]
    right = (stroke_c * c) ** 2 * h ** 2 * norm(lambda x: f(x, lam) ** 2, l)

    return left <= right, left
