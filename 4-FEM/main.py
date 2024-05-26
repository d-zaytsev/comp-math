from math import pi
import matplotlib.pyplot as plt

from utils import *

hs = []
errs = []

# разные границы
for n in [1, 2, 7, 10]:
    # разные размеры сетки
    for s in [10, 100, 1000, 5000]:
        # общее решение
        a, b = 0.0, pi * n  # границы
        h = b / s  # шаг сетки

        lam = pi ** 2  # лямбду фиксируем
        xs = np.linspace(a, b, s + 1)  # равномерная сетка

        # решаем задачу и сохраняем результаты в ys
        sys = build_system(lam, xs, s, h)
        ys = solve(*sys)

        res, err = estimate(lam, b, s, h, ys, xs)
        errs.append(err)
        hs.append(h)

        print(f'{n},{s}: {res}, {err}')
plt.loglog(hs, errs, 'o')
plt.xlabel('h')
plt.ylabel('Error')
plt.title('Convergence Rate of FEM')
plt.grid(True)
plt.show()
