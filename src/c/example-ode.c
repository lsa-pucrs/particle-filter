/*
instalacao da lib:
$ sudo apt-get install libgsl0-dev

Compile and link with:
$ gcc -c ode_test.c
$ gcc -o ode_test ode_test.o -lgsl -lgslcblas -lm

Manual ODE lib GSL:
www.gnu.org/software/gsl/manual/html_node/Ordinary-Differential-Equations.html

Examples:
http://www.gnu.org/software/gsl/manual/html_node/ODE-Example-programs.html
https://www.physics.ohio-state.edu/~ntg/780/gsl_examples/ode_test.c
http://codedmi.com/questions/4207936/writing-runge-kutta-ode-solver-using-gsl
http://stackoverflow.com/questions/15813815/matlab-script-moved-to-c-but-with-different-output-for-ode45

stepping functions:
https://www.gnu.org/software/gsl/manual/html_node/Stepping-Functions.html

*/


#include <stdio.h>
#include <math.h>
//#include <gsl/gsl_sf_bessel.h>
//#include <gsl/gsl_matrix.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv2.h>

/* parameters used to calculate odometry of a differential car*/
struct diff_drive_car_params{
	double vr,vl,wheel_radius,distance_between_wheels;
};

/* odometry function of a differential car*/
int diff_drive_car (double t, const double y[], double f[],
      void *params)
{
  struct diff_drive_car_params diff_par = *(struct diff_drive_car_params *)params;
  // vr,vl,R,L
  /*estranho mandar um vetor de 3 posições e usar somente a ultima (y[2]). revisar*/
  f[0] = (diff_par.wheel_radius/2)*(diff_par.vr+diff_par.vl)*cos(y[2]);
  f[1] = (diff_par.wheel_radius/2)*(diff_par.vr+diff_par.vl)*sin(y[2]);    
  f[2] = (diff_par.wheel_radius/diff_par.distance_between_wheels)*(diff_par.vr-diff_par.vl);  
  return GSL_SUCCESS;
}

int main ( )
{
  // diferential car parameters
  struct diff_drive_car_params params;
  params.wheel_radius = 0.02;
  params.distance_between_wheels = 0.08;
  // change these two to change the path of the car
  params.vr = 1;
  params.vl = 1;
 
  //http://www.gnu.org/software/gsl/manual/html_node/Defining-the-ODE-System.html#Defining-the-ODE-System
  gsl_odeiv2_system sys;
  sys.function = diff_drive_car; /* the right-hand-side functions dy[i]/dt */
  sys.jacobian = NULL;	         /* the Jacobian df[i]/dy[j] */
  sys.dimension = 3;	         /* number of differential equations: x, y, th */
  sys.params = &params;	         /* parameters to pass to rhs and jacobian */
  
  // driver: gsl_odeiv2_driver_alloc_y_new
  //   https://www.gnu.org/software/gsl/manual/html_node/Driver.html
  // stepping functions: gsl_odeiv2_step_rkf45
  //   www.gnu.org/software/gsl/manual/html_node/Stepping-Functions.html
  gsl_odeiv2_driver * d = 
    gsl_odeiv2_driver_alloc_y_new (&sys, gsl_odeiv2_step_rkf45,1e-3, 1e-3, 0.0);
  
  int status;
  double pose[3] = { 1.0, 1.0, 1.0 };

  double t, t_next;		    /* current and next independent variable */
  double tmin, tmax, delta_t;	/* range of t and step size for output */

  t    = 0.;			/* starting t value */
  tmin = 0.;			/* starting t value */
  tmax = 270.;			/* final t value */
  delta_t = 0.5;

  /* initial values */
  printf ("%.5e: %.5e %.5e %.5e\n", t, pose[0], pose[1], pose[2]);	
  
  for (t_next = tmin + delta_t; t_next <= tmax; t_next += delta_t){
	  
      status = gsl_odeiv2_driver_apply (d, &t, t_next, pose);
      if (status != GSL_SUCCESS){
	    printf ("error, return value=%d\n", status);
	    break;
	  }

      printf ("%.5e: %.5e %.5e %.5e\n", t, pose[0], pose[1], pose[2]);
  }

  /* all done; free up the gsl_odeiv stuff */
  gsl_odeiv2_driver_free (d);
  return 0;  
  
  /*
  double x = 5.0;
  double y = gsl_sf_bessel_J0 (x);
  printf ("J0(%g) = %.18e\n", x, y);
  return 0;
  */
}
