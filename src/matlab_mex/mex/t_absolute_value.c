#include "mex.h"
#include <math.h>

double r2()
{
    return (double)rand()/(double)RAND_MAX;
}

double random_normal() 
{
    double u = r2() * 2 - 1;
    double v = r2() * 2 - 1;
    double r = u * u + v * v;
    if (r == 0 || r > 1)
        return random_normal();
    return u * sqrt(-2 * log(r) / r);
}

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    double *y1_t;
    double x_t[5];
    double x_t2[5];
    mwSize i;
    /* Create a 1-by-1 matrix for the return argument. */
    plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);

    /* Get the scalar value of the input x. */
    /* Note: mxGetScalar returns a value, not a pointer. */

    for(i=0; i<=4; i++)
    {
        x_t[i]=random_normal();      
        mexPrintf("x_t[%d]=%f\n", i, x_t[i]);
    }
    
    mexPrintf("\n-------------\n");
    
    for(i=0; i<=4; i++)
    {
        x_t[i]=random_normal();      
        mexPrintf("x_t[%d]=%f\n", i, x_t[i]);
    }
    
    mexPrintf("\n-------------\n");
    
    for(i=0; i<=4; i++)
    {
        x_t2[i]= fabs(x_t[i]);    
        mexPrintf("x_t[%d]=%f\n", i, x_t2[i]);
    }
    
    mexPrintf("\n-------------\n");
    
    /* Assign a pointer to the output. */
    y1_t = mxGetPr(x_t); 

}