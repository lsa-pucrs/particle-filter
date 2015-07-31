#include "mex.h"
#include <math.h>
#include <stdlib.h>

static double *angL=NULL;
static double *angR=NULL;
double *pr_angL;
double *pr_angR;

void exitFcn() 
{
    if (pr_angL != NULL)
        mxFree(pr_angL);

    if (pr_angR != NULL)
        mxFree(pr_angR);
}


void mexEncoder(double pose_distance_x, double pose_distance_y, double pose_distance_th, double wheel_distance, double encoder_ticks, double wheel_radius, double *dtick_L, double *dtick_R)
{
    mexPrintf(" ---------------------------------begin Function--------------------------------- \n\n");
    double tick_R = 0;
    double tick_L = 0;
    
    double D;
    double Dr;
    double Dl;
    double angR_end;
    double angL_end;
    double angles;
    
    D = sqrt( pose_distance_x*pose_distance_x + pose_distance_y*pose_distance_y);
    Dr = (D + ((wheel_distance/2)*pose_distance_th));
    Dl = (D - ((wheel_distance/2)*pose_distance_th));
    /* converts that to degrees */
    angR_end = pr_angR[0] + round(((Dr/wheel_radius)*2*180)/M_PI);
    angL_end = pr_angL[0] + round(((Dl/wheel_radius)*2*180)/M_PI);
    
    double teste = ((Dr/wheel_radius)*2*180)/M_PI;
    double teste2 = round(teste);
    
    mexPrintf("pr_angR: %f\n", pr_angR[0]);
    mexPrintf("pr_angL: %f\n", pr_angL[0]);
    mexPrintf("D: %f\n", D);
    mexPrintf("Dr: %f\n", Dr);
    mexPrintf("Dl: %f\n", Dl);    
    mexPrintf("teste: %f\n", teste);
    mexPrintf("teste2: %f\n", teste2);
    mexPrintf("angR_end: %f\n", angR_end);
    mexPrintf("angR: %f\n", angL_end);
    mexPrintf("tick_R: %f\n", tick_R);
    
    /* right wheel */
    
    mexPrintf("----------- Right wheel\n");
    if((angR_end - angR[0]) > 0) /* positive rotation */
    {
        mexPrintf("Entro no if do Right - positive rotation\n");
        
        for (angles = angR[0]+1; angles<=angR_end; angles++)
        {
            /* mexPrintf("angles: %f\n", angles); */
            if (fmod(angles,2*360/encoder_ticks)==0 )
            {
                tick_R=tick_R+1;
                mexPrintf("tick_R: %f\n", tick_R);
            }
        }
    }
    else if ((angR_end - angR[0]) < 0) /* negative rotation */
    {
        mexPrintf("Entro no else do Right - positive rotation\n");
        for (angles = angR_end+1; angles<=angR[0]; angles++)
        {
            if (fmod(angles,2*360/encoder_ticks)==0)
            {
                tick_R=tick_R-1;
            }
        }
    }
    
    /* left wheel */
    mexPrintf("----------- left wheel\n");
    if ((angL_end - angL[0]) > 0) /*positive rotation */
    {
        mexPrintf("Entro no if wheel do left - positive rotation\n");
        for (angles=angL[0]+1; angles<=angL_end; angles++)
        {
            if (fmod(angles,2*360/encoder_ticks)==0)
            {
                tick_L=tick_L+1;
            }
        }
    }
    else if ((angL_end - angL[0]) < 0) /*negative rotation */
    {
        mexPrintf("Entro no else do left - positive rotation\n");
        for (angles=angL_end+1; angles<=angL[0]; angles++)
        {
            if (fmod(angles,2*360/encoder_ticks)==0)
            {
                tick_L=tick_L-1;
            }
        }
    }

    pr_angL[0] = mxGetScalar(angL_end);    
    pr_angR[0] = mxGetScalar(angR_end);
    
    dtick_R[0] = tick_R;
    dtick_L[0] = tick_L;
}
/*mexEncoder(0.01, 0, 0, 0.08, 48, 0.02)*/
/*
 *
 * D=0,01
 * Dr=0,01
 * Dl=0,01
 * angR_end = 57
 * angR_end = 57
 * positive rotation
 **/

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    /* Variable declarations here */
    double pose_distance_x_t;   /* pose_distance */
    double pose_distance_y_t;   /* pose_distance */
    double pose_distance_th_t;  /* pose_distance */
    double wheel_distance_t;    /* wheel_distance */
    mwSize encoder_ticks_t;     /* encoder_ticks */
    double wheel_radius_t;      /* wheel_radius */
    double *dtick_L_t;          /* output scalar */
    double *dtick_R_t;          /* output scalar */
    
    /* Code here */
    /* Check for parameters */
    if(nrhs!=6){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nrhs",
                          "Sixteen inputs required.");
    }
    /*
    if(nlhs!=2){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nlhs",
                          "Two outputs required.");
    }*/
    
    pose_distance_x_t = mxGetScalar(prhs[0]);
    pose_distance_y_t = mxGetScalar(prhs[1]);
    pose_distance_th_t = mxGetScalar(prhs[2]);
    wheel_distance_t = mxGetScalar(prhs[3]);
    encoder_ticks_t = mxGetScalar(prhs[4]);
    wheel_radius_t = mxGetScalar(prhs[5]);
    
    
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

    /* mexPrintf("Old string was '%f' and '%f'.\n",angL[0], angR[0]);  */
    pr_angL = mxGetPr(prhs[0]);    
    pr_angR = mxGetPr(prhs[1]);    
    /* mexPrintf("New string is '%f' and '%f'.\n",pr_angL[0], pr_angR[0]); */
    mexPrintf("--------------First angL and angR: '%f' and '%f'.\n",pr_angL[0], pr_angR[0]);  
    
    /* Create the output vector */
    plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
    plhs[1] = mxCreateDoubleMatrix(1, 1, mxREAL);

    /* Get pointer to the real data in the output scalar */
    dtick_L_t = mxGetPr(plhs[0]);
    dtick_R_t = mxGetPr(plhs[1]);
    
    /* Call the computational routine */
    mexEncoder(pose_distance_x_t,pose_distance_y_t,pose_distance_th_t,wheel_distance_t,encoder_ticks_t,wheel_radius_t, dtick_L_t, dtick_R_t);
}
