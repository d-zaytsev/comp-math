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
| Width x Height | **811x594**  | **7952x5304**  | **640x426** |

### N = 10
*[!] The matrices at the output of the algorithms are limited in size to N.*

- Luntik.bmp

| | Numpy svd | Basic method | Upgraded method |
| --- | :----: | :----: | :----: |
| File size | 331K | 663K | 331K |
| Time | 0.5s | 2.72s | 0.19s |

- Beach.bmp

| | Numpy svd | Basic method | Upgraded method |
| --- | :----: | :----: | :----: |
| File size | 3109K | - | 3109K |
| Time | 265s | - | 13.7s |

- Field.bmp

| | Numpy svd | Basic method | Upgraded method |
| --- | :----: | :----: | :----: |
| File size | 252k | 504K | 252K |
| Time | 0.3s | 1.6s | 0.14s |


### 2x compression
*[!] The size of each file is compressed by 2 times.*

- Luntik.bmp

| | Numpy svd | Basic method | Upgraded method |
| --- | :----: | :----: | :----: |
| File size | 837K | 864K | 837K |
| Time | 0.5s | 2.77s | 0.2s |
| N | 25 | 13 | 25 |

- Beach.bmp: None of the algorithms could process the image in a sane time.

- Field.bmp

| | Numpy svd | Basic method | Upgraded method |
| --- | :----: | :----: | :----: |
| File size | 380k | 402K | 380K |
| Time | 0.3s | 1.2s | 0.15s |
| N | 15 | 8 | 15 |

### Minor compression test
*[!] An attempt to compress the image slightly to see how the algorithms behave in this case.*

- Luntik.bmp

| | Numpy svd | Basic method | Upgraded method |
| --- | :----: | :----: | :----: |
| Compressed image | ![resize](https://github.com/d-zaytsev/comp-math/assets/113129532/ea958982-9a2a-4869-b45c-349d221cc2fa) | ![resize](https://github.com/d-zaytsev/comp-math/assets/113129532/fdb61eef-514c-4cbb-bc74-beaca9e6a385) | ![resize](https://github.com/d-zaytsev/comp-math/assets/113129532/d2507ad2-37d7-486f-9340-d3b89a12cad5) |
| Time | 0.7s | 2.7s | 0.3s |
| N | 200 | 200 | 200 |
