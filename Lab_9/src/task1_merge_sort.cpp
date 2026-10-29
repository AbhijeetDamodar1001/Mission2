#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <omp.h>

static void merge_ranges(std::vector<int>& a, std::vector<int>& tmp,
                         long long left, long long mid, long long right) {
    long long i=left, j=mid+1, k=left;
    while (i<=mid && j<=right) {
        if (a[i] <= a[j]) tmp[k++]=a[i++];
        else tmp[k++]=a[j++];
    }
    while (i<=mid) tmp[k++]=a[i++];
    while (j<=right) tmp[k++]=a[j++];
    for (long long p=left; p<=right; ++p) a[p]=tmp[p];
}
static void merge_sort_serial(std::vector<int>& a, std::vector<int>& tmp,
                              long long left, long long right) {
    if (left>=right) return;
    long long mid=left+(right-left)/2;
    merge_sort_serial(a,tmp,left,mid);
    merge_sort_serial(a,tmp,mid+1,right);
    merge_ranges(a,tmp,left,mid,right);
}
static void merge_sort_tasks(std::vector<int>& a, std::vector<int>& tmp,
                             long long left, long long right, long long cutoff) {
    if (left>=right) return;
    if (right-left+1<=cutoff) {
        merge_sort_serial(a,tmp,left,right);
        return;
    }
    long long mid=left+(right-left)/2;
    #pragma omp task shared(a,tmp) firstprivate(left,mid,cutoff)
    merge_sort_tasks(a,tmp,left,mid,cutoff);
    #pragma omp task shared(a,tmp) firstprivate(mid,right,cutoff)
    merge_sort_tasks(a,tmp,mid+1,right,cutoff);
    #pragma omp taskwait
    merge_ranges(a,tmp,left,mid,right);
}
int main(int argc,char** argv) {
    long long n=argc>1?std::atoll(argv[1]):1000000LL;
    int threads=argc>2?std::atoi(argv[2]):4;
    long long cutoff=argc>3?std::atoll(argv[3]):2048;
    if(n<1||threads<1||cutoff<1) {
        std::cerr<<"Usage: task1.exe <N> <threads> [cutoff]\n"; return 1;
    }
    std::vector<int> original(n);
    for(long long i=0;i<n;++i) original[i]=static_cast<int>((i*48271LL+17)%1000003LL);
    std::vector<int> serial=original, parallel=original, tmp(n);
    double s=omp_get_wtime();
    merge_sort_serial(serial,tmp,0,n-1);
    double st=omp_get_wtime()-s;
    omp_set_dynamic(0); omp_set_num_threads(threads);
    std::fill(tmp.begin(),tmp.end(),0);
    double p=omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        merge_sort_tasks(parallel,tmp,0,n-1,cutoff);
    }
    double pt=omp_get_wtime()-p;
    bool sorted=std::is_sorted(parallel.begin(),parallel.end());
    bool matches=serial==parallel;
    double speedup=pt>0?st/pt:0;
    std::cout<<"Task 1: Recursive Merge Sort with OpenMP Tasks\nN: "<<n
             <<"\nThreads: "<<threads<<"\nCutoff: "<<cutoff
             <<"\nSorted: "<<(sorted?"yes":"no")
             <<"\nMatches serial output: "<<(matches?"yes":"no")
             <<"\nCorrect: "<<(sorted&&matches?"yes":"no")
             <<"\nSerial time: "<<st<<" seconds\nParallel time: "<<pt
             <<" seconds\nSpeedup: "<<speedup<<"\nEfficiency: "<<speedup/threads<<"\n";
    return sorted&&matches?0:2;
}
