#include <iostream>
#include <cmath>

#include <fftw3.h>

void fftw3_test_syntax() {
  std::cout << std::endl;
  std::cout << "fftw3_test [N [offset]]" << std::endl;
  std::cout << "  N: 2^N Fourier Power" << std::endl;
  std::cout << "     default value if not given: N=13 (2^13 = 8192)" << std::endl;
  std::cout << "  offset: allows to have a length != 2^N, namely 2^N + offset" << std::endl;
  std::cout << "     default value of offset if not given is 0." << std::endl;
  std::cout << std::endl;
}

int main(int argc, char *argv[]) {
  unsigned int N=8192;
  unsigned int offset=0;
  
  if (argc > 3) {
    fftw3_test_syntax();
    return 1;
  }

  int a=-1;
  for (int i=1; i<argc; i++) {
    int status = sscanf(argv[i], "%d", &a);
    if (status != 1) {
      fftw3_test_syntax();
      return 2;
    }
    if (i==1) // N
      N = (unsigned int)pow(2.0, a);
    if (i==2) // offset
      offset = (unsigned int)a;
  }
std::cout << "debug> N=" << N << ", offset=" << offset << std::endl;
    
  fftw_complex *in, *out;
  fftw_plan my_plan;
    
  N += offset;
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
