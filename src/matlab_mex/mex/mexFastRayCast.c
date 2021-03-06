#include "mex.h"
#include <math.h>

double r2()
{
    return (double)rand()/(double)RAND_MAX;
}

void mexRayCast(double xc, double yc, double theta, double *map, mwSize map_M, mwSize map_N, double max_range, double *angles, mwSize nAngles, double mapscale, double accuracy, double sigma, double *range)
{
    /* Code for ray cast */
    mwSize i, end_flag = 0;
    
    xc *= (double)mapscale;
    yc *= (double)mapscale;
    max_range *= (double)mapscale;
    
    /* If robot outside map */
    if((yc > map_M) || (xc > map_N) || (xc <= -1) || (yc <= -1))
    {
        for(i=0; i<nAngles; i++)
        {
            range[i] = 0;
        }
        end_flag = 1;
    }
    /* If the robot is on top of an occupied area */
    if(map[(mwSize)ceil(xc)*map_M + (mwSize)ceil(yc)]==1)
    {
        for(i=0; i<nAngles; i++)
        {
            range[i] = 0;
        }
        end_flag = 1;
    }
    
    if(!end_flag)
    {
        for(i=0; i<nAngles; i++)
        {
            range[i] = 1*max_range;
        }

        for(i = 0; i<nAngles; i++)
        {
            /* Compute direction */
            double dx, dy, x, y, xceil, yceil, dist;
            dx = accuracy*cos(theta+angles[i]);
            dy = accuracy*sin(theta+angles[i]);

            x = xc;
            y = yc;
            dist = max_range;
            
            while(dist<=max_range)
            {
                x += dx;
                y += dy;
                dist = sqrt(pow((x-xc),2) + pow((y-yc),2));
                xceil = ceil(x);
                yceil = ceil(y);

                
                if((xceil>map_N)||(yceil>map_M)||(xceil<0)||(yceil<0))
                {
                    /* Outside */
                    break;
                }
                
                if(map[(mwSize)xceil*map_M+(mwSize)yceil]==1)
                {
                    /* Hit */
                    range[i] = dist;
                    break;
                }
            }
        }                 
        
        for(i=0; i<nAngles; i++)
        {
            range[i] /= mapscale;
        }
        
        
        for(i=0; i<nAngles; i++)
        {
            range[i] = range[i] + fabs(r2()-0.5)*sigma;
        }
    }
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
    double mapscale_t;  /* mapscale */
    double accuracy_t;  /* accuracy */
    double sigma_t;     /* sigma */
    double *range_t;    /* output array */
    
    /* Code here */
    /* Check for parameters */
    if(nrhs!=9){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nrhs",
                          "Nine inputs required.");
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
    sigma_t = mxGetScalar(prhs[8]);
        
    
    /* Create a pointer to the real data in the matrixes */
    map_t = mxGetPr(prhs[3]);
    angles_t = mxGetPr(prhs[5]);
    
    /* Get dimensions */
    map_M = mxGetM(prhs[3]);
    map_N = mxGetN(prhs[3]);
    nAngles = mxGetN(prhs[5]);
    
    /* Create the output array */
    plhs[0] = mxCreateDoubleMatrix(1,nAngles,mxREAL);
    
    /* Get pointer to the real data in the output array */
    range_t = mxGetPr(plhs[0]);
    
    /* Call the computational routine */
    mexRayCast(x_t, y_t, theta_t, map_t, map_M, map_N, max_range_t, angles_t, nAngles, mapscale_t, accuracy_t, sigma_t, range_t);
}
