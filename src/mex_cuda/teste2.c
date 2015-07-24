#include <stdlib.h>
#include "mex.h"
static double *angL = NULL;
static double *angR=NULL;
double *pr_angL;
double *pr_angR;
 
void exitFcn() 
{
    if (angL != NULL)
        mxFree(angL);

    if (angR != NULL)
        mxFree(angL);
}

void teste()
{
    mexPrintf("New");
    mexPrintf("New string is '%f' and '%f'.\n",angL[0], angR[0]);
    /* mexPrintf("New string is '%f' and '%f'.\n",pr_angL[0], pr_angR[0]);*/
}

void mexFunction(int nlhs,mxArray *plhs[],int nrhs, const mxArray *prhs[])
{
    
    if (nrhs<1 | mxIsChar(prhs[0]))
    {
        mexErrMsgTxt("Must have one non-string input");
    }
    
    if (angL==NULL)
    {  
        /* since angL is initialized to NULL, we know
        this is the first call of the MEX-function 
        after it was loaded.  Therefore, we should
        set up angL and the exit function. */
        /* Allocate array. Use mexMackMemoryPersistent to make the allocated memory persistent in subsequent calls*/

        mexPrintf("First call to MEX-file\n");
        angL = mxCalloc(1,8);
        angR = mxCalloc(1,8);
        mexMakeMemoryPersistent(angL);
        mexMakeMemoryPersistent(angR);
        mexAtExit(exitFcn);
    }

    mexPrintf("Old string was '%f' and '%f'.\n",angL[0], angR[0]);
    pr_angL = mxGetPr(prhs[0]);
    pr_angR = mxGetPr(prhs[1]);
    mexPrintf("New string is '%f' and '%f'.\n",pr_angL[0], pr_angR[0]);
    memcpy((char*)angL,(char*)mxGetPr(prhs[0]), sizeof(double));
    memcpy((char*)angR,(char*)mxGetPr(prhs[1]), sizeof(double));
   
    teste();
}