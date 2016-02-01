/*
 * MAT-file creation program
 *
 * See the MATLAB External Interfaces/API Guide for compiling information.
 *
 * Calling syntax:
 *
 *   matcreat
 *
 * Create a MAT-file which can be loaded into MATLAB.
 *
 * This program demonstrates the use of the following functions:
 *
 *  matClose
 *  matGetVariable
 *  matOpen
 *  matPutVariable
 *  matPutVariableAsGlobal
 *
 * Copyright 1984-2005 The MathWorks, Inc.
 * http://note.sonots.com/Mex/Matrix.html
 */
 
#include <stdio.h>
#include <string.h> /* For strcmp() */
#include <stdlib.h> /* For EXIT_FAILURE, EXIT_SUCCESS */
#include <math.h>
#include "mat.h"
#include "map.h"
#include "matrix.h"

#define BUFSIZE 256
#define NP 50
#define K 540
//#define DEBUG


/* Map */
int mapscale = 120; // each grid in 'map' represents 1/mapscale meters
unsigned char *map_opt;
int col_opt;
double *map;
int map_M, map_N;

/* Initial conditions */
double *x;
double *y;
double *th;
double varx = 0.01;
double vary = 0.01;
double vart = 0.01;

/* Parameters */
double T  = .5;     			// sampling time
double wheel_radius = 0.02;   	// wheel radius 2cm
double wheel_distance = 0.08; 	// distance between wheels 8cm
int encoder_ticks = 48; 		//encoder ticks per rotation (must be a multiple of 720)
double max_range = 1; 			// Tem que ser double
double angles[5] = {0*M_PI/180, 90*M_PI/180, -90*M_PI/180, 45*M_PI/180, -45*M_PI/180};
int nAngles = 5;

/* Particle Filter parameters */
double alpha[4] = {2,50,50,2};
double *w, *p, *PP;
int p_M=3, p_N=50;

/* Dynamic Variables */ 
double *sonars, *dtick_L, *dtick_R, sigma;

double vr=1;
double vl=1;

/*
 * this part gets the byte: map_opt[i*col_opt+(j/8)]
 * get the bit: (1<<(7-(j%8))) 
 * make it return 0 or 1 : "'1' : '0'"
 */
#define GETPIXEL(i,j) (   ( (  map_opt[i*col_opt+(j/8)] & (1<<(7-(j%8)))  ) != 0 ) ? 1 : 0    )


int setCarPosition()
{
	MATFile *pmat;
	mxArray *x_t, *y_t, *th_t;
	const char *file = "car.mat";
	char str[BUFSIZE];
	int status; 
			
	/*
	* Open file and verify its contents with matGetVariable
	*/
	pmat = matOpen(file, "r");
	if (pmat == NULL) 
	{
		printf("Error opening file %s\n", file);
		return(EXIT_FAILURE);
	}
	
	/*
	* Create a vector to the variables
	*/	
	x_t= mxCreateDoubleMatrix(1,541,mxREAL);
	if (x_t == NULL)
	{
	  printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
	  printf("Unable to create mxArray.\n");
	  return(EXIT_FAILURE);
	}
	
	y_t= mxCreateDoubleMatrix(1,541,mxREAL);
	if (y_t == NULL)
	{
	  printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
	  printf("Unable to create mxArray.\n");
	  return(EXIT_FAILURE);
	}
	
	th_t= mxCreateDoubleMatrix(1,541,mxREAL);
	if (th_t == NULL)
	{
	  printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
	  printf("Unable to create mxArray.\n");
	  return(EXIT_FAILURE);
	}
	
	/*
	* Copy an existing array in a MAT-file.
	*/
	x_t = matGetVariable(pmat, "x");
	if (x_t == NULL) {
		printf("Error reading existing matrix LocalDouble\n");
		return(EXIT_FAILURE);
	}
	else
	{
		x = mxGetPr(x_t);
		/*
		int i;
		printf("------------ x ------------  \n");
		for(i=0;i<mxGetM(x_t)*mxGetN(x_t);i++)
		{
			printf("%f\n", x[i]);
		}*/
	}
	
	y_t = matGetVariable(pmat, "y");
	if (y_t == NULL) {
		printf("Error reading existing matrix LocalDouble\n");
		return(EXIT_FAILURE);
	}
	else
	{
		y = mxGetPr(y_t);
		
		/*
		int i;
		printf("------------ y ------------  \n");
		for(i=0;i<mxGetM(y_t)*mxGetN(y_t);i++)
		{
			printf("%f\n", y[i]);
		}*/
	}
	
	th_t = matGetVariable(pmat, "th");
	if (th_t == NULL) {
		printf("Error reading existing matrix LocalDouble\n");
		return(EXIT_FAILURE);
	}
	else
	{
		th = mxGetPr(th_t);
		/*
		int i;
		printf("------------ th ------------  \n");
		for(i=0;i<mxGetM(th_t)*mxGetN(th_t);i++)
		{
			printf("%f\n", th[i]);
		}*/
	}
	
	/*
	* Close file
	*/	
	if (matClose(pmat) != 0) 
	{
		printf("Error closing file %s\n",file);
		return(EXIT_FAILURE);
	}
	return(EXIT_SUCCESS);
}

