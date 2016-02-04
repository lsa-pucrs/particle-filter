#include "mex.h"
#include <math.h>
#include <stdlib.h>

/* Function to return a random number in the range (0,1) */
double r2()
{
    return (double)rand()/(double)RAND_MAX;
}

/* Function to return normalized random number */
double random_normal() 
{
    double u = r2() * 2 - 1;
    double v = r2() * 2 - 1;
    double r = u * u + v * v;
    if (r == 0 || r > 1)
        return random_normal();
    return u * sqrt(-2 * log(r) / r);
}

/* Function to find maximum between two numbers */
mwSize max(mwSize first, mwSize second)
{
    if(second > first)
        return second;
    else
        return first;
}

/* Function to find minimum between two numbers */
mwSize min(mwSize first, mwSize second)
{
    if(second < first)
        return second;
    else
        return first;
}

/* Prediction Stage - Implements motion estimation and variance for the particles */
void mexEstimateProb(double *p, double dtick_L, double dtick_R, double wheel_distance, mwSize encoder_ticks, double wheel_radius, double *alpha, mwSize i, double *p2)
{
    /* Code for estimate probability */
    double Dl, Dr, Dc, bar_p[3],rot_1, trans, rot_2, drot_1, dtrans, drot_2;
    Dl = 2*M_PI*wheel_radius*dtick_L/encoder_ticks;
    Dr = 2*M_PI*wheel_radius*dtick_R/encoder_ticks;
    Dc = (Dl+Dr)/2;

    bar_p[2] = p[2] + (Dr-Dl)/wheel_distance;
    bar_p[0] = p[0] + Dc*cos((p[2]+bar_p[2])/2);
    bar_p[1] = p[1] + Dc*sin((p[2]+bar_p[2])/2);

    rot_1 = atan2(bar_p[1]-p[1],bar_p[0]-p[0])-p[2];
    trans = sqrt(((bar_p[1]-p[1])*(bar_p[1]-p[1])) + ((bar_p[0]-p[0])*(bar_p[0]-p[0])));
    rot_2 = bar_p[2] - p[2] - rot_1;

    drot_1 = rot_1 - (alpha[0]*(rot_1*rot_1) + alpha[1]*(trans*trans))*random_normal();
    dtrans = trans - (alpha[2]*(trans*trans) + alpha[3]*((rot_1*rot_1)+(rot_2*rot_2)))*random_normal();
    drot_2 = rot_2 - (alpha[0]*(rot_2*rot_2) + alpha[1]*(trans*trans))*random_normal();

    p2[i*3+0] = p[0] + dtrans*cos(p[2]+drot_1);
    p2[i*3+1] = p[1] + dtrans*sin(p[2]+drot_1);
    p2[i*3+2] = p[2] + drot_1 + drot_2;
}

