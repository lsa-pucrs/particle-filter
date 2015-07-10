/* Attempt to use fast_ray_cast function in a MEX file */

/*
* mexMeasurProb.c
*
* Calculates probability
*
* The calling syntax is:
*
*		prob = mexEstimateProb(p, dtick_L, dtick_R, wheel_distance, encoder_ticks, wheel_radius, alpha)
 *      p = [x y theta] (double vector)
 *      dtick_L, dtick_R = double scalars
 *      wheel_distance, encoder_ticks, wheel_radius = int scalars
 *      alpha = PF divergence params (double vector)
*
* This is a MEX-file for MATLAB.
*/

#include "mex.h"
#include <math.h>

double r2(){
    return (double)rand()/(double)RAND_MAX;
}

double random_normal() {
    double u = r2() * 2 - 1;
    double v = r2() * 2 - 1;
    double r = u * u + v * v;
    if (r == 0 || r > 1) 
        return random_normal();
    double c = sqrt(-2 * log(r) / r);
    return u * c;
}

void mexEstimateProb(double *p, double dtick_L, double dtick_R, double wheel_distance, mwSize encoder_ticks, double wheel_radius, double *alpha, double *p2)
{
    /* Code for estimate probability */    
    double Dl, Dr, Dc;
    Dl = 2*M_PI*wheel_radius*dtick_L/encoder_ticks;
    Dr = 2*M_PI*wheel_radius*dtick_R/encoder_ticks;
    Dc = (Dl+Dr)/2;
    
    double bar_p[3];
    bar_p[2] = p[2] + (Dr-Dl)/wheel_distance;
    bar_p[0] = p[0] + Dc*cos((p[2]+bar_p[2])/2);
    bar_p[1] = p[1] + Dc*sin((p[2]+bar_p[2])/2);
    
    double rot_1, trans, rot_2, drot_1, dtrans, drot_2;
    rot_1 = atan2(bar_p[1]-p[1],bar_p[0]-p[0])-p[2];
    trans = sqrt(((bar_p[1]-p[1])*(bar_p[1]-p[1])) + ((bar_p[0]-p[0])*(bar_p[0]-p[0])));
    rot_2 = bar_p[2] - p[2] - rot_1;
    
    drot_1 = rot_1 - (alpha[0]*(rot_1*rot_1) + alpha[1]*(trans*trans))*random_normal();
    dtrans = trans - (alpha[2]*(trans*trans) + alpha[3]*((rot_1*rot_1)+(rot_2*rot_2)))*random_normal();
    drot_2 = rot_2 - (alpha[0]*(rot_2*rot_2) + alpha[1]*(trans*trans))*random_normal();
    
    p2[0] = p[0] + dtrans*cos(p[2]+drot_1);
    p2[1] = p[1] + dtrans*sin(p[2]+drot_1);
    p2[2] = p[2] + drot_1 + drot_2;
}

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    /* Variable declarations here */
    double *p_t;                /* p */
    double dtick_L_t;           /* dtick_L */
    double dtick_R_t;           /* dtick_R */
    double wheel_distance_t;    /* wheel_distance */
    mwSize encoder_ticks_t;     /* encoder_ticks */
    double wheel_radius_t;      /* wheel_radius */
    double *alpha_t;            /* alpha */
    double *p2_t;               /* output vector */
    
    /* Code here */
    /* Check for parameters */
    if(nrhs!=7){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nrhs",
                          "Seven inputs required.");
    }
    if(nlhs!=1){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nlhs",
                          "One output required.");
    }
    
    /* Get the scalar values */
    dtick_L_t = mxGetScalar(prhs[1]);
    dtick_R_t = mxGetScalar(prhs[2]);
    wheel_distance_t = mxGetScalar(prhs[3]);
    encoder_ticks_t = mxGetScalar(prhs[4]);
    wheel_radius_t = mxGetScalar(prhs[5]);
  
    /* Get pointers to vectors */
    p_t = mxGetPr(prhs[0]);
    alpha_t = mxGetPr(prhs[6]);
    
    /* Create the output vector */
    plhs[0] = mxCreateDoubleMatrix(1,3,mxREAL);
    
    /* Get pointer to the real data in the output scalar */
    p2_t = mxGetPr(plhs[0]);
    
    /* Call the computational routine */
    mexEstimateProb(p_t, dtick_L_t, dtick_R_t, wheel_distance_t, encoder_ticks_t, wheel_radius_t, alpha_t, p2_t);
}
