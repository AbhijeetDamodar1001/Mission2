#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;

long long sumSerial(const vector<int>& a) {
    long long sum = 0;
    for (long long i = 0; i < (long long)a.size(); i++) sum += a[i];
    return sum;
}

long long sumNaive(const vector<int>& a) {
    long long sum = 0;
    #pragma omp parallel for
    for (long long i = 0; i < (long long)a.size(); i++) sum += a[i];
    return sum;
}

long long sumReduction(const vector<int>& a) {
    long long sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (long long i = 0; i < (long long)a.size(); i++) sum += a[i];
    return sum;
}

int main(int argc, char* argv[]) {
    long long n = (argc > 1) ? atoll(argv[1]) : 1000000;
    int threads = (argc > 2) ? atoi(argv[2]) : 4;
    if (n <= 0 || threads <= 0) { cerr << "Use a positive size and thread count.\n"; return 1; }
    omp_set_num_threads(threads);
    vector<int> a(n, 1);

    double start = omp_get_wtime();
    long long expected = sumSerial(a);
    double serialTime = omp_get_wtime() - start;

    start = omp_get_wtime();
    long long naive = sumNaive(a);
    double naiveTime = omp_get_wtime() - start;

    start = omp_get_wtime();
    long long reduced = sumReduction(a);
    double reductionTime = omp_get_wtime() - start;

    cout << "N: " << n << "\nThreads: " << threads << "\nSerial sum: " << expected
         << "\nNaive parallel sum (data race; result is not reliable): " << naive
         << "\nReduction sum: " << reduced << "\nReduction correct: " << (reduced == expected ? "yes" : "no")
         << "\nSerial time: " << serialTime << " seconds\nNaive time: " << naiveTime
         << " seconds\nReduction time: " << reductionTime << " seconds\n";
    return reduced == expected ? 0 : 1;
}
