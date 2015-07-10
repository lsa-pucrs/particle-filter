#include <stdio.h>
#include <stdlib.h>
#include "mex.h" 

void print_rand2(){
    mexPrintf("\nRandom number 2: %d\n", rand());
}

void mexFunction(int nlhs, mxArray *plhs[],
    int nrhs, const mxArray *prhs[]) {
    
    print_rand2();
}