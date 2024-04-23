# SVD. Image compression methods
## Conditions and goals
For this assignment, 3 different image compression algorithms were implemented in Python. The main goal is to evaluate their effectiveness and compare them. It is also possible to save the intermediate state to a file.
## File format
the file stores metadata (H, width and height of the source image) and the matrices themselves U, sigma, V. Saving matrices is described in *saver.py*
## Experiments
### Test images
| | Luntik | 4k Beach | Field |
| --- | :----: | :----: | :----: |
| Image | ![resize](https://github.com/d-zaytsev/comp-math/assets/113129532/7a95bb8c-ce06-469d-b8dc-451d887d1d5a) | ![resize](https://github.com/d-zaytsev/comp-math/assets/113129532/cddaae7d-7c1c-4a2e-bb30-fed97f4856d1) | ![resize](https://github.com/d-zaytsev/comp-math/assets/113129532/e75c7b41-a36f-4cd5-9844-42701042cd92) |
| File size | **1.4M** | **121M** | **799K** |
### Memory comparison


| | Numpy svd | Basic method | Upgraded method |
| --- | --- | --- | --- |
| File size | Title | Title | Title |
| Image |  | Title | Title |
