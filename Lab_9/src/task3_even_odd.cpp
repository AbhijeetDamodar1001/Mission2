#include <cstdlib>
#include <iostream>
#include <vector>
#include <omp.h>
struct Counts { long long even=0, odd=0; };
static Counts serial_count(const std::vector<int>& a) {
    Counts c; for(int v:a) { if(v%2==0) ++c.even; else ++c.odd; } return c;
}
static Counts reduction_count(const std::vector<int>& a,int nt) {
    long long even=0,odd=0,n=a.size();
    #pragma omp parallel for num_threads(nt) reduction(+:even,odd)
    for(long long i=0;i<n;++i) { if(a[i]%2==0) ++even; else ++odd; }
    return {even,odd};
}
static Counts atomic_count(const std::vector<int>& a,int nt) {
    long long even=0,odd=0,n=a.size();
    #pragma omp parallel for num_threads(nt)
    for(long long i=0;i<n;++i) {
        if(a[i]%2==0) {
            #pragma omp atomic update
            even++;
        } else {
            #pragma omp atomic update
            odd++;
        }
    }
    return {even,odd};
}
int main(int argc,char** argv) {
    long long n=argc>1?std::atoll(argv[1]):1000000LL;
    int nt=argc>2?std::atoi(argv[2]):4;
    if(n<1||nt<1) { std::cerr<<"Usage: task3.exe <N> <threads>\n"; return 1; }
    std::vector<int> a(n);
    for(long long i=0;i<n;++i) a[i]=static_cast<int>((i*17LL+3)%1000LL);
    double t=omp_get_wtime(); Counts s=serial_count(a); double st=omp_get_wtime()-t;
    omp_set_dynamic(0);
    t=omp_get_wtime(); Counts r=reduction_count(a,nt); double rt=omp_get_wtime()-t;
    t=omp_get_wtime(); Counts at=atomic_count(a,nt); double atime=omp_get_wtime()-t;
    bool rok=s.even==r.even&&s.odd==r.odd, aok=s.even==at.even&&s.odd==at.odd;
    double rs=st/rt, as=st/atime;
    std::cout<<"Task 3: Even/Odd Counting (Reduction vs Atomic)\nN: "<<n<<"\nThreads: "<<nt
      <<"\nSerial even: "<<s.even<<"\nSerial odd: "<<s.odd<<"\nSerial total: "<<s.even+s.odd
      <<"\nSerial time: "<<st<<" seconds\n\nReduction even: "<<r.even<<"\nReduction odd: "<<r.odd
      <<"\nReduction correct: "<<(rok?"yes":"no")<<"\nReduction time: "<<rt<<" seconds"
      <<"\nReduction speedup: "<<rs<<"\nReduction efficiency: "<<rs/nt
      <<"\n\nAtomic even: "<<at.even<<"\nAtomic odd: "<<at.odd<<"\nAtomic correct: "<<(aok?"yes":"no")
      <<"\nAtomic time: "<<atime<<" seconds\nAtomic speedup: "<<as<<"\nAtomic efficiency: "<<as/nt
      <<"\n\nCount invariant (even + odd == N): "<<(s.even+s.odd==n?"yes":"no")<<"\n";
    return rok&&aok&&s.even+s.odd==n?0:2;
}
