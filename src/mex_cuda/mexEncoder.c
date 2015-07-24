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
        mxFree(pr_angL);
}


void mexEncoder(double pose_distance_x_t, double pose_distance_y_t, double pose_distance_th_t, double wheel_distance_t, double encoder_ticks_t, double wheel_radius_t, double *dtick_L_t, double *dtick_R_t)
{
    double tick_R = 0;
    double tick_L = 0;
    
    double D;
    double Dr;
    double Dl;
    double angR_end;
    double angL_end;
    double angles;
    
    /* calculate the distance traveled by each wheel */
    D = sqrt((pose_distance_x_t*pose_distance_x_t)+(pose_distance_y_t*pose_distance_y_t));
    Dr = (D + wheel_distance_t/2*pose_distance_th_t);
    Dl = (D - wheel_distance_t/2*pose_distance_th_t);
    
    /* converts that to degrees */
    angR_end = angR[0] + round(Dr/wheel_radius_t*2*180/M_PI);
    angL_end = angR[0] + round(Dl/wheel_radius_t*2*180/M_PI);
    
    /* right wheel */
    if ((angR_end - angR[0]) > 0 )/* positive rotation */
    {
        double temp1 = (2*360/encoder_ticks_t);
        double temp2 = fmod(angles,temp1);
        
        for (angles = angR[0]+1; angles<=angR_end; angles++)
        {          
            if ( fmod(angles,2*360/encoder_ticks_t)== 0)
            {
                tick_R = tick_R+1;
            }
        }
    }    
    else if ((angR_end - angR[0]) < 0 )/* negative rotation */
    {
        for (angles = angR_end+1; angles<=angR[0]; angles++)
        {
            if ( fmod(angles,(2*360/encoder_ticks_t))==0)
            {
                tick_R = tick_R-1;
            }
        }
    }      
    
    /* left wheel */
    if ((angL_end - angL[0]) > 0 )/* positive rotation */
    {
        for (angles = angL[0]+1; angles<=angL_end; angles++)
        {
            if ( fmod(angles, (2*360/encoder_ticks_t))==0)
            {
                tick_L = tick_L+1;
            }
        }
    }
    else if ((angL_end - angL[0]) < 0) /* negative rotation */
    {
        for (angles = angL_end+1; angles<=angL[0]; angles++)
        {
            if (fmod(angles, (2*360/encoder_ticks_t))==0)
            {
                tick_L = tick_L-1;
            }
        }
    }
    
    pr_angL[0] = angL_end;
    pr_angR[0] = angR_end;
}

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
    if(nlhs!=2){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nlhs",
                          "Four outputs required.");
    }
    
    pose_distance_x_t = mxGetScalar(prhs[0]);
    pose_distance_y_t = mxGetScalar(prhs[1]);
    pose_distance_th_t = mxGetScalar(prhs[2]);
    wheel_distance_t = mxGetScalar(prhs[3]);
    encoder_ticks_t = mxGetScalar(prhs[4]);
    wheel_radius_t = mxGetScalar(prhs[5]);
    
    
    if (pr_angL==NULL)
    {  
        mexPrintf("First call to MEX-file\n");
        pr_angL = mxCalloc(1,8);
        pr_angR = mxCalloc(1,8);
        mexMakeMemoryPersistent(pr_angL);
        mexMakeMemoryPersistent(pr_angR);
        mexAtExit(exitFcn);
    }

    mexPrintf("Old string was '%f' and '%f'.\n",angL[0], angL[0]);
    pr_angL = mxGetPr(prhs[0]);
    pr_angR = mxGetPr(prhs[1]);
    mexPrintf("New string is '%f' and '%f'.\n",pr_angL[0], pr_angR[0]);
    memcpy((char*)pr_angL,(char*)mxGetPr(prhs[0]), sizeof(double));
    memcpy((char*)pr_angR,(char*)mxGetPr(prhs[1]), sizeof(double));
    
    /* Create the output vector */
    plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
    plhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);

    /* Get pointer to the real data in the output scalar */
    dtick_L_t = mxGetPr(plhs[0]);
    dtick_R_t = mxGetPr(plhs[1]);    
    
    /* Call the computational routine */
    mexEncoder(pose_distance_x_t,pose_distance_y_t,pose_distance_th_t,wheel_distance_t,encoder_ticks_t,wheel_radius_t, dtick_L_t, dtick_R_t);
}