int setDynamicVariables()
{
	MATFile *pmat;
	mxArray *sonars_t, *sigma_t;
	const char *file = "dynamic_param.mat";
	char str[BUFSIZE];
	int status; 
			
	/*
	* Open file and verify its contents with matGetVariable
	*/
	pmat = matOpen(file, "r");
	if (pmat == NULL) 
	{
		printf("Error opening file %s\n", file);
		return(EXIT_FAILURE);
	}
	
	/*
	* Create a vector to the variables
	*/
	sonars_t = mxCreateDoubleMatrix(1,K,mxREAL);
	if (sonars_t == NULL)
	{
	  printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
	  printf("Unable to create mxArray.\n");
	  return(EXIT_FAILURE);
	}
		
	/*
	* Copy an existing array in a MAT-file.
	*/
	sonars_t = matGetVariable(pmat, "see_sonar");
	if (sonars_t == NULL) 
	{
		printf("Error reading existing matrix LocalDouble\n");
		return(EXIT_FAILURE);
	}
	else
	{
		sonars = mxGetPr(sonars_t);
		/*
		int i;
		printf("------------ sonars ------------  \n");
		for(i=0;i<mxGetM(sonars_t)*mxGetN(sonars_t);i++)
		{
			printf("%f\n", sonars[i]);
		}*/
	}

	sigma_t = matGetVariable(pmat, "CovSonars");
	if (sigma_t == NULL) 
	{
		printf("Error reading existing matrix LocalDouble\n");
		return(EXIT_FAILURE);
	}
	else
	{
		sigma = mxGetScalar(sigma_t);		
	}

	/*
	* Close file
	*/
	if (matClose(pmat) != 0) 
	{
		printf("Error closing file %s\n",file);
		return(EXIT_FAILURE);
	}
	return(EXIT_SUCCESS);
}

int setStaticVariables()
{
	MATFile *pmat;
	mxArray *map_t;
	const char *file = "set_value.mat";
	char str[BUFSIZE];
	int status; 
			
	/*
	* Open file and verify its contents with matGetVariable
	*/
	pmat = matOpen(file, "r");
	if (pmat == NULL) 
	{
		printf("Error opening file %s\n", file);
		return(EXIT_FAILURE);
	}
	
	/*
	* Create a vector to the variables
	*/	
	map_t= mxCreateDoubleMatrix(240,300,mxREAL);
	if (map_t == NULL)
	{
	  printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
	  printf("Unable to create mxArray.\n");
	  return(EXIT_FAILURE);
	}
	
	/*
	* Copy an existing array in a MAT-file.
	*/
	map_t = matGetVariable(pmat, "map");
	if (map_t == NULL) {
		printf("Error reading existing matrix LocalDouble\n");
		return(EXIT_FAILURE);
	}
	else
	{
		map = mxGetPr(map_t);
		map_M = mxGetM(map_t);
		map_N = mxGetN(map_t);
	}
	
	/*
	* Close file
	*/	
	if (matClose(pmat) != 0) 
	{
		printf("Error closing file %s\n",file);
		return(EXIT_FAILURE);
	}
	return(EXIT_SUCCESS);
}

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

