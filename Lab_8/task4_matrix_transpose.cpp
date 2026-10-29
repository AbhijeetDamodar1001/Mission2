#include <cmath>
#include <iostream>
#include <vector>
#include <omp.h>

int main(int argc, char* argv[]) {
    int n = argc > 1 ? std::stoi(argv[1]) : 1024;
    int threads = argc > 2 ? std::stoi(argv[2]) : 4;
    if (n <= 0 || threads <= 0) {
        std::cerr << "Usage: " << argv[0] << " [N>0] [threads>0]\n";
        return 1;
    }

    std::vector<double> matrix(static_cast<size_t>(n) * n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            matrix[static_cast<size_t>(i) * n + j] = i * 1.0 + j * 0.01;

    std::vector<double> serial(static_cast<size_t>(n) * n);
    double start = omp_get_wtime();
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            serial[static_cast<size_t>(j) * n + i] = matrix[static_cast<size_t>(i) * n + j];
    double serialTime = omp_get_wtime() - start;

    std::vector<double> parallel(static_cast<size_t>(n) * n);
    start = omp_get_wtime();
    #pragma omp parallel for collapse(2) num_threads(threads)
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            parallel[static_cast<size_t>(j) * n + i] = matrix[static_cast<size_t>(i) * n + j];
    double parallelTime = omp_get_wtime() - start;

    bool correct = serial == parallel;
    std::cout << "Task 4: Matrix Transpose\nMatrix size: " << n << " x " << n
              << "\nThreads: " << threads << "\nCorrect: " << (correct ? "yes" : "no")
              << "\nSerial time: " << serialTime << " seconds"
              << "\nParallel time: " << parallelTime << " seconds\n";
    if (parallelTime > 0) {
        double speedup = serialTime / parallelTime;
        std::cout << "Speedup: " << speedup << "\nEfficiency: " << speedup / threads << "\n";
    }
    return correct ? 0 : 1;
}
