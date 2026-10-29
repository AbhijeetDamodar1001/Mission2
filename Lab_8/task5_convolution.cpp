#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <omp.h>

int main(int argc, char* argv[]) {
    int n = argc > 1 ? std::stoi(argv[1]) : 1024;
    int threads = argc > 2 ? std::stoi(argv[2]) : 4;
    if (n < 3 || threads <= 0) {
        std::cerr << "Usage: " << argv[0] << " [N>=3] [threads>0]\n";
        return 1;
    }

    std::vector<double> image(static_cast<size_t>(n) * n);
    std::mt19937 rng(527);
    std::uniform_int_distribution<int> dist(0, 255);
    for (double& pixel : image) pixel = dist(rng);

    int outputSize = n - 2;
    std::vector<double> serial(static_cast<size_t>(outputSize) * outputSize);
    std::vector<double> parallel(static_cast<size_t>(outputSize) * outputSize);

    // Assignment's 3x3 averaging filter: nine ones, with each sum divided by 9.
    double start = omp_get_wtime();
    for (int row = 0; row < outputSize; ++row) {
        for (int col = 0; col < outputSize; ++col) {
            double sum = 0;
            for (int kr = 0; kr < 3; ++kr)
                for (int kc = 0; kc < 3; ++kc)
                    sum += image[static_cast<size_t>(row + kr) * n + col + kc];
            serial[static_cast<size_t>(row) * outputSize + col] = sum / 9.0;
        }
    }
    double serialTime = omp_get_wtime() - start;

    start = omp_get_wtime();
    #pragma omp parallel for collapse(2) num_threads(threads)
    for (int row = 0; row < outputSize; ++row) {
        for (int col = 0; col < outputSize; ++col) {
            double sum = 0;
            for (int kr = 0; kr < 3; ++kr)
                for (int kc = 0; kc < 3; ++kc)
                    sum += image[static_cast<size_t>(row + kr) * n + col + kc];
            parallel[static_cast<size_t>(row) * outputSize + col] = sum / 9.0;
        }
    }
    double parallelTime = omp_get_wtime() - start;

    bool correct = true;
    for (size_t i = 0; i < serial.size(); ++i) {
        if (std::abs(serial[i] - parallel[i]) > 1e-9) { correct = false; break; }
    }

    std::cout << "Task 5: 2D Convolution (3x3 averaging filter)\nInput image: "
              << n << " x " << n << "\nOutput image: " << outputSize << " x " << outputSize
              << "\nThreads: " << threads << "\nCorrect: " << (correct ? "yes" : "no")
              << "\nSerial time: " << serialTime << " seconds"
              << "\nParallel time: " << parallelTime << " seconds\n";
    if (parallelTime > 0) {
        double speedup = serialTime / parallelTime;
        std::cout << "Speedup: " << speedup << "\nEfficiency: " << speedup / threads << "\n";
    }
    std::cout << "Top-left output sample:\n";
    for (int row = 0; row < std::min(outputSize, 3); ++row) {
        for (int col = 0; col < std::min(outputSize, 3); ++col)
            std::cout << parallel[static_cast<size_t>(row) * outputSize + col] << " ";
        std::cout << "\n";
    }
    return correct ? 0 : 1;
}
