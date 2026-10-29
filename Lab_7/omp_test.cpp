#include <iostream>
#include <omp.h>
using namespace std;

int main() {
    cout << "Maximum threads: "
         << omp_get_max_threads() << endl;

    #pragma omp parallel
    {
        cout << "Hello from a thread!" << endl;
    }

    return 0;
}
