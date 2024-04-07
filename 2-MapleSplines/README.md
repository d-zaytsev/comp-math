# Implementation of cubic and B-splines in Maple
## Conditions and goals
Experiments were carried out on standard functions from Maple and on my functions. The main goal was to understand which functions are most suitable for approximating with presented splines and how my implementation differs from the implementation in Maple.

## Approximation estimate
During testing, the obtained results were evaluated using a special function. It splits the original function and the function obtained by approximation into nodes and checked the values in them. Thus, it was possible evaluate the success of the approximation.

### Materials
The following articles were used in preparing the experiments:
- [Approximation of discontinuous functions] (https://arxiv.org/ftp/arxiv/papers/1601/1601.05132.pdf)