void estimateProb(int i, int k)
{
    /* Code for estimate probability */
    double Dl, Dr, Dc, bar_p[3],rot_1, trans, rot_2, drot_1, dtrans, drot_2;
    Dl = 2*M_PI*wheel_radius*dtick_L[k]/encoder_ticks;
    Dr = 2*M_PI*wheel_radius*dtick_R[k]/encoder_ticks;
    Dc = (Dl+Dr)/2;
	
    bar_p[2] = p[i*3+2] + (Dr-Dl)/wheel_distance;
    bar_p[0] = p[i*3+0] + Dc*cos((p[i*3+2]+bar_p[2])/2);
    bar_p[1] = p[i*3+1] + Dc*sin((p[i*3+2]+bar_p[2])/2);
	
    rot_1 = atan2(bar_p[1]-p[i*3+1],bar_p[0]-p[i*3+0])-p[i*3+2];
    trans = sqrt(((bar_p[1]-p[i*3+1])*(bar_p[1]-p[i*3+1])) + ((bar_p[0]-p[i*3+0])*(bar_p[0]-p[i*3+0])));
    rot_2 = bar_p[2] - p[i*3+2] - rot_1;
		
    drot_1 = rot_1 - (alpha[0]*(rot_1*rot_1) + alpha[1]*(trans*trans))*random_normal();
    dtrans = trans - (alpha[2]*(trans*trans) + alpha[3]*((rot_1*rot_1)+(rot_2*rot_2)))*random_normal();
    drot_2 = rot_2 - (alpha[0]*(rot_2*rot_2) + alpha[1]*(trans*trans))*random_normal();
	
    p[i*3+0] = p[i*3+0] + dtrans*cos(p[i*3+2]+drot_1);
    p[i*3+1] = p[i*3+1] + dtrans*sin(p[i*3+2]+drot_1);
    p[i*3+2] = p[i*3+2] + drot_1 + drot_2;
}

void rayCasting(double xc, double yc, double theta, double accuracy, int j, int k)
{
    /* Code for ray cast */
    double *range;
    int i, end_flag = 0;
    range = malloc(nAngles * sizeof(double));
    
    xc *= (double)mapscale;
    yc *= (double)mapscale;
    max_range *= mapscale;

    /* If robot outside map */
    if((yc > map_M) || (xc > map_N) || (xc <= -1) || (yc <= -1))
    {
        for(i=0; i<nAngles; i++)
            range[i] = 0;
        end_flag = 1;
    }
    
    /* If the robot is on top of an occupied area */
    // if(map[(int)ceil(xc)*map_M + (int)ceil(yc)]==1)
    if(GETPIXEL((int)ceil(xc),(int)ceil(yc)) ==1)
    {
        for(i=0; i<nAngles; i++)
            range[i] = 0;
        end_flag = 1;
    }
    
    if(!end_flag){
		for(i = 0; i<nAngles; i++)
		{
			/* Compute direction */
			double dx, dy, x, y, xceil, yceil, dist;
			dx = accuracy*cos(theta+angles[i]);
			dy = accuracy*sin(theta+angles[i]);
			
			/* Initialize the ray */
			x = xc;
			y = yc;
			dist = 0;
			
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
				
				// if(map[(int)xceil*map_M+(int)yceil]==1)
				if(GETPIXEL((int)xceil,(int)yceil)==1)
				{
					/* Hit */
					range[i] = dist;
					break;
				}
			}    
		}
	}

	for(i=0; i<nAngles; i++)
	{
		range[i] /= mapscale;
		
	}
    
    for(i=0; i<nAngles; i++)
    {
        w[j] *= exp(-pow((range[i]-sonars[k*5+i]),2)/(2*pow(sigma,2))) / sqrt(2*M_PI*pow(sigma,2));
    }
	
    free(range);
}

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

