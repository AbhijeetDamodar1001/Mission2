#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <omp.h>
using namespace std;

void multiplySerial(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        double sum = 0.0;
        for (int k = 0; k < n; k++) sum += a[(long long)i*n+k] * b[(long long)k*n+j];
        c[(long long)i*n+j] = sum;
    }
}
void multiplyOuter(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        double sum = 0.0;
        for (int k = 0; k < n; k++) sum += a[(long long)i*n+k] * b[(long long)k*n+j];
        c[(long long)i*n+j] = sum;
    }
}
void multiplyCollapse(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        double sum = 0.0;
        for (int k = 0; k < n; k++) sum += a[(long long)i*n+k] * b[(long long)k*n+j];
        c[(long long)i*n+j] = sum;
    }
}
bool same(const vector<double>& a, const vector<double>& b) {
    for (size_t i = 0; i < a.size(); i++) if (fabs(a[i]-b[i]) > 1e-9) return false;
    return true;
}
int main(int argc, char* argv[]) {
    int n = (argc > 1) ? atoi(argv[1]) : 128;
    int threads = (argc > 2) ? atoi(argv[2]) : 4;
    if (n <= 0 || threads <= 0) { cerr << "Use a positive matrix size and thread count.\n"; return 1; }
    omp_set_num_threads(threads);
    long long count = (long long)n*n;
    vector<double> a(count), b(count), serialResult(count,0.0), outerResult(count,0.0), collapseResult(count,0.0);
    for (long long i = 0; i < count; i++) { a[i] = (i % 13) * 0.1; b[i] = (i % 17) * 0.05; }
    double start = omp_get_wtime(); multiplySerial(a,b,serialResult,n); double serialTime = omp_get_wtime()-start;
    start = omp_get_wtime(); multiplyOuter(a,b,outerResult,n); double outerTime = omp_get_wtime()-start;
    start = omp_get_wtime(); multiplyCollapse(a,b,collapseResult,n); double collapseTime = omp_get_wtime()-start;
    cout << "Matrix size: " << n << " x " << n << "\nThreads: " << threads
         << "\nOuter-loop result correct: " << (same(serialResult,outerResult) ? "yes" : "no")
         << "\nCollapse(2) result correct: " << (same(serialResult,collapseResult) ? "yes" : "no")
         << "\nSerial time: " << serialTime << " seconds\nOuter-loop parallel time: " << outerTime
         << " seconds\nOuter-loop speedup: " << serialTime/outerTime << "\nOuter-loop efficiency: " << (serialTime/outerTime)/threads
         << "\nCollapse(2) time: " << collapseTime << " seconds\nCollapse(2) speedup: " << serialTime/collapseTime
         << "\nCollapse(2) efficiency: " << (serialTime/collapseTime)/threads << "\n";
    return (same(serialResult,outerResult) && same(serialResult,collapseResult)) ? 0 : 1;
}
