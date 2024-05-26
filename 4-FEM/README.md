# Finite Element Method
Implementation of constructing an approximate solution to the Sturm-Liouville problem using the finite element method

# Conditions
## Hardware/Software
- **OC**: Ubuntu 23.10 mantic x86_64;
- **PC**: VivoBook_ASUSLaptop X513EQN_S5;
- **CPU**: 11th Gen Intel i7-1165G7 (4 cores, 8 threads);
- **Python**: 3.12
- **Libraries**: specified in requirements.txt

## Math
- **Equation**: $y''-(\lambda+x) * y = -x * sin( x * \sqrt{\lambda})$
- **Interval**: $[0; n* \pi]$
- **Solution**: $y = sin( \sqrt{\lambda * x} )$
- $\lambda = \pi^2$

# Experiments
Experiments were carried out on grids of sizes 10, 100, 1000, 5000. Parameter n was equal to 1, 2, 7, 10. During the experiments, the error of approximation was measured.

| grid size / n| 1 | 2 | 7 | 10 |
| --- | :----: | :----: | :----: | :----: |
| 10         | 1.156 | 1.611 | 3.158 |  3.491 |
| 100       | -      | 1.461 | 2.45  |  2.646 |
| 1000      | -      | -     | 2.34  |  2.571 |
| 5000      | -      | -     | 2.34  |  2.57 |

As a result, we can conclude that the error depends on the mesh fineness.

## Materials
The following articles were used in preparing the task:
- [Calculation methods II](http://www.ict.nsc.ru/matmod/files/textbooks/KhakimzyanovCherny-2.pdf)
- [Description of the Sturm-Liouville problem](https://ru.wikipedia.org/wiki/%D0%97%D0%B0%D0%B4%D0%B0%D1%87%D0%B0_%D0%A8%D1%82%D1%83%D1%80%D0%BC%D0%B0_%E2%80%94_%D0%9B%D0%B8%D1%83%D0%B2%D0%B8%D0%BB%D0%BB%D1%8F)
