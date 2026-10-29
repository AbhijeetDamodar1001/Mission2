#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <omp.h>
using namespace std;

void addSerial(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) c[(long long)i*n+j] = a[(long long)i*n+j] + b[(long long)i*n+j];
}
void addOuter(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) c[(long long)i*n+j] = a[(long long)i*n+j] + b[(long long)i*n+j];
}
void addCollapse(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) c[(long long)i*n+j] = a[(long long)i*n+j] + b[(long long)i*n+j];
}
bool same(const vector<double>& a, const vector<double>& b) {
    for (size_t i = 0; i < a.size(); i++) if (a[i] != b[i]) return false;
    return true;
}
int main(int argc, char* argv[]) {
    int n = (argc > 1) ? atoi(argv[1]) : 512;
    int threads = (argc > 2) ? atoi(argv[2]) : 4;
    if (n <= 0 || threads <= 0) { cerr << "Use a positive matrix size and thread count.\n"; return 1; }
    omp_set_num_threads(threads);
    long long count = (long long)n * n;
    vector<double> a(count), b(count), serialResult(count), outerResult(count), collapseResult(count);
    for (long long i = 0; i < count; i++) { a[i] = i % 101; b[i] = i % 37; }
    double start = omp_get_wtime(); addSerial(a,b,serialResult,n); double serialTime = omp_get_wtime()-start;
    start = omp_get_wtime(); addOuter(a,b,outerResult,n); double outerTime = omp_get_wtime()-start;
    start = omp_get_wtime(); addCollapse(a,b,collapseResult,n); double collapseTime = omp_get_wtime()-start;
    cout << "Matrix size: " << n << " x " << n << "\nThreads: " << threads
         << "\nOuter-loop result correct: " << (same(serialResult,outerResult) ? "yes" : "no")
         << "\nCollapse result correct: " << (same(serialResult,collapseResult) ? "yes" : "no")
         << "\nSerial time: " << serialTime << " seconds\nOuter-loop parallel time: " << outerTime
         << " seconds\nCollapse(2) time: " << collapseTime << " seconds\n";
    return (same(serialResult,outerResult) && same(serialResult,collapseResult)) ? 0 : 1;
}
