# CS527 Lab 8 — OpenMP: Arrays and Matrices

## Student Details

- **Name:** Abhijeet Damodar
- **Roll No.:** 2026AIM1001


## Overview

This lab explores OpenMP parallelism on array and matrix workloads. Each task was run with **1, 2, 4, and 8 threads**. The reported values below are the outputs collected from the local Windows environment.

## Build environment

- Compiler: GCC (`g++`) from MSYS2 UCRT64
- Build tool: `mingw32-make`
- Language standard: C++17
- OpenMP enabled with `-fopenmp`
- Build flags: `-O2 -Wall -Wextra -fopenmp -std=c++17`

First, add the MSYS2 UCRT64 and Unix utility directories to the current PowerShell session's `PATH`:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + $env:Path
```

Then build all tasks:

```powershell
mingw32-make
```

If your Makefile uses a different target or executable naming convention, follow the targets defined in that Makefile.

## Run commands

Run each executable with its input size and thread count:

```powershell
.\task1.exe 1000000 1
.\task1.exe 1000000 2
.\task1.exe 1000000 4
.\task1.exe 1000000 8

.\task2.exe 200000 1
.\task2.exe 200000 2
.\task2.exe 200000 4
.\task2.exe 200000 8

.\task3.exe 1000000 1
.\task3.exe 1000000 2
.\task3.exe 1000000 4
.\task3.exe 1000000 8

.\task4.exe 1024 1
.\task4.exe 1024 2
.\task4.exe 1024 4
.\task4.exe 1024 8

.\task5.exe 1024 1
.\task5.exe 1024 2
.\task5.exe 1024 4
.\task5.exe 1024 8
```

## Task 1 — Array Search

Input size: **1,000,000**  
Target: `1999998`  
Timing repetitions: **30**  
Correctness: **PASS** for all thread counts (serial index and parallel index were both `999999`).

| Threads | Serial time (s) | Parallel time (s) | Speedup |
|---:|---:|---:|---:|
| 1 | 0.001233331 | 0.000966668 | 1.275858100 |
| 2 | 0.000633335 | 0.000399995 | 1.583358169 |
| 4 | 0.000666666 | 0.000400003 | 1.666653421 |
| 8 | 0.000600004 | 0.000233332 | 2.571457766 |

The best measured speedup in these runs was approximately **2.57× with 8 threads**.

## Task 2 — Array Sorting (Parallel Merge Sort)

Input size: **200,000**  
Correctness: **PASS** for all thread counts.

| Threads | Serial time (s) | Parallel time (s) | Speedup | Efficiency |
|---:|---:|---:|---:|---:|
| 1 | 0.00999999 | 0.0120001 | 0.833327 | 0.833327 |
| 2 | 0.00999999 | 0.00699997 | 1.42858 | 0.714288 |
| 4 | 0.0110002 | 0.0369999 | 0.297302 | 0.0743255 |
| 8 | 0.00900006 | 0.0059998 | 1.50006 | 0.187507 |

The 4-thread run was slower than the serial run in this measurement. Parallel overhead and timing variability can affect small benchmark runs.

## Task 3 — Histogram (Values 0 Through 9)

Input size: **1,000,000**  
Timing repetitions: **20**  
Correctness: **PASS** for all thread counts. The serial and parallel counts matched for every bin.

Histogram counts (same for all four thread counts):

| Value | Serial count | Parallel count |
|---:|---:|---:|
| 0 | 99563 | 99563 |
| 1 | 100463 | 100463 |
| 2 | 100068 | 100068 |
| 3 | 99577 | 99577 |
| 4 | 100531 | 100531 |
| 5 | 99544 | 99544 |
| 6 | 100169 | 100169 |
| 7 | 99580 | 99580 |
| 8 | 100514 | 100514 |
| 9 | 99991 | 99991 |

| Threads | Serial time (s) | Parallel time (s) | Speedup | Efficiency |
|---:|---:|---:|---:|---:|
| 1 | 0.000400007 | 0.000349998 | 1.142881471 | 1.142881471 |
| 2 | 0.000549996 | 0.000750005 | 0.733322737 | 0.366661369 |
| 4 | 0.000750005 | 0.000800002 | 0.937504657 | 0.234376164 |
| 8 | 0.000500000 | 0.000699997 | 0.714288147 | 0.089286018 |

## Task 4 — Matrix Transpose

Matrix size: **1024 × 1024**  
Correctness: **PASS** for all thread counts.

| Threads | Serial time (s) | Parallel time (s) | Speedup | Efficiency |
|---:|---:|---:|---:|---:|
| 1 | 0.00800014 | 0.00800014 | 1 | 1 |
| 2 | 0.00500011 | 0.00699997 | 0.714305 | 0.357153 |
| 4 | 0.00699997 | 0.00600004 | 1.16665 | 0.291663 |
| 8 | 0.0119998 | 0.0079999 | 1.5 | 0.1875 |

## Task 5 — 2D Convolution (3×3 Averaging Filter)

Input image: **1024 × 1024**  
Output image: **1022 × 1022**  
Correctness: **PASS** for all thread counts. The top-left output sample was the same in all runs.

Top-left output sample:

```text
145.111 134.889 152
133 123.667 134.778
89.5556 97 119.556
```

| Threads | Serial time (s) | Parallel time (s) | Speedup | Efficiency |
|---:|---:|---:|---:|---:|
| 1 | 0.00200009 | 0.00200009 | 1 | 1 |
| 2 | 0.00199986 | 0.00300002 | 0.666614 | 0.333307 |
| 4 | 0.00399995 | 0.00200009 | 1.99988 | 0.49997 |
| 8 | 0.00200009 | 0.00300002 | 0.666693 | 0.0833366 |

## Notes on performance results

- All 20 runs reported `Correct: yes`.
- Task 1 reports speedup but does not print efficiency in the supplied output, so efficiency is not added for that task.
- Performance does not necessarily improve with every additional thread. OpenMP startup/synchronization overhead, memory access patterns, and normal timing noise can make a parallel run slower than its serial counterpart.
- These measurements are the actual outputs collected during this lab run on one machine. Very short execution times can vary between runs; repeat benchmarks under consistent conditions for more reliable comparisons.
- Efficiency values above are reproduced from the program output and have not been independently recalculated.

## Summary

| Task | Input | Correctness | Best observed result in these runs |
|---|---|---|---|
| Task 1 — Array Search | 1,000,000 elements | PASS, all thread counts | 2.571457766× speedup at 8 threads |
| Task 2 — Array Sorting | 200,000 elements | PASS, all thread counts | 1.50006× speedup at 8 threads |
| Task 3 — Histogram | 1,000,000 elements | PASS, all thread counts | 1.142881471× speedup at 1 thread |
| Task 4 — Matrix Transpose | 1024 × 1024 | PASS, all thread counts | 1.5× speedup at 8 threads |
| Task 5 — 2D Convolution | 1024 × 1024 input | PASS, all thread counts | 1.99988× speedup at 4 threads |