/* Update Stage - Implements Ray Casting algorithm to simulare sonars in particles */
double mexRayCasting(double xc, double yc, double theta, double *map, mwSize map_M, mwSize map_N, double max_range,
                     double *angles, mwSize nAngles, mwSize mapscale, double accuracy, double *mu, double sigma,
                     double w_i)
{
    /* Code for ray cast */
    double *range, prob=1;
    mwSize i, end_flag = 0;
    range = malloc(nAngles * sizeof(double));
    

    xc *= (double)mapscale;
    yc *= (double)mapscale;
    max_range *= mapscale;

    /* If robot outside map */
    if((yc > map_M) || (xc > map_N) || (xc <= -1) || (yc <= -1)){
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

    for(i = 0; i<nAngles; i++){
        /* Compute direction */
        double dx, dy, x, y, xceil, yceil, dist;
        dx = accuracy*cos(theta+angles[i]);
        dy = accuracy*sin(theta+angles[i]);
        
        /* Initialize the ray */
        x = xc;
        y = yc;
        dist = 0;
        
        while(dist<=max_range){
            x += dx;
            y += dy;
            dist = sqrt(pow((x-xc),2) + pow((y-yc),2));
            xceil = ceil(x);
            yceil = ceil(y);
            
            
            if((xceil>map_N)||(yceil>map_M)||(xceil<0)||(yceil<0)){
                /* Outside */
                break;
            }
            
            if(map[(mwSize)xceil*map_M+(mwSize)yceil]==1){
                /* Hit */
                /*range[i] = dist;*/
                break;
            }
        }
        range[i] = dist;
    }

    for(i=0; i<nAngles; i++){
        range[i] /= mapscale;
    }
        
    prob = w_i*exp(-pow((range[0]-mu[0]),2)/(2*pow(sigma,2))) / sqrt(2*M_PI*pow(sigma,2));
    
    for(i=1; i<nAngles; i++)
    {
        prob *= exp(-pow((range[i]-mu[i]),2)/(2*pow(sigma,2))) / sqrt(2*M_PI*pow(sigma,2));
    }

    free(range);
    return prob;
}

/* Function to find maximum element in array */
double max2(double *arr, mwSize size)
{
    double max_elem=arr[0];
    mwSize i;
    
    for(i=1; i<size; i++)
    {
        if(arr[i] > max_elem)
            max_elem = arr[i];
    }
    
    return max_elem;
}

/* Resampling Stage - Implements sampling wheel algorithm for resampling */
void mexSamplingWheel(double *w, double *p, mwSize p_N, mwSize p_M)
{
    /* Code for Resampling */    
    double *p3, mw, beta = 0;
    mwSize index,i;
    p3 = malloc(p_M*p_N *sizeof(double));
    mw = max2(w, p_N);
    index = rand() % p_N;/* random from 0 to p_N-1 */
    
    for(i=0; i<p_N; i++)
    {
        beta += r2()*mw*2;
        while(beta>w[index])
        {
            beta -= w[index];
            index++;
            if(index > p_N-1)
            {
                index = 0;
            }
        }
             
        p3[i*3 + 0] = p[index*3 + 0];
        p3[i*3 + 1] = p[index*3 + 1];
        p3[i*3 + 2] = p[index*3 + 2];
    } 
        
    for(i=0; i<p_N; i++)
    {
        p[i*3 + 0] = p3[i*3 + 0];
        p[i*3 + 1] = p3[i*3 + 1];
        p[i*3 + 2] = p3[i*3 + 2];        
        w[i] = 1;
    }     
}

/* Main Particle Filter - Implements MATLAB's main particle filter loop */
void mexFP(double *p, double dtick_L, double dtick_R, double wheel_distance, mwSize encoder_ticks,double wheel_radius, double *alpha, double *p2/**/, 
           double *map, mwSize map_M, mwSize map_N, double max_range, double *angles, mwSize nAngles, mwSize mapscale, double accuracy, double *mu, double sigma, double *w, double *prob, mwSize p_M, mwSize p_N, mwSize k, double *PP)
{
    double total_weight = 0, p_temp[3], sum_row_p[3]={0,0,0};
    mwSize i, Np = p_N;
    for(i=0; i<Np; i++)
    {
        p_temp[0] = p[i*3+0];
        p_temp[1] = p[i*3+1];
        p_temp[2] = p[i*3+2];
        /* estimate movement according to encoders */
        mexEstimateProb(p_temp, dtick_L, dtick_R, wheel_distance, encoder_ticks, wheel_radius, alpha, i, p2);
        
        prob[i] = mexRayCasting(p2[i*3+0],p2[i*3+1],p2[i*3+2], map, map_M, map_N, max_range, angles, nAngles, mapscale, accuracy, mu, sigma, w[i]);
        total_weight += prob[i];
    }
    
    for(i=0; i<Np; i++)
    {
        prob[i] /= total_weight;
    }
    
    if ( (k%15) == 0 && k>10 )/* choose "-1" for open loop simulation of the Part. Filt */
    {
        mexSamplingWheel(prob, p2, p_N, p_M);
    } 
    
    for(i=0; i<Np; i++)
    {
        PP[i*3 + 0] = sum_row_p[i*3 + 0]/Np;
        PP[i*3 + 1] = sum_row_p[i*3 + 1]/Np;
        PP[i*3 + 2] = sum_row_p[i*3 + 2]/Np;
    }
    
    if (k>120)
    {
       sigma = 1;
    }
}

/* Entry function */
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
    double *w_t;        /* w */
    double *prob_t;     /* output vector */

    mwSize k_t;
    mwSize p_M_t;       /* M-size of p */
    mwSize p_N_t;       /* N-size of p */
    
    double *PP_t;       /* output vector */
    
    /* Check for parameters */
    if(nrhs!=16){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nrhs",
                          "Sixteen inputs required.");
    }
    if(nlhs!=4){
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nlhs",
                          "Four outputs required.");
    }

    p_t = mxGetPr(prhs[0]);
    p_M_t = mxGetM(prhs[0]);
    p_N_t = mxGetN(prhs[0]);
    dtick_L_t = mxGetScalar(prhs[1]);
    dtick_R_t = mxGetScalar(prhs[2]);
    wheel_distance_t = mxGetScalar(prhs[3]);
    encoder_ticks_t = mxGetScalar(prhs[4]);
    wheel_radius_t = mxGetScalar(prhs[5]);
    alpha_t = mxGetPr(prhs[6]);
    map_t = mxGetPr(prhs[7]);
    map_M = mxGetM(prhs[7]);
    map_N = mxGetN(prhs[7]);
    max_range_t = mxGetScalar(prhs[8]);
    angles_t = mxGetPr(prhs[9]);
    nAngles = mxGetN(prhs[9]);
    mapscale_t = mxGetScalar(prhs[10]);
    accuracy_t = mxGetScalar(prhs[11]);
    mu_t = mxGetPr(prhs[12]);
    sigma_t = mxGetScalar(prhs[13]);
    w_t = mxGetPr(prhs[14]);
    k_t = mxGetScalar(prhs[15]);
    
    /* Create the output vectors and scalar */
    plhs[0] = mxCreateDoubleMatrix(p_M_t,p_N_t,mxREAL);
    plhs[1] = mxCreateDoubleMatrix(1,p_N_t,mxREAL);
    plhs[2] = mxCreateDoubleMatrix(p_M_t,p_N_t,mxREAL);
    plhs[3] = mxCreateDoubleScalar(sigma_t);

    /* Get pointer to the real data in the output scalar */
    p2_t = mxGetPr(plhs[0]);
    prob_t = mxGetPr(plhs[1]);
    PP_t = mxGetPr(plhs[2]);
    
    /* Call the main computational routine */
    mexFP(p_t, dtick_L_t, dtick_R_t, wheel_distance_t, encoder_ticks_t, wheel_radius_t, alpha_t, p2_t/**/,
          map_t, map_M, map_N, max_range_t, angles_t, nAngles, mapscale_t, accuracy_t, mu_t, sigma_t, w_t, prob_t, p_M_t, p_N_t, k_t, PP_t);
}
