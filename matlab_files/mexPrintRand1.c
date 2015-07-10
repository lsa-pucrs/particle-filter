#include <stdio.h>
#include <stdlib.h>
#include "mex.h" 

void print_rand1(){
    mexPrintf("\nRandom number 1: %d\n", rand());
}

void mexFunction(int nlhs, mxArray *plhs[],
    int nrhs, const mxArray *prhs[]) {
    
    print_rand1();
}