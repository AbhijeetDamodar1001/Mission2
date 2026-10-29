#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include <omp.h>

using namespace std;

constexpr int HISTOGRAM_SIZE = 10;

using Histogram = array<long long, HISTOGRAM_SIZE>;

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

NOINLINE Histogram serialHistogram(const vector<int>& values) {
    Histogram counts{};
    for (int value : values) {
        ++counts[static_cast<size_t>(value)];
    }
    return counts;
}

NOINLINE Histogram parallelHistogram(const vector<int>& values, int threadCount) {
    // Each thread writes only to its own histogram, so there are no data races.
    vector<Histogram> localCounts(static_cast<size_t>(threadCount));
    for (auto& counts : localCounts) {
        counts.fill(0);
    }

    #pragma omp parallel num_threads(threadCount)
    {
        const int tid = omp_get_thread_num();
        Histogram& mine = localCounts[static_cast<size_t>(tid)];

        #pragma omp for schedule(static)
        for (long long i = 0; i < static_cast<long long>(values.size()); ++i) {
            ++mine[static_cast<size_t>(values[static_cast<size_t>(i)])];
        }
    }

    Histogram total{};
    for (int tid = 0; tid < threadCount; ++tid) {
        for (int value = 0; value < HISTOGRAM_SIZE; ++value) {
            total[static_cast<size_t>(value)] +=
                localCounts[static_cast<size_t>(tid)][static_cast<size_t>(value)];
        }
    }
    return total;
}

int main(int argc, char* argv[]) {
    long long n = 1'000'000;
    int threads = 4;

    if (argc >= 2) n = atoll(argv[1]);
    if (argc >= 3) threads = atoi(argv[2]);

    if (n <= 0 || threads <= 0) {
        cerr << "Usage: " << argv[0] << " [N>0] [threads>0]\n";
        return 1;
    }

    vector<int> values(static_cast<size_t>(n));
    mt19937 generator(527);
    uniform_int_distribution<int> distribution(0, 9);
    for (int& value : values) {
        value = distribution(generator);
    }

    const Histogram serialCounts = serialHistogram(values);
    const Histogram parallelCounts = parallelHistogram(values, threads);
    const bool correct = (serialCounts == parallelCounts);

    // Repeated runs make the measured duration large enough for useful timing.
    constexpr int repetitions = 20;
    volatile long long benchmarkSink = 0;

    const double serialStart = omp_get_wtime();
    for (int r = 0; r < repetitions; ++r) {
        const Histogram counts = serialHistogram(values);
        benchmarkSink = benchmarkSink + counts[static_cast<size_t>(r % HISTOGRAM_SIZE)];
    }
    const double serialTotal = omp_get_wtime() - serialStart;

    const double parallelStart = omp_get_wtime();
    for (int r = 0; r < repetitions; ++r) {
        const Histogram counts = parallelHistogram(values, threads);
        benchmarkSink = benchmarkSink + counts[static_cast<size_t>(r % HISTOGRAM_SIZE)];
    }
    const double parallelTotal = omp_get_wtime() - parallelStart;

    const double serialTime = serialTotal / repetitions;
    const double parallelTime = parallelTotal / repetitions;
    const double speedup = parallelTime > 0.0 ? serialTime / parallelTime : 0.0;
    const double efficiency = speedup / threads;

    cout << fixed << setprecision(9);
    cout << "Task 3: Histogram (values 0 through 9)\n";
    cout << "N: " << n << '\n';
    cout << "Threads: " << threads << '\n';
    cout << "Value | Serial | Parallel\n";
    for (int value = 0; value < HISTOGRAM_SIZE; ++value) {
        cout << value << " | "
             << serialCounts[static_cast<size_t>(value)] << " | "
             << parallelCounts[static_cast<size_t>(value)] << '\n';
    }
    cout << "Correct: " << (correct ? "yes" : "no") << '\n';
    cout << "Timing repetitions: " << repetitions << '\n';
    cout << "Serial time (average): " << serialTime << " seconds\n";
    cout << "Parallel time (average): " << parallelTime << " seconds\n";
    cout << "Speedup: " << speedup << '\n';
    cout << "Efficiency: " << efficiency << '\n';

    return correct ? 0 : 2;
}
