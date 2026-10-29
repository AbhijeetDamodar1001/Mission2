# CS527 Computer Systems Lab 9 — OpenMP

**Student:** Abhijeet Damodar  
**Roll No.:** 2026AIM1001

## Overview

This project implements and measures three OpenMP exercises:

1. **Recursive merge sort:** serial baseline versus task-based parallel merge sort.
2. **Uneven workload scheduling:** compare `static`, `dynamic` (chunk size 100), and `guided` scheduling.
3. **Even/odd counting:** compare serial counting, OpenMP `reduction`, and OpenMP `atomic`.

The results below are transcribed from the runs supplied for this lab. Execution times are machine- and load-dependent; repeat runs can differ.

## Project structure

```text
CS527_Lab9_OpenMP/
├── Makefile
├── README.md
├── build_windows.bat
└── src/
    ├── task1_merge_sort.cpp
    ├── task2_scheduling.cpp
    └── task3_even_odd.cpp
```

## Build and run (Windows / MSYS2 UCRT64)

Open PowerShell in the project root (the folder containing `Makefile` and `src`).

Add the MSYS2 UCRT64 tools to the current terminal's PATH:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + $env:Path
```

Compile the programs:

```powershell
mingw32-make
```

The build creates executables in `bin`. If the build command is not found, check that MSYS2 is installed and that `C:\msys64\ucrt64\bin` is present in PATH.

### Individual command formats

```powershell
.\bin\task1.exe <N> <threads>
.\bin\task2.exe <N> <threads>
.\bin\task3.exe <N> <threads>
```

For Task 1, an optional third argument can set the task cutoff; the default used in the recorded runs was 2048.

To run each test, enter the command for the task with the required input size (`N`) and thread count. For example:

```powershell
.\bin\task1.exe 100000 1
.\bin\task2.exe 100000 1
.\bin\task3.exe 100000 1
```

Change `N` and the thread count to run the other required test cases.

## Task 1 — Recursive merge sort with OpenMP tasks

Each run reported `Sorted: yes`, `Matches serial output: yes`, and `Correct: yes`.

| N | Threads | Serial time (s) | Parallel time (s) | Speedup | Efficiency |
|---:|---:|---:|---:|---:|---:|
| 100,000 | 1 | 0.00200009 | 0.00300002 | 0.666693 | 0.666693 |
| 100,000 | 2 | 0.00200009 | 0.00200009 | 1.000000 | 0.500000 |
| 100,000 | 4 | 0.00300002 | 0.00200009 | 1.499940 | 0.374985 |
| 100,000 | 8 | 0.00200009 | 0.00200009 | 1.000000 | 0.125000 |
| 1,000,000 | 1 | 0.0260000 | 0.0250001 | 1.040000 | 1.040000 |
| 1,000,000 | 2 | 0.0280001 | 0.0149999 | 1.866690 | 0.933346 |
| 1,000,000 | 4 | 0.0270000 | 0.0109999 | 2.454560 | 0.613640 |
| 1,000,000 | 8 | 0.0260000 | 0.00999999 | 2.600000 | 0.325001 |
| 10,000,000 | 1 | 0.307000 | 0.333000 | 0.921922 | 0.921922 |
| 10,000,000 | 2 | 0.326000 | 0.203000 | 1.605910 | 0.802955 |
| 10,000,000 | 4 | 0.331000 | 0.148000 | 2.236490 | 0.559122 |
| 10,000,000 | 8 | 0.337000 | 0.145000 | 2.324140 | 0.290517 |

### Task 1 observations

- All runs produced a sorted result matching the serial output.
- At \(N=10^6\), the best measured parallel time was **0.00999999 s with 8 threads**, with speedup **2.60×**.
- At \(N=10^7\), 8 threads took **0.145 s**, compared with **0.337 s** in the recorded serial baseline.
- Parallel overhead can dominate for smaller inputs. The 1-thread parallel run is not guaranteed to match the serial baseline because it still incurs task/runtime overhead and the timing is noisy.
- Efficiency generally decreases as thread count rises because speedup does not increase linearly with the number of threads.

## Task 2 — OpenMP scheduling strategies

Configuration: work scale = 1; dynamic chunk size = 100. Every strategy in all 12 supplied runs reported `Correct: yes`.

Each table row contains the measured parallel time, speedup, and efficiency for one scheduling strategy. The serial baseline is shown once per input/thread run.

### N = 100,000

| Threads | Serial time (s) | Strategy | Parallel time (s) | Speedup | Efficiency |
|---:|---:|---|---:|---:|---:|
| 1 | 0.0850000381 | Static | 0.0850000381 | 1.000000 | 1.000000 |
| 1 | 0.0850000381 | Dynamic, chunk=100 | 0.0840001106 | 1.011904 | 1.011904 |
| 1 | 0.0850000381 | Guided | 0.0839998722 | 1.011907 | 1.011907 |
| 2 | 0.0850000381 | Static | 0.0439999104 | 1.931823 | 0.965911 |
| 2 | 0.0850000381 | Dynamic, chunk=100 | 0.0429999828 | 1.976746 | 0.988373 |
| 2 | 0.0850000381 | Guided | 0.0429999828 | 1.976746 | 0.988373 |
| 4 | 0.112999916 | Static | 0.0239999294 | 4.708344 | 1.177086 |
| 4 | 0.112999916 | Dynamic, chunk=100 | 0.0230000019 | 4.913039 | 1.228260 |
| 4 | 0.112999916 | Guided | 0.0220000744 | 5.136342 | 1.284086 |
| 8 | 0.0850000381 | Static | 0.013999939 | 6.071458 | 0.758932 |
| 8 | 0.0850000381 | Dynamic, chunk=100 | 0.0120000839 | 7.083287 | 0.885411 |
| 8 | 0.0850000381 | Guided | 0.0130000114 | 6.538459 | 0.817307 |

### N = 1,000,000

| Threads | Serial time (s) | Strategy | Parallel time (s) | Speedup | Efficiency |
|---:|---:|---|---:|---:|---:|
| 1 | 0.842000008 | Static | 0.835999966 | 1.007177 | 1.007177 |
| 1 | 0.842000008 | Dynamic, chunk=100 | 0.838000059 | 1.004773 | 1.004773 |
| 1 | 0.842000008 | Guided | 0.835000038 | 1.008383 | 1.008383 |
| 2 | 0.835999966 | Static | 0.425000191 | 1.967058 | 0.983529 |
| 2 | 0.835999966 | Dynamic, chunk=100 | 0.419000149 | 1.995226 | 0.997613 |
| 2 | 0.835999966 | Guided | 0.421000004 | 1.985748 | 0.992874 |
| 4 | 0.836999893 | Static | 0.224000216 | 3.736603 | 0.934151 |
| 4 | 0.836999893 | Dynamic, chunk=100 | 0.222000122 | 3.770268 | 0.942567 |
| 4 | 0.836999893 | Guided | 0.220999956 | 3.787331 | 0.946833 |
| 8 | 0.839999914 | Static | 0.164999962 | 5.090910 | 0.636364 |
| 8 | 0.839999914 | Dynamic, chunk=100 | 0.116000175 | 7.241368 | 0.905171 |
| 8 | 0.839999914 | Guided | 0.118000031 | 7.118642 | 0.889830 |

### N = 10,000,000

| Threads | Serial time (s) | Strategy | Parallel time (s) | Speedup | Efficiency |
|---:|---:|---|---:|---:|---:|
| 1 | 8.41699982 | Static | 8.42299986 | 0.999288 | 0.999288 |
| 1 | 8.41699982 | Dynamic, chunk=100 | 8.71500015 | 0.965806 | 0.965806 |
| 1 | 8.41699982 | Guided | 8.37699986 | 1.004775 | 1.004775 |
| 2 | 8.37800002 | Static | 4.24099994 | 1.975478 | 0.987739 |
| 2 | 8.37800002 | Dynamic, chunk=100 | 4.27399993 | 1.960225 | 0.980112 |
| 2 | 8.37800002 | Guided | 4.20599985 | 1.991916 | 0.995958 |
| 4 | 8.38199997 | Static | 2.23699999 | 3.746983 | 0.936746 |
| 4 | 8.38199997 | Dynamic, chunk=100 | 2.21499991 | 3.784199 | 0.946050 |
| 4 | 8.38199997 | Guided | 2.24699998 | 3.730307 | 0.932577 |
| 8 | 8.3900001 | Static | 1.16700006 | 7.189374 | 0.898672 |
| 8 | 8.3900001 | Dynamic, chunk=100 | 1.16700006 | 7.189374 | 0.898672 |
| 8 | 8.3900001 | Guided | 1.24199986 | 6.755234 | 0.844404 |

### Task 2 observations

- All three scheduling strategies produced correct results in every recorded run.
- For \(N=100{,}000\), 8 threads with dynamic scheduling had the shortest measured time: **0.0120000839 s**.
- For \(N=1{,}000{,}000\), 8-thread dynamic scheduling was fastest at **0.116000175 s** (speedup **7.241368×**).
- For \(N=10{,}000{,}000\), static and dynamic scheduling tied at **1.16700006 s** with 8 threads (speedup **7.189374×**).
- Static scheduling assigns chunks predictably with relatively low scheduling overhead. Dynamic scheduling assigns chunks as threads become available and can help balance uneven workloads, but has more scheduling overhead. Guided scheduling starts with larger chunks and gradually reduces chunk sizes, balancing overhead and load distribution.
- No strategy wins for every input size and thread count. The result depends on workload imbalance, scheduling overhead, thread count, and system conditions.

## Task 3 — Even/odd counting: reduction versus atomic

All runs reported the expected counts and `Count invariant (even + odd == N): yes`. For all tested even values of \(N\), the counts were \(N/2\) even and \(N/2\) odd.

**Timing note:** the supplied program output rounds some short durations to `0 seconds`. Where the measured serial time is zero, the program prints `nan` speedup/efficiency for reduction or `0` for atomic. These values are preserved as reported; they are not meaningful performance ratios because the timer resolution is too coarse for that run.

| N | Threads | Serial time (s) | Reduction time (s) | Reduction speedup | Reduction efficiency | Atomic time (s) | Atomic speedup | Atomic efficiency |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 100,000 | 1 | 0 | 0 | nan | nan | 0.00100017 | 0 | 0 |
| 100,000 | 2 | 0 | 0 | nan | nan | 0.00299978 | 0 | 0 |
| 100,000 | 4 | 0.000999928 | 0.00100017 | 0.999762 | 0.249940 | 0.00199986 | 0.500000 | 0.125000 |
| 100,000 | 8 | 0 | 0.000999928 | 0 | 0 | 0.00200009 | 0 | 0 |
| 1,000,000 | 1 | 0 | 0.000999928 | 0 | 0 | 0.00399995 | 0 | 0 |
| 1,000,000 | 2 | 0 | 0 | nan | nan | 0.00999999 | 0 | 0 |
| 1,000,000 | 4 | 0.00100017 | 0.000999928 | 1.000240 | 0.250060 | 0.0109999 | 0.090925 | 0.022731 |
| 1,000,000 | 8 | 0.000999928 | 0.000999928 | 1.000000 | 0.125000 | 0.0140002 | 0.071423 | 0.008928 |
| 10,000,000 | 1 | 0.00699997 | 0.0190001 | 0.368418 | 0.368418 | 0.0479999 | 0.145833 | 0.145833 |
| 10,000,000 | 2 | 0.00399995 | 0.00300002 | 1.333310 | 0.666653 | 0.224000 | 0.017857 | 0.008928 |
| 10,000,000 | 4 | 0.00399995 | 0.00300002 | 1.333310 | 0.333327 | 0.262000 | 0.015267 | 0.003817 |
| 10,000,000 | 8 | 0.00399995 | 0.00300002 | 1.333310 | 0.166663 | 0.219000 | 0.018265 | 0.002283 |
| 100,000,000 | 1 | 0.0389998 | 0.039000 | 0.999994 | 0.999994 | 0.420000 | 0.092857 | 0.092857 |
| 100,000,000 | 2 | 0.052000 | 0.030000 | 1.733340 | 0.866668 | 2.406000 | 0.021613 | 0.010806 |
| 100,000,000 | 4 | 0.049000 | 0.027000 | 1.814820 | 0.453705 | 3.365000 | 0.014562 | 0.003640 |
| 100,000,000 | 8 | 0.100000 | 0.0219998 | 4.545490 | 0.568187 | 2.191000 | 0.045641 | 0.005705 |

### Task 3 correctness counts

| N | Expected even | Expected odd | Result |
|---:|---:|---:|---|
| 100,000 | 50,000 | 50,000 | All supplied runs correct |
| 1,000,000 | 500,000 | 500,000 | All supplied runs correct |
| 10,000,000 | 5,000,000 | 5,000,000 | All supplied runs correct |
| 100,000,000 | 50,000,000 | 50,000,000 | All supplied runs correct |

### Task 3 observations

- `reduction` allows each thread to accumulate private partial counts and combines them at the end, avoiding a shared counter update for every element.
- `atomic` protects updates to shared counters, preserving correctness, but frequent atomic updates can serialize threads and create contention.
- For \(N=100{,}000{,}000\) and 8 threads, reduction took **0.0219998 s**, compared with **2.191 s** for atomic in the supplied run. Reduction was much faster in this case.
- Very short timings are below or near the timer's effective resolution, so the smallest-input speedup and efficiency values should not be over-interpreted.

## Formulas

- **Speedup:** \(S = T_{serial}/T_{parallel}\)
- **Efficiency:** \(E = S/P\), where \(P\) is the number of threads.

A speedup above 1 indicates that the measured parallel version was faster than the serial baseline for that run. Efficiency above 1 can appear in noisy measurements when the parallel timing is slightly lower than the recorded baseline; it does not mean that the processor has exceeded a physical limit.


## Report questions and answers

### Task 1 — Recursive merge sort

**1. What is the difference between OpenMP tasks and `parallel for`?**  
`parallel for` splits loop iterations between threads, while tasks let the program create independent units of work. Tasks fit recursive merge sort because the left and right halves are created dynamically as the recursion runs.

**2. Why is `taskwait` required before merging the two halves?**  
The merge step needs both halves to be sorted first. `taskwait` makes the parent task wait for its child tasks, so merging does not start while either half is still being sorted.

**3. How does the cutoff threshold affect performance?**  
The cutoff keeps small subarrays on the serial path instead of creating a task for every recursive call. If the cutoff is too small, task overhead can grow; if it is too large, there may be less parallel work available.

**4. Why can parallel merge sort be slower than serial merge sort for small arrays?**  
For small arrays, sorting itself takes very little time, so creating tasks and managing threads can cost more than the work saved. Parallelism is more useful when the array is large enough to offset that overhead.

**5. How does task creation overhead affect scalability?**  
Creating and scheduling tasks uses time and runtime resources. If too many tiny tasks are created, that overhead can limit speedup even when more threads are available.

### Task 2 — OpenMP scheduling strategies

**1. What is the difference between static, dynamic, and guided scheduling?**  
Static scheduling assigns iterations to threads ahead of time. Dynamic scheduling gives threads more chunks as they finish their current work, while guided scheduling starts with larger chunks and gradually makes them smaller.

**2. Which scheduling strategy works best for uniform workloads and uneven workloads?**  
Static scheduling is often a good choice when each iteration takes about the same time because its scheduling overhead is low. For uneven workloads, dynamic or guided scheduling can balance work better; in our measurements, the fastest strategy changed with input size and thread count.

**3. Why can dynamic scheduling introduce extra overhead?**  
Threads need to request and receive new chunks while the loop is running. This scheduling work adds overhead compared with assigning all chunks at the start.

**4. How does chunk size affect performance?**  
Small chunks can spread uneven work more evenly, but they require more scheduling operations. Large chunks reduce scheduling overhead, though they can leave some threads waiting if one chunk takes much longer than the others.

### Task 3 — Even/odd counting

**1. How can multiple variables be included in a reduction clause?**  
List each variable after the same reduction operator, separated by commas. For example, `reduction(+:even_count, odd_count)` creates private counters and combines their partial sums at the end.

**2. Why is reduction generally faster than atomic updates?**  
With reduction, each thread can count locally and combine its result at the end. Atomic updates repeatedly coordinate access to shared counters, which can cause contention when many threads update them frequently.

**3. Why must the sum of even and odd counts equal N?**  
Every array element is either even or odd, so each element contributes to exactly one of the two counters. Therefore, adding both counts must give the total number of elements, `N`.

**4. Does increasing the number of threads always improve performance?**  
No. More threads can add scheduling and synchronization overhead, and they share the machine's available resources. The measured results also show that atomic counting can become much slower because of contention, while very short runs are difficult to time accurately.
