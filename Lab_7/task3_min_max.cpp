#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;

void serialMinMax(const vector<int>& a, int& maxVal, long long& maxIdx, int& minVal, long long& minIdx) {
    maxVal = minVal = a[0]; maxIdx = minIdx = 0;
    for (long long i = 1; i < (long long)a.size(); i++) {
        if (a[i] > maxVal) { maxVal = a[i]; maxIdx = i; }
        if (a[i] < minVal) { minVal = a[i]; minIdx = i; }
    }
}

int parallelMax(const vector<int>& a) {
    int value = a[0];
    #pragma omp parallel for reduction(max:value)
    for (long long i = 0; i < (long long)a.size(); i++) if (a[i] > value) value = a[i];
    return value;
}

int parallelMin(const vector<int>& a) {
    int value = a[0];
    #pragma omp parallel for reduction(min:value)
    for (long long i = 0; i < (long long)a.size(); i++) if (a[i] < value) value = a[i];
    return value;
}

void maxIndexCritical(const vector<int>& a, int& bestVal, long long& bestIdx) {
    bestVal = a[0]; bestIdx = 0;
    #pragma omp parallel for
    for (long long i = 1; i < (long long)a.size(); i++) {
        #pragma omp critical
        {
            if (a[i] > bestVal) { bestVal = a[i]; bestIdx = i; }
        }
    }
}

void minIndexCritical(const vector<int>& a, int& bestVal, long long& bestIdx) {
    bestVal = a[0]; bestIdx = 0;
    #pragma omp parallel for
    for (long long i = 1; i < (long long)a.size(); i++) {
        #pragma omp critical
        {
            if (a[i] < bestVal) { bestVal = a[i]; bestIdx = i; }
        }
    }
}

void maxIndexManual(const vector<int>& a, int& bestVal, long long& bestIdx) {
    int threads = omp_get_max_threads();
    vector<int> values(threads, a[0]);
    vector<long long> indices(threads, 0);
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        long long size = (long long)a.size();
        long long begin = size * tid / threads;
        long long end = size * (tid + 1) / threads;
        int localVal = a[begin]; long long localIdx = begin;
        for (long long i = begin + 1; i < end; i++) {
            if (a[i] > localVal) { localVal = a[i]; localIdx = i; }
        }
        values[tid] = localVal; indices[tid] = localIdx;
    }
    bestVal = values[0]; bestIdx = indices[0];
    for (int t = 1; t < threads; t++) if (values[t] > bestVal) { bestVal = values[t]; bestIdx = indices[t]; }
}

void minIndexManual(const vector<int>& a, int& bestVal, long long& bestIdx) {
    int threads = omp_get_max_threads();
    vector<int> values(threads, a[0]);
    vector<long long> indices(threads, 0);
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        long long size = (long long)a.size();
        long long begin = size * tid / threads;
        long long end = size * (tid + 1) / threads;
        int localVal = a[begin]; long long localIdx = begin;
        for (long long i = begin + 1; i < end; i++) {
            if (a[i] < localVal) { localVal = a[i]; localIdx = i; }
        }
        values[tid] = localVal; indices[tid] = localIdx;
    }
    bestVal = values[0]; bestIdx = indices[0];
    for (int t = 1; t < threads; t++) if (values[t] < bestVal) { bestVal = values[t]; bestIdx = indices[t]; }
}

int main(int argc, char* argv[]) {
    long long n = (argc > 1) ? atoll(argv[1]) : 1000000;
    int threads = (argc > 2) ? atoi(argv[2]) : 4;
    if (n <= 0 || threads <= 0) { cerr << "Use a positive size and thread count.\n"; return 1; }
    omp_set_num_threads(threads);
    vector<int> a(n);
    for (long long i = 0; i < n; i++) a[i] = (int)((i * 37 + 19) % 1000003) - 500001;
    a[n / 3] = 2000000; a[(2 * n) / 3] = -2000000;

    int serialMax, serialMin; long long serialMaxIdx, serialMinIdx;
    double start = omp_get_wtime();
    serialMinMax(a, serialMax, serialMaxIdx, serialMin, serialMinIdx);
    double serialTime = omp_get_wtime() - start;

    start = omp_get_wtime(); int redMax = parallelMax(a); int redMin = parallelMin(a);
    double valueTime = omp_get_wtime() - start;
    int maxC, minC; long long maxCi, minCi;
    start = omp_get_wtime(); maxIndexCritical(a, maxC, maxCi); minIndexCritical(a, minC, minCi);
    double criticalTime = omp_get_wtime() - start;
    int maxM, minM; long long maxMi, minMi;
    start = omp_get_wtime(); maxIndexManual(a, maxM, maxMi); minIndexManual(a, minM, minMi);
    double manualTime = omp_get_wtime() - start;

    cout << "N: " << n << "\nThreads: " << threads << "\nSerial max: " << serialMax << " at index " << serialMaxIdx
         << "\nSerial min: " << serialMin << " at index " << serialMinIdx
         << "\nReduction max/min correct: " << ((redMax == serialMax && redMin == serialMin) ? "yes" : "no")
         << "\nCritical max/index correct: " << ((maxC == serialMax && maxCi == serialMaxIdx && minC == serialMin && minCi == serialMinIdx) ? "yes" : "no")
         << "\nManual max/index correct: " << ((maxM == serialMax && maxMi == serialMaxIdx && minM == serialMin && minMi == serialMinIdx) ? "yes" : "no")
         << "\nSerial time (values and indices): " << serialTime << " seconds\nReduction values time: " << valueTime
         << " seconds\nCritical indices time: " << criticalTime << " seconds\nManual indices time: " << manualTime << " seconds\n";
    return 0;
}
