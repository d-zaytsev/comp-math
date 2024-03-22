import numpy as np
import matplotlib.pyplot as plt
import sys
import os

build_folder = sys.argv[1]
print(os.listdir(build_folder))

file = np.loadtxt(build_folder + "/grid.txt")

x = np.linspace(0, 1, file.shape[0])
y = np.linspace(0, 1, file.shape[1])
X, Y = np.meshgrid(x, y)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot_wireframe(X, Y, file)

ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

plt.savefig(build_folder + "/picture.png")
# plt.show()