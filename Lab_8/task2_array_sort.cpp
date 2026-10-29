#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include <omp.h>

static void mergeRanges(std::vector<int>& a, std::vector<int>& temp,
                        int left, int middle, int right) {
    int i = left, j = middle, k = left;
    while (i < middle && j < right)
        temp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    while (i < middle) temp[k++] = a[i++];
    while (j < right) temp[k++] = a[j++];
    for (int p = left; p < right; ++p) a[p] = temp[p];
}

static void parallelMergeSort(std::vector<int>& a, std::vector<int>& temp,
                              int left, int right, int cutoff) {
    if (right - left <= cutoff) {
        std::sort(a.begin() + left, a.begin() + right);
        return;
    }
    int middle = left + (right - left) / 2;
    #pragma omp task shared(a, temp) firstprivate(left, middle, cutoff)
    parallelMergeSort(a, temp, left, middle, cutoff);
    #pragma omp task shared(a, temp) firstprivate(middle, right, cutoff)
    parallelMergeSort(a, temp, middle, right, cutoff);
    #pragma omp taskwait
    mergeRanges(a, temp, left, middle, right);
}

int main(int argc, char* argv[]) {
    int n = argc > 1 ? std::stoi(argv[1]) : 200000;
    int threads = argc > 2 ? std::stoi(argv[2]) : 4;
    if (n <= 0 || threads <= 0) {
        std::cerr << "Usage: " << argv[0] << " [N>0] [threads>0]\n";
        return 1;
    }
    std::vector<int> original(n);
    std::mt19937 rng(527);
    std::uniform_int_distribution<int> dist(-1000000, 1000000);
    for (int& x : original) x = dist(rng);

    std::vector<int> serial = original;
    double start = omp_get_wtime();
    std::sort(serial.begin(), serial.end());
    double serialTime = omp_get_wtime() - start;

    std::vector<int> parallel = original, temp(n);
    start = omp_get_wtime();
    #pragma omp parallel num_threads(threads) shared(parallel, temp)
    {
        #pragma omp single
        parallelMergeSort(parallel, temp, 0, n, 2048);
    }
    double parallelTime = omp_get_wtime() - start;
    bool correct = serial == parallel;

    std::cout << "Task 2: Array Sorting (parallel merge sort)\nN: " << n
              << "\nThreads: " << threads << "\nCorrect: " << (correct ? "yes" : "no")
              << "\nSerial time: " << serialTime << " seconds"
              << "\nParallel time: " << parallelTime << " seconds\n";
    if (parallelTime > 0) {
        double speedup = serialTime / parallelTime;
        std::cout << "Speedup: " << speedup << "\nEfficiency: " << speedup / threads << "\n";
    }
    return correct ? 0 : 1;
}
