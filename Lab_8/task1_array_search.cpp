#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>
#include <omp.h>

using namespace std;

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

// Returns the first matching index, or -1 if the target is absent.
NOINLINE long long serialSearch(const vector<long long>& values, long long target) {
    for (long long i = 0; i < static_cast<long long>(values.size()); ++i) {
        if (values[static_cast<size_t>(i)] == target) {
            return i;
        }
    }
    return -1;
}

NOINLINE long long parallelSearch(const vector<long long>& values,
                                  long long target,
                                  int threadCount) {
    const long long n = static_cast<long long>(values.size());
    long long firstIndex = n; // reduction identity for min

    #pragma omp parallel for num_threads(threadCount) reduction(min:firstIndex) schedule(static)
    for (long long i = 0; i < n; ++i) {
        if (values[static_cast<size_t>(i)] == target && i < firstIndex) {
            firstIndex = i;
        }
    }

    return firstIndex == n ? -1 : firstIndex;
}

int main(int argc, char* argv[]) {
    long long n = 1'000'000;
    int threads = 4;

    if (argc >= 2) n = atoll(argv[1]);
    if (argc >= 3) threads = atoi(argv[2]);

    if (n <= 0 || threads <= 0) {
        cerr << "Usage: " << argv[0] << " [N>0] [threads>0] [target]\n";
        return 1;
    }

    vector<long long> values(static_cast<size_t>(n));
    for (long long i = 0; i < n; ++i) {
        values[static_cast<size_t>(i)] = i * 2;
    }

    long long target = values.back();
    if (argc >= 4) target = atoll(argv[3]);

    const long long serialIndex = serialSearch(values, target);
    const long long parallelIndex = parallelSearch(values, target, threads);
    const bool correct = (serialIndex == parallelIndex);

    // Repeat each operation so timer resolution does not round a tiny run to 0.
    // The volatile sink ensures every measured result is consumed.
    constexpr int repetitions = 30;
    volatile long long benchmarkSink = 0;

    const double serialStart = omp_get_wtime();
    for (int r = 0; r < repetitions; ++r) {
        benchmarkSink = benchmarkSink + serialSearch(values, target);
    }
    const double serialTotal = omp_get_wtime() - serialStart;

    const double parallelStart = omp_get_wtime();
    for (int r = 0; r < repetitions; ++r) {
        benchmarkSink = benchmarkSink + parallelSearch(values, target, threads);
    }
    const double parallelTotal = omp_get_wtime() - parallelStart;

    const double serialTime = serialTotal / repetitions;
    const double parallelTime = parallelTotal / repetitions;
    const double speedup = parallelTime > 0.0 ? serialTime / parallelTime : 0.0;

    cout << fixed << setprecision(9);
    cout << "Task 1: Array Search\n";
    cout << "N: " << n << '\n';
    cout << "Threads: " << threads << '\n';
    cout << "Target: " << target << '\n';
    cout << "Serial index: " << serialIndex << '\n';
    cout << "Parallel index: " << parallelIndex << '\n';
    cout << "Found: " << (serialIndex != -1 ? "yes" : "no") << '\n';
    cout << "Correct: " << (correct ? "yes" : "no") << '\n';
    cout << "Timing repetitions: " << repetitions << '\n';
    cout << "Serial time (average): " << serialTime << " seconds\n";
    cout << "Parallel time (average): " << parallelTime << " seconds\n";
    cout << "Speedup: " << speedup << '\n';

    return correct ? 0 : 2;
}
