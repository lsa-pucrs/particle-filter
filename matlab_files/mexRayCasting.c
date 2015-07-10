/*
* mexRayCast.c
*
* Makes ray cast
*
* The calling syntax is:
*
*		z = mexRayCasting(xc, yc, theta, map', max_range, angles, mapscale, accuracy, sonars', CovSonars, w(i))
*
* This is a MEX-file for MATLAB.
*/

#include "mex.h"
#include <math.h>
#include <stdlib.h>

void mexRayCasting(double xc, double yc, double theta, double *map, mwSize map_M, mwSize map_N, double max_range, double *angles, mwSize nAngles, mwSize mapscale, double accuracy, double *mu, double sigma, double w_i, double *prob)
{
    /* Code for ray cast */
    double *range;
    range = malloc(nAngles * sizeof(double));
    mwSize i, end_flag = 0;
    
    xc *= mapscale;
    yc *= mapscale;
    max_range *= mapscale;
    
    /* If robot outside map */
    if((yc > map_N) || (xc > map_M) || (xc <= -1) || (yc <= -1)){
        for(i=0; i<nAngles; i++)
            range[i] = 0;
        end_flag = 1;
    }
    /* If the robot is on top of an occupied area */
    if(map[(mwSize)ceil(xc)*map_M + (mwSize)ceil(yc)]==1){
        for(i=0; i<nAngles; i++)
            range[i] = 0;
        end_flag = 1;
    }
    
    if(!end_flag){
        for(i=0; i<nAngles; i++){
            range[i] = 1*max_range;
        }
                
        for(i = 0; i<nAngles; i++){
            /* Compute direction */
            double dx, dy, x, y, xceil, yceil, dist;
            dx = accuracy*cos(theta+angles[i]);
            dy = accuracy*sin(theta+angles[i]);
            
            /* Initialize the ray */
            x = xc;
            y = yc;
            dist = max_range;
            
            while(dist<=max_range){
                x += dx;
                y += dy;
                dist = sqrt(pow((x-xc),2) + pow((y-yc),2));
                xceil = ceil(x);
                yceil = ceil(y);
                if((xceil>map_M)||(yceil>map_N)||(xceil<-1)||(yceil<-1)){
                    /* Outside */
                    break;
                }
                if(map[(mwSize)xceil*map_M+(mwSize)yceil]==1){
                    /* Hit */
                    range[i] = dist;
                    break;
                }
            }
        }
        
        for(i=0; i<nAngles; i++){
            range[i] /= mapscale;
        }
    }
    
    prob[0] = w_i*exp(-pow((range[0]-mu[0]),2)/(2*pow(sigma,2))) / sqrt(2*M_PI*pow(sigma,2));
    for(i=1; i<nAngles; i++){
        prob[0] *= exp(-pow((range[i]-mu[i]),2)/(2*pow(sigma,2))) / sqrt(2*M_PI*pow(sigma,2));
    }
    
    free(range);
}

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    /* Variable declarations here */
    double x_t;         /* xc */
    double y_t;         /* yc */
    double theta_t;     /* theta */
    double *map_t;      /* map (MxN matrix) */
    mwSize map_M;       /* M-dimension of map */
    mwSize map_N;       /* N-dimension of map */
    double max_range_t; /* max_range */
    double *angles_t;   /* angles */
    mwSize nAngles;     /* size of angles */
    mwSize mapscale_t;  /* mapscale */
    double accuracy_t;  /* accuracy */
    double *mu_t;       /* mu */
    double sigma_t;     /* sigma */
    double w_i_t;       /* w_i */
    double *prob_t;     /* output scalar */
    
    /* Code here */
    /* Check for parameters */
    if(nrhs!=11){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nrhs",
                          "Eleven inputs required.");
    }
    if(nlhs!=1){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nlhs",
                          "One output required.");
    }
    
    /* Get the scalar values */
    x_t = mxGetScalar(prhs[0]);
    y_t = mxGetScalar(prhs[1]);
    theta_t = mxGetScalar(prhs[2]);
    max_range_t = mxGetScalar(prhs[4]);
    mapscale_t = mxGetScalar(prhs[6]);
    accuracy_t = mxGetScalar(prhs[7]);
    sigma_t = mxGetScalar(prhs[9]);
    w_i_t = mxGetScalar(prhs[10]);
    
    /* Create a pointer to the real data in the matrixes */
    map_t = mxGetPr(prhs[3]);
    angles_t = mxGetPr(prhs[5]);
    mu_t = mxGetPr(prhs[8]);
    
    /* Get dimensions */
    map_M = mxGetM(prhs[3]);
    map_N = mxGetN(prhs[3]);
    nAngles = mxGetN(prhs[5]);
    
    /* Create the output array */
    plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
    
    /* Get pointer to the real data in the output array */
    prob_t = mxGetPr(plhs[0]);
    
    /* Call the computational routine */
    mexRayCasting(x_t, y_t, theta_t, map_t, map_M, map_N, max_range_t, angles_t, nAngles, mapscale_t, accuracy_t, mu_t, sigma_t, w_i_t, prob_t);
}
