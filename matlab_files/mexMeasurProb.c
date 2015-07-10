/* Attempt to use fast_ray_cast function in a MEX file */

/*
* mexMeasurProb.c
*
* Calculates probability
*
* The calling syntax is:
*
*		prob = mexMeasurProb(z(j), sonars(j), CovSonars)
*
* This is a MEX-file for MATLAB.
*/

#include "mex.h"
#include <math.h>

void mexMeasurProb(double x[], double mu[], double sigma[], double prob[])
{
    /* Code for measure probability */
    prob[0] = exp(-pow((x[0]-mu[0]),2)/(2*pow(sigma[0],2))) / sqrt(2*M_PI*pow(sigma[0],2));
}

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    /* Variable declarations here */
    double *x_t;        /* z */
    double *mu_t;       /* sonars */
    double *sigma_t;    /* CovSonars */
    double *prob_t;    /* output scalar */
    
    /* Code here */
    /* Check for parameters */
    if(nrhs!=3){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nrhs",
                          "Three inputs required.");
    }
    if(nlhs!=1){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nlhs",
                          "One output required.");
    }
    
    /* Get the scalar values */
    x_t = mxGetPr(prhs[0]);
    mu_t = mxGetPr(prhs[1]);
    sigma_t = mxGetPr(prhs[2]);
    
    /* Create the output Scalar */
    plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
    
    /* Get pointer to the real data in the output scalar */
    prob_t = mxGetPr(plhs[0]);
    
    /* Call the computational routine */
    mexMeasurProb(x_t, mu_t, sigma_t, prob_t);
}
