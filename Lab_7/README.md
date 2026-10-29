# CS527 Computer Systems — OpenMP Lab (C++)

This folder contains readable C++ implementations for all five tasks in the supplied lab document. Each program includes a serial implementation, the requested OpenMP version(s), a correctness check, and timing with `omp_get_wtime()`.

## Requirements

- A C++ compiler with OpenMP support (for example, GCC/g++).
- Linux, macOS with an OpenMP-enabled compiler, or Windows using WSL/MinGW with OpenMP support.

## Compile

Run these commands from this folder:

```bash
g++ -O2 -Wall -fopenmp task1_vector_add.cpp -o task1
g++ -O2 -Wall -fopenmp task2_array_sum.cpp -o task2
g++ -O2 -Wall -fopenmp task3_min_max.cpp -o task3
g++ -O2 -Wall -fopenmp task4_matrix_add.cpp -o task4
g++ -O2 -Wall -fopenmp task5_matrix_multiply.cpp -o task5
```

## Run

Each program takes optional command-line arguments: problem size, then thread count. If omitted, a manageable default is used.

```bash
./task1 1000000 4
./task2 1000000 4
./task3 1000000 4
./task4 512 4
./task5 128 4
```

To use the environment variable instead of passing a thread count, run for example:

```bash
export OMP_NUM_THREADS=4
./task1 1000000
```

If a thread count is supplied as the second argument, the program uses `omp_set_num_threads()` in code. State which method you used in your report.

## Task-specific notes

- **Task 1:** Tests serial and parallel vector addition and compares each output element exactly. The generated values make the additions exactly representable for this test.
- **Task 2:** The naive sum is intentionally incorrect/unsafe because all threads update the same `sum` without synchronization. Its output can vary and is not guaranteed to be wrong on every run. The reduction version is the correct implementation.
- **Task 3:** Includes `max` and `min` value reductions, critical-section index searches, and a manual per-thread index search. For ties, the serial and parallel index may differ if the same maximum/minimum appears more than once; this input intentionally places unique extreme values.
- **Task 4:** Stores each matrix in a flat `vector<double>` in row-major order. Compare outer-loop parallelization with `collapse(2)`.
- **Task 5:** Stores matrices in flat row-major vectors. Each `(i,j)` output cell is computed using a local `sum`; the `k` loop stays serial so threads do not race while accumulating into the same cell.

## Important resource note

The lab asks for very large sizes, including `N = 10^8` for vector addition and sum, and `N = 4096` for matrix addition. These can require substantial RAM and may take time. Start with the small defaults above, confirm correctness, and then increase sizes gradually. For Task 1 at `N = 10^8`, four double arrays alone use about 3.2 GB (not counting program overhead). If your computer does not have enough available memory, document the largest size you could run rather than allowing the system to freeze.

## Timing/report checklist

Record the output from each run in a table with problem size, thread count, serial time, parallel time, speedup, and (where requested) efficiency. Use measured values from your own computer; do not copy assumed timings. Explain that small tasks can be dominated by thread-startup/scheduling overhead, and speedup is limited by memory bandwidth and the amount of parallel work.

### Questions to answer

1. **Vector addition:** Parallel work may start helping at larger `N`; for small arrays, OpenMP overhead can exceed saved computation. Speedup usually is not linear because of overhead, memory bandwidth, and finite CPU cores.
2. **Array sum:** The naive version has a data race because multiple threads read-modify-write shared `sum` at once. `critical` can make updates correct, but serializes each update and is generally much slower than `reduction`, which uses private partial sums and combines them.
3. **Min/max:** A reduction on `best_idx` only finds the largest index, not the index associated with the largest value. The value and index must be kept as a pair. Critical sections serialize updates; per-thread local candidates usually reduce contention. `critical` may also choose an arbitrary index when values tie.
4. **Matrix addition:** With a very small outer dimension, only a few threads get rows; `collapse(2)` exposes all cells as iterations. For large matrices, the outer loop may already provide enough rows to keep threads busy.
5. **Matrix multiplication:** Each thread owns different `(i,j)` output cells, so parallelizing `i` or `(i,j)` is safe. Parallelizing `k` directly makes multiple threads update the same `C[i][j]` without synchronization. `collapse(2)` can help when `N` is small relative to thread count; for large `N`, the outer loop may already expose enough work. The local `sum` avoids repeated writes to `C` inside the innermost loop.

## Before submission

Compile with the requested flags and keep the terminal output for correctness evidence. Add your own measured timing tables and short observations to your report. Review and understand the code before submitting it as your individual work.
