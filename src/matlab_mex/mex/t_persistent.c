#include "mex.h"

static int initialized = 0;
static mxArray *angL=NULL;
static mxArray *angR=NULL;


void cleanup(void)
{
    mexPrintf("MEX-file is terminating, destroying array\n");
    mxDestroyArray(angL);
    mxDestroyArray(angR);
}

void teste()
{
    double angR_end;
    double angL_end;          
    
    angR_end = angR[0] + round(0.0100/0.02*2*180/M_PI);
    angL_end = angL[0] + round(0.0100/0.02*2*180/M_PI);
    
    mexPrintf("angR_end = %f.\n",angR_end);
    mexPrintf("angR_end = %f.\n",angR_end);
    
    angL[0] = angL_end;
    angR[0] = angR_end;
    
    mexPrintf("angL = %f.\n",angL[0]);
    mexPrintf("angR = %f.\n",angR[0]);
    
}
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (!initialized) 
    {
        mexPrintf("MEX-file initializing, creating array\n");

        /* Create persistent array and register its cleanup. */
        angL = mxCreateDoubleMatrix(1, 1, mxREAL);
        angR = mxCreateDoubleMatrix(1, 1, mxREAL);
        mexMakeArrayPersistent(angL);
        mexMakeArrayPersistent(angR);
        mexAtExit(cleanup);
        initialized = 1;

        /* Set the data of the array to some interesting value. */
        *mxGetPr(angL) = 0;
        *mxGetPr(angR) = 0;
    } 
    else 
    {
        mexPrintf("MEX-file executing; value of first array element is %g\n",*mxGetPr(angL));
        mexPrintf("MEX-file executing; value of first array element is %g\n",*mxGetPr(angR));
    }
    
    mexPrintf("angR %g\n",*mxGetPr(angR));
    mexPrintf("angL %g\n",*mxGetPr(angR));
    
}

