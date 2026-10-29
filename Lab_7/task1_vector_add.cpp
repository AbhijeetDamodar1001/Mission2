#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;

void addSerial(const vector<double>& a, const vector<double>& b, vector<double>& c) {
    for (long long i = 0; i < (long long)a.size(); i++) c[i] = a[i] + b[i];
}

void addParallel(const vector<double>& a, const vector<double>& b, vector<double>& c) {
    #pragma omp parallel for
    for (long long i = 0; i < (long long)a.size(); i++) c[i] = a[i] + b[i];
}

int main(int argc, char* argv[]) {
    long long n = (argc > 1) ? atoll(argv[1]) : 1000000;
    int threads = (argc > 2) ? atoi(argv[2]) : 4;
    if (n <= 0 || threads <= 0) { cerr << "Use a positive size and thread count.\n"; return 1; }
    omp_set_num_threads(threads);

    vector<double> a(n), b(n), serialResult(n), parallelResult(n);
    for (long long i = 0; i < n; i++) { a[i] = (i % 100) * 0.5; b[i] = (i % 50) * 0.25; }

    double start = omp_get_wtime();
    addSerial(a, b, serialResult);
    double serialTime = omp_get_wtime() - start;

    start = omp_get_wtime();
    addParallel(a, b, parallelResult);
    double parallelTime = omp_get_wtime() - start;

    bool correct = true;
    for (long long i = 0; i < n; i++) if (serialResult[i] != parallelResult[i]) { correct = false; break; }
    cout << "N: " << n << "\nThreads: " << threads << "\nCorrect: " << (correct ? "yes" : "no")
         << "\nSerial time: " << serialTime << " seconds\nParallel time: " << parallelTime
         << " seconds\nSpeedup: " << serialTime / parallelTime << "\n";
    return correct ? 0 : 1;
}