void samplingWheel()
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
    
    free(p3);
}

void particleFilter(int k)
{
    double total_weight = 0, p_temp[3], sum_row_p[3]={0,0,0};
    int i;
    
    for(i=0; i<NP; i++)
    {      
        /* estimate movement according to encoders */
        estimateProb(i, k);
		
        rayCasting(p[i*3+0],p[i*3+1],p[i*3+2],1,i,k);
        total_weight += w[i];
    }

	
    for(i=0; i<NP; i++)
    {
        w[i] /= total_weight;
    }
 
 
    if ((k%15) == 0 && k>10 ) /* choose "-1" for open loop simulation of the Part. Filt */
    {
        samplingWheel();
    }

	  
    for(i=0; i<NP; i++)
    {
        PP[i*3 + 0] = sum_row_p[0]/NP;
        PP[i*3 + 1] = sum_row_p[1]/NP;
        PP[i*3 + 2] = sum_row_p[2]/NP;
    }
    
}

void fdraw_p()
{
	double xceil, yceil;
	double temp1=0, temp2=0, temp3=0;
	int i;

	for (i=0; i<p_N; i++) //Number of particles
	{

		p[i*3+0] = x[0] + (r2()-0.5)*varx;
		p[i*3+1] = y[0] + (r2()-0.5)*vary;
		p[i*3+2] = th[0] + (r2()-0.5)*vart;

		xceil = ceil(p[i*3+0]);
		yceil = ceil(p[i*3+1]);			
		
		while ((xceil>map_N)||(yceil>map_M)||(xceil<0)||(yceil<0))
		{
			p[i*3+0] = x[0] + (r2()-.5)*varx;
			p[i*3+1] = y[0] + (r2()-.5)*vary;
			p[i*3+2] = th[0] + (r2()-.5)*vart;  
		}
		
		temp1 += p[i*3+0]; 
		temp2 += p[i*3+1]; 
		temp3 += p[i*3+2]; 
	}
	
	PP[p_M*0] = temp1/p_N;
	PP[p_M*1] = temp2/p_N;
	PP[p_M*2] = temp3/p_N;
}

void F_encoders(double dx, double dy, double dth, int pos)
{
	static double angL=0, angR=0;
	int ang;
	double tick_R = 0;
	double tick_L = 0;
	
	/*
	 * 
	 * dx = x(k+1)-x(k)
	 * dy = y(k+1)-y(k)
	 * dth = th(k+1)-th(k)
	 * L = wheel_distance
	 * N = encoder_ticks
	 * R = wheel_radius
	 * 
	 * */

	//calculate the distance traveled by each wheel
	double D = sqrt( dx*dx + dy*dy);
	double Dr = (D + wheel_distance/2*dth);
	double Dl = (D - wheel_distance/2*dth);
	
	
	// converts that to degrees
	double angR_end = angR + round(((Dr/wheel_radius)*2*180)/M_PI);
	double angL_end = angL + round(((Dl/wheel_radius)*2*180)/M_PI);
	

	// right wheel
	if ((angR_end - angR) > 0) //positive rotation
	{
		for (ang=angR+1; ang<=(int)angR_end; ang++)
		{
			if ( (ang % (2*360/encoder_ticks)) == 0)
			{
				
				tick_R = tick_R + 1;
			}
		}
	}
	else if ((angR_end - angR) < 0) //negative rotation
	{
		for (ang=angR+1; ang<=(int)angR_end; ang++)
		{
			if ( (ang % (2*360/encoder_ticks)) == 0 )
			{
				tick_R = tick_R - 1;
			}
		}
	}

	
	// left wheel
	if ((angL_end - angL) > 0 )// positive rotation
	{
		for (ang=angL+1; ang<=(int)angL_end; ang++)
		{
			if ( (ang % (2*360/encoder_ticks) ) == 0 )
			{
				tick_L = tick_L + 1;
			}
		}
	}
	else if((angL_end - angL) < 0 )// negative rotation
	{
		for (ang=angL+1; ang<=(int)angL_end; ang++)
		{
			if ( (ang % (2*360/encoder_ticks)) == 0 )
			{
				tick_L = tick_L - 1;
			}
		}
	}  
	angL = angL_end;
	angR = angR_end;

	dtick_L[pos] = tick_L;
	dtick_R[pos] = tick_R;
}

