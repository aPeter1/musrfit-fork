#include <iostream>
#include <cmath>

#include <fftw3.h>

int main(int argc, char *argv[]) {
    const unsigned int N=8192;
    
    fftw_complex *in, *out;
    fftw_plan my_plan;
    
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*N);
    
    // feed input
    double x=0.0;
    for (unsigned int i=0; i<N; i++) {
        x = (double)i / (double)N;
        in[i][0] = sin(100.0*x)*exp(-x/8.0);
        in[i][1] = 0.0;
//as35        if ((i%100)==0) std::cout << i << ": x=" << x << ", in=" << in[i][0] << std::endl;
    }
    
    my_plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    
    fftw_execute(my_plan);
    
    double sumRe = 0.0, sumIm = 0.0;
    for (unsigned int i=0; i<N/2; i++) {
//as35        if ((i % 10) == 0) std::cout << i << ": re=" << out[i][0] << ", im=" << out[i][1] << std::endl;
        sumRe += out[i][0];
        sumIm += out[i][1];
    }
    
    std::cout << "sumRe=" << sumRe << std::endl;
    std::cout << "sumIm=" << sumIm << std::endl;

    fftw_destroy_plan(my_plan);
    fftw_free(in);
    fftw_free(out);
    
    return 0;
}
