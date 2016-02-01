#include "mex.h"

void timestwo_alt(double *y1, double *y2, double x)
{
    *y1 = 2.0*x;
    *y2 = 2.0*x;
}

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
  double *y1_t;
  double *y2_t;
  double x_t;

  /* Create a 1-by-1 matrix for the return argument. */
  plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
  plhs[1] = mxCreateDoubleMatrix(1, 1, mxREAL);

  /* Get the scalar value of the input x. */
  /* Note: mxGetScalar returns a value, not a pointer. */
  x_t = mxGetScalar(prhs[0]);

  /* Assign a pointer to the output. */
  y1_t = mxGetPr(plhs[0]);
  y2_t = mxGetPr(plhs[1]);
  
  /* Call the timestwo_alt subroutine. */
  timestwo_alt(y1_t, y2_t,x_t);
}