void ode23(int k)
{
	th[k+1] = th[k] + (vr-vl)*T*wheel_radius/wheel_distance;
	x[k+1]  = x[k] + (vr+vl)*T*cos(th[k+1])*wheel_radius/2;
	y[k+1]  = y[k] + (vr+vl)*T*sin(th[k+1])*wheel_radius/2;
	printf("%f %f %f\n", x[k], y[k], th[k]);
	
}


void fastRayCasting(double xc, double yc, double theta, double accuracy, int k, int sizek)
{
    /* Code for ray cast */
    double *range;
    int i, end_flag = 0;
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
    // if(map[(int)ceil(xc)*map_M + (int)ceil(yc)]==1)
    if(GETPIXEL((int)ceil(xc),(int)ceil(yc)) ==1){
        for(i=0; i<nAngles; i++)
            range[i] = 0;
        end_flag = 1;
    }
    
    if(!end_flag){
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
				
				// if(map[(int)xceil*map_M+(int)yceil]==1)
				if(GETPIXEL((int)xceil,(int)yceil)==1){
					/* Hit */
					range[i] = dist;
					break;
				}
			}    
		}
	}

	for(i=0; i<nAngles; i++)
	{
		range[i] /= mapscale;
		sonars[sizek*i+k] = range[i];
	}

    free(range);
}

void F_estimate_p(double *p2, int k)
{
	double Dl, Dr, Dc, bar_p[3];
    Dl = 2*M_PI*wheel_radius*dtick_L[k]/encoder_ticks;
    Dr = 2*M_PI*wheel_radius*dtick_R[k]/encoder_ticks;
    Dc = (Dl+Dr)/2;
	
    bar_p[2] = p2[2] + (Dr-Dl)/wheel_distance;
    bar_p[0] = p2[0] + Dc*cos((p[2]+bar_p[2])/2);
    bar_p[1] = p2[1] + Dc*sin((p[2]+bar_p[2])/2);
}

