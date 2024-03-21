# WaveParallelizer
Implementation of algorithms for solving partial differential equations. 1 sequential and 5 parallel algorithms are presented (using openMP).
## CI/Performance test
The results of testing these algorithms are stored in artifacts. The efficiency comparison was made by comparing the algorithms speed on different numbers of threads.
## Boundary value problem
The boundary value problem was chosen to show the ability of the algorithms to process complex functions at the boundaries of the domain.
## Experiment conditions
- **OC**: Ubuntu 23.10 x86_64
- **PC**: VivoBook_ASUSLaptop X513EQN_S5
- **CPU**: 11th Gen Intel i7-1165G7 (4 cores, 8 threads)
- **For comparison** I use conditions from the book and my own conditions (you can find them in *trig_cond()*) 
## Results
- Too many threads can harm performance (in my case, 16 threads work much slower than 8 due to amount of logic threads in my laptop).
- Optimizing settings in compiler (*gcc -O3/-O2/-O1*) greatly affect performance (both for parallel and sequential algorithms, regardless of the number of threads)
- The complexity of boundary value problems can also affect performance (Complexity of functions at boundaries increases running time). In my case, when I use trigonometric functions performance could drop up to 5 times.
- The mesh size also significantly affects the speed of operation. When its size doubles, the speed drops tens of times.
