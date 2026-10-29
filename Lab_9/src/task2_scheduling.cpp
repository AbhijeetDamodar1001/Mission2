#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>
#include <omp.h>

static void work(double& value,long long i,int scale) {
    double x=value;
    int iterations=static_cast<int>(i%1000LL)*scale;
    for(int j=0;j<iterations;++j) x=x*1.0000001+0.0000001;
    value=x;
}
static double serial_run(const std::vector<double>& in,std::vector<double>& out,int scale) {
    double t=omp_get_wtime();
    for(long long i=0;i<(long long)in.size();++i) { out[i]=in[i]; work(out[i],i,scale); }
    return omp_get_wtime()-t;
}
static double static_run(const std::vector<double>& in,std::vector<double>& out,int nt,int scale) {
    long long n=in.size(); double t=omp_get_wtime();
    #pragma omp parallel for num_threads(nt) schedule(static)
    for(long long i=0;i<n;++i) { out[i]=in[i]; work(out[i],i,scale); }
    return omp_get_wtime()-t;
}
static double dynamic_run(const std::vector<double>& in,std::vector<double>& out,int nt,int scale,int chunk) {
    long long n=in.size(); double t=omp_get_wtime();
    #pragma omp parallel for num_threads(nt) schedule(dynamic,chunk)
    for(long long i=0;i<n;++i) { out[i]=in[i]; work(out[i],i,scale); }
    return omp_get_wtime()-t;
}
static double guided_run(const std::vector<double>& in,std::vector<double>& out,int nt,int scale) {
    long long n=in.size(); double t=omp_get_wtime();
    #pragma omp parallel for num_threads(nt) schedule(guided)
    for(long long i=0;i<n;++i) { out[i]=in[i]; work(out[i],i,scale); }
    return omp_get_wtime()-t;
}
static void report(const char* name,double t,double serial,int nt,bool ok) {
    double speedup=t>0?serial/t:0;
    std::cout<<"\nStrategy: "<<name<<"\nCorrect: "<<(ok?"yes":"no")
             <<"\nParallel time: "<<t<<" seconds\nSpeedup: "<<speedup
             <<"\nEfficiency: "<<speedup/nt<<"\n";
}
int main(int argc,char** argv) {
    long long n=argc>1?std::atoll(argv[1]):1000000LL;
    int nt=argc>2?std::atoi(argv[2]):4;
    int scale=argc>3?std::atoi(argv[3]):1;
    int chunk=argc>4?std::atoi(argv[4]):100;
    if(n<1||nt<1||scale<0||chunk<1) {
        std::cerr<<"Usage: task2.exe <N> <threads> [work_scale=1] [dynamic_chunk=100]\n"; return 1;
    }
    std::vector<double> in(n), serial(n), out(n);
    for(long long i=0;i<n;++i) in[i]=i*0.001;
    double st=serial_run(in,serial,scale);
    omp_set_dynamic(0); std::cout<<std::setprecision(9)
        <<"Task 2: OpenMP Scheduling Strategies\nN: "<<n<<"\nThreads: "<<nt
        <<"\nWork scale: "<<scale<<"\nDynamic chunk size: "<<chunk
        <<"\nSerial time: "<<st<<" seconds\n";
    double t=static_run(in,out,nt,scale); report("static",t,st,nt,serial==out);
    t=dynamic_run(in,out,nt,scale,chunk); report(("dynamic, chunk="+std::to_string(chunk)).c_str(),t,st,nt,serial==out);
    t=guided_run(in,out,nt,scale); report("guided",t,st,nt,serial==out);
    return 0;
}