int main() 
{	
	int i, k;
	int ttotal = 270;
	int temp = ttotal/T;
	//double T = 0.5; // sampling time
	
	p 	= malloc(3*NP*sizeof(double));		
	PP 	= malloc(3*NP*sizeof(double));
	w 	= malloc(NP *sizeof(double));
	dtick_L = malloc(3*540*sizeof(double));
	dtick_R = malloc(3*540*sizeof(double));
	
	x = malloc(540*sizeof(double));
	y = malloc(540*sizeof(double));
	th = malloc(540*sizeof(double));
	
	for(i=0; i<540; i++){
		x[i] = 0;
		y[i] = 0;
		th[i] = 0;
	}
		
	for(i=0; i<NP; i++){
		w[i] = 1;
	}
	
	for(i=0; i<3*NP; i++){
		p[i] = 0;
		PP[i] = 0;
	}
	
	srand(time(NULL));
	setDynamicVariables();
	setStaticVariables();
	setCarPosition();
	fdraw_p();
	
	/* *************** Map *************** */    
	
#ifdef DEBUG
	for(i=0 ; i < map_N; i++){ // N: numero de linhas 
		for(j=0; j<map_M; j++){ // M: numero de colunas
			printf("%d ", (int) map[i*(map_M)+j]);
		}
		printf("\n");
	}
#endif

    unsigned char byte;
	
	col_opt = (map_N+(map_N%8))/8;
	map_opt = malloc(map_M*col_opt*sizeof(char));
	
	if (map_opt == NULL)
	{
		 printf("nao aloco");
	}
	
	memset(map_opt,0,map_M*col_opt*sizeof(char));
			
	col_opt = resize_map(map, map_opt, map_N, map_M);

#ifdef DEBUG
    for(i = 0 ; i < map_N; i++){
        for(j = 0 ; j < col_opt ; j++){
			byte = map_opt[i * col_opt + j];
			for(k = 7 ; k >= 0 ; k--){
				printf("%c ", (byte & (1<<k)) ? '1' : '0' );
			}
		}
        printf("\n");
    }
#endif

#ifdef DEBUG
	for(i = 0 ; i < map_N ; i++){
        for(j = 0 ; j < map_M ; j++){
			printf("%d ",  GETPIXEL(i,j));
		}
        printf("\n");
    }
#endif
	
	/* ************ End - Map ************ */
	
	MATFile *pmat;
	mxArray *p_temp;
	const char *file = "t_file.mat";
	char str[BUFSIZE];
	int status; 
	
	p_temp = mxCreateDoubleMatrix(3,NP,mxREAL);
	
	
	pmat = matOpen(file, "w");
	if (pmat == NULL)
	{
		printf("Error creating file %s\n", file);
		printf("(Do you have write permission in this directory?)\n");
		return(EXIT_FAILURE);
	}
	
	p_temp = mxCreateDoubleMatrix(3,NP,mxREAL);
	
	int u=0, d=0, c=0;
	char aux[4] = "p";
	int row, col;
	
	for (k=0; k<temp; k++)
	{
		if (k == 10)  vl=0.7;
		if (k == 60)  vl=1.0;
		if (k == 120) vr=0.7;
		if (k == 170) vr=1.0;
		if (k == 310) vr=0.5;
		if (k == 345) vr=1.0;
		if (k == 430) vl=0.7;
		if (k == 488) vl=1.001;
		/* ------------------- actual car  ------------------- */
		ode23(k);
		/* ---------------  End - actual car  ---------------- */
		
		
		
		/* ------------------- sonars ------------------- */
		fastRayCasting(x[k+1],y[k+1],th[k+1], 1, k, temp);
		/* ---------------  End - sonars ---------------- */
		
		
		
		/* ------------------- encoders ------------------- */
		F_encoders(x[k+1]-x[k],y[k+1]-y[k],th[k+1]-th[k], k);
		
		if ( (k%50) == 0 ){  // this if perturbs the encoder measurements		
			dtick_L[k] = dtick_L[k] + 1;
		}
		
		double odo[3] = {x[k], y[k], th[k]};
		F_estimate_p(odo, k); 
		/* ---------------  End - encoders ---------------- */
		
		
		
		/* ------------------- particle filter ------------------- */
		particleFilter(k);
		/* ---------------  End - particle filter ---------------- */
		
		
		/* Gravacao das partículas de saída */
		memcpy((void *)(mxGetPr(p_temp)), (void *)p, p_M*p_N*sizeof(p));
		
		u++;
		if(u>9)
		{
			d++;
			u=0;			
		}

		if(d>9)
		{
			c++;
			d=0;
		}
		
		if(c!=0)
		{
			aux[1]=(char)c+48;
			aux[2]=(char)d+48;
			aux[3]=(char)u+48;
		}
		else if(d!=0)
		{
			aux[1]=(char)d+48;
			aux[2]=(char)u+48;
		}
		else
		{			
			aux[1]=(char)u+48;
		}
		
		status = matPutVariable(pmat, aux, p_temp);
		if (status != 0) 
		{
			printf("Error using matPutVariableAsGlobal\n");
			return(EXIT_FAILURE);
		}
	}
	
	/*
	for(i=0; i<540; i++){
		printf("%f \n", x[i]);
	}
	printf("\n\n");
	for(i=0; i<540; i++){
		printf("%f \n", y[i]);
	}*/
	
	if (matClose(pmat) != 0) 
	{
		printf("Error closing file %s\n",file);
		return(EXIT_FAILURE);
	}
	
	return(EXIT_SUCCESS);
}
