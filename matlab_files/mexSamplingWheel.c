/*
* mexSamplingWheel.c
*
* Resampling by sampling wheel
*
* The calling syntax is:
*
*		p2 = mexSamplingWheel(w, p)
*
* This is a MEX-file for MATLAB.
*/

#include "mex.h"
#include <stdlib.h>

/* Subfunction to calculate random number in the range of 0 - 1 */
double r3(){
    return (double)rand()/(double)RAND_MAX;
}

/* Subfunction to find max element in a double array */
double max(double *arr, mwSize size){
    double max_elem;
    mwSize i;
    
    for(i=1; i<size; i++){
        if(arr[i] > max_elem)
            max_elem = arr[i];
    }
    
    return max_elem;
}

void mexSamplingWheel(double *w, double *p, mwSize p_N, double *p2)
{
    /* Code for Resampling */
    double mw, beta = 0;
    mwSize index;
    mw = max(w, p_N);
    index = rand() % p_N;/* random from 0 to p_N-1 */;
    
    mwSize i;
    for(i=0; i<p_N; i++){
        beta += r3()*mw*2;
        while(beta>w[index]){
            beta -= w[index];
            index++;
            if(index > p_N-1)
                index = 0;
        }
        /*p2[i] = index;*/
        p2[i*3 + 0] = p[index*3 + 0];
        p2[i*3 + 1] = p[index*3 + 1];
        p2[i*3 + 2] = p[index*3 + 2];
    }
}

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    /* Variable declarations here */
    double        *w_t;        /* w */
    double        *p_t;        /* p2 */
    mwSize         p_M;        /* M size of w */
    mwSize         p_N;        /* N size of w */
    double       *p2_t;        /* output array */
    
    /* Code here */
    /* Check for parameters */
    if(nrhs!=2){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nrhs",
                          "Two inputs required.");
    }
    if(nlhs!=1){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nlhs",
                          "One output required.");
    }
    
    /* Create a pointer to the real data in the matrixes */
    w_t = mxGetPr(prhs[0]);
    p_t = mxGetPr(prhs[1]);
    
    /* Get dimensions */
    p_M = mxGetM(prhs[1]);
    p_N = mxGetN(prhs[1]);
    
    /* Create the output array */
    plhs[0] = mxCreateDoubleMatrix(3,p_N,mxREAL);
    
    /* Get pointer to the real data in the output array */
    p2_t = mxGetPr(plhs[0]);
    
    /* Call the computational routine */
    mexSamplingWheel(w_t, p_t, p_N, p2_t);
}