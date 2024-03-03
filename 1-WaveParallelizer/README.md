# WaveParallelizer
Implementation of algorithms for solving partial differential equations. 1 sequential and 5 parallel algorithms are presented (using openMP).
## CI/Perfomance test
The results of testing these algorithms are stored in artifacts. The efficiency comparison was made by comparing the algorithms speed on different numbers of threads.
## Boundary value problem
The boundary value problem was chosen to show the ability of the algorithms to process complex functions at the boundaries of the domain.
## Results
- The parallel algorithm 11.2 turned out to be much slower than its sequential version (due to threads synchronization costs).
- Only parallel algorithms 11.5 and 11.6 were successful in terms of performance.
- The parallel algorithm 11.6 turned out to be the fastest (due to low thread competition and optimal use of the cache).
- [!] Increasing the number of threads increase the speed of parallel algorithms, but hyperthreading can harm performance (in my case, 16 threads work much slower than 8).
- In some parallel algorithms (11.4), to increase the speed, we need to use up a lot of memory.
- [!] Optimizing settings in compiler (gcc -O3/-O2/-O1) greatly affect perfomance (both for parallel and sequential algorithms, regardless of the number of threads)
- [!] The complexity of boundary value problems can also affect performance (Complexity of functions at boundaries increases running time).
