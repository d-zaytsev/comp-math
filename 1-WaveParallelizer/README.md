# WaveParallelizer
Implementation of the function approximation search algorithm (using C+openMP).
## CI/Performance test
- The results of algorithm testing are stored in artifacts;
- **Measurements**: measurements were carried out on different threads (1, 2, 4, 8, 16), average values and confidence intervals were also calculated;
- **Boundary problems**: The sin function at the boundaries was chosen to show how the approximation of such periodic functions will occur.
![experiment](https://github.com/d-zaytsev/comp-math/assets/113129532/7c546860-52eb-4ec0-8435-54efa46dabdd)

## Experiment conditions
- **OC**: Ubuntu 23.10 mantic x86_64;
- **PC**: VivoBook_ASUSLaptop X513EQN_S5;
- **CPU**: 11th Gen Intel i7-1165G7 (4 cores, 8 threads).

## Results
- Too many threads can harm performance (in my case, 16 threads work much slower than 8 due to amount of logic threads in my laptop). Initially, it was expected that as the number of threads increases, performance will only increase;
  
- Optimizing settings in compiler (*gcc -O3/-O2/-O1*) greatly affect performance (both for parallel and sequential algorithms, regardless of the number of threads). The book we have read don't say this;
- The complexity of boundary value problems can also affect performance (Complexity of functions at boundaries increases running time). In my case, when I use trigonometric functions performance could drop up to 5 times;
- The mesh size also significantly affects the speed of operation. When its size doubles, the speed drops tens of times;
- As it is written in the book, the quality of approximation is greatly influenced by epsilon, grid size and number of blocks.
