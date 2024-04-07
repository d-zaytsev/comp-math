# Implementation of cubic and B-splines in Maple
## Conditions and goals
Experiments were carried out on standard functions from Maple and on my functions. The main goal was to understand which functions are most suitable for approximating with presented splines and how my implementation differs from the implementation in Maple.

## Experiments

### Cubic Splines

From the researches I read and personal reflections, I realized that cubic splines work worst when approximating highly periodic functions, functions with discontinuities, and curves.

- **Blue** - original function;
- **Red** - approximation with my cubic spline;
- **Green** - approximation by cubic spline from maple

*Comparing the Maple cubic spline and the one I created, we can conclude that with high frequency of the function it is necessary to increase the number of nodes. (in my implementation there are always 10)*
| $f(x) = sin(5\pi x)$  | $f(x) = sin(8\pi x)$  |
|---|---|
| ![image](https://github.com/d-zaytsev/comp-math/assets/113129532/91190658-411e-41bf-ae31-2e952ffb08c7)  | ![image](https://github.com/d-zaytsev/comp-math/assets/113129532/bfe905c9-0903-49be-88ec-a739f7dc7f42)|
|![image](https://github.com/d-zaytsev/comp-math/assets/113129532/19774aa2-9622-4b91-be1e-19d2346a9c60)   | ![image](https://github.com/d-zaytsev/comp-math/assets/113129532/ebdcb409-4833-4ade-bb63-3e19036f93ed)|

*As we can see in the examples below, splines do not do a very good job of approximating functions with discontinuities. Non-standard periodic functions are also very difficult to approximate.*
| $f(x) = sign(x - 0.5)$ | $f(x) = 1/2 - sum(sin(n*\pi*x)/n, n = 1..10) / \pi$|
|---|---|
|![image](https://github.com/d-zaytsev/comp-math/assets/113129532/592e3447-11c5-4457-8964-127e4c3a5e4f)|![image](https://github.com/d-zaytsev/comp-math/assets/113129532/a6ffd105-07df-45ac-b5c5-6ca39ebc786c)|
|![image](https://github.com/d-zaytsev/comp-math/assets/113129532/2621b54c-235e-4c0e-9b98-aa31dddb736c)|![image](https://github.com/d-zaytsev/comp-math/assets/113129532/cbbaa11c-5b07-4a5d-86f2-55b3fa78041b)|

*For obvious reasons, curves are also poorly approximated by cubic splines.*
| $f(x) = frac(12*x/\pi)$ | $f(x) = frac(12*x/\pi)$|
|---|---|
|![image](https://github.com/d-zaytsev/comp-math/assets/113129532/e2ed49c9-9916-4726-b30c-b0b532b18d08)|![image](https://github.com/d-zaytsev/comp-math/assets/113129532/d5381daf-ac81-4bab-9cde-a66c08dbf142)|

### Quadratic B-Spline
*B-splines, even on simple functions, did not give as good a result as cubic splines.*
| $f(x) = -x^2 + x $  | $f(x) = sin(5*\pi*x)$  |
|---|---|
| ![image](https://github.com/d-zaytsev/comp-math/assets/113129532/a7ca700e-9dbd-46c5-aedb-d9f644baa1cd) | ![image](https://github.com/d-zaytsev/comp-math/assets/113129532/eff38008-9aa8-4af0-9048-501753b06b59)|

*It was also decided to repeat the experiment from the article (the link is below). The result was approximately similar.*
| $f(x) = \mid cos(3 \pi x) \mid $  |
|---|
| ![image](https://github.com/d-zaytsev/comp-math/assets/113129532/140cd5ad-16ff-41f1-8c82-8c255c826300) |
## Approximation estimate
During testing, the obtained results were evaluated using a special function. It splits the original function and the function obtained by approximation into nodes and checked the values in them. Thus, it was possible evaluate the success of the approximation.

## Materials
The following articles were used in preparing the experiments:
- [Approximation of discontinuous functions](https://arxiv.org/ftp/arxiv/papers/1601/1601.05132.pdf)
- [Quadratic B-Spline](https://core.ac.uk/download/pdf/82327690.pdf)
