/**** Wander program for stage robot
* Based on Jan Stevens' code (http://www.fritz-hut.com/2013/01/31/particle-filter-implementation-in-player-stage/)
* and stage's wander example.
**************************************/

#include "stage.hh"
#include <vector>

using namespace Stg;
using namespace std;

typedef struct
{
  ModelPosition* pos;
  ModelRanger* laser;
  
  int avoidCount;
  
  vector<meters_t> sonars;
  
} robot_t;

Pose destination;
int destCnt = 0;
double thresh = 0.5;
double x_dest[] = {4,   1,   9, 6, 9,   9,   1, 1};
double y_dest[] = {1, 6.5, 6.5, 1, 1, 6.5, 6,5, 1};
//double a_dest[] = {M_PI/2, M_PI/2, -M_PI/2,    0,  M_PI/2, M_PI,  3*M_PI/2,    0};

int wander(Model* mod, robot_t* robot);
int poseUpdate(Model* mod, robot_t* robot);
	
// Stage calls this when the model starts up
extern "C" int Init( Model* mod, CtrlArgs* args )
{
  // local arguments
  /*  printf( "\nWander controller initialised with:\n"
      "\tworldfile string \"%s\"\n" 
      "\tcmdline string \"%s\"",
      args->worldfile.c_str(),
      args->cmdline.c_str() );
  */

  robot_t* robot = new robot_t;
  
  robot->avoidCount = 0;
  
  robot->pos = (ModelPosition*)mod;
  
  robot->pos->AddCallback( Model::CB_UPDATE, (model_callback_t)poseUpdate, robot );
  robot->pos->Subscribe(); // starts the position updates
  
  robot->laser = (ModelRanger*)mod->GetChild( "ranger:0" );
  robot->laser->AddCallback( Model::CB_UPDATE, (model_callback_t)wander, robot );
  robot->laser->Subscribe(); // starts the ranger updates
  
  // Drive
  robot->pos->SetXSpeed(1);	
  destination.x = 4;
  destination.y = 1;
  destination.a = 0.0;
  robot->pos->GoTo(destination);
  
  cout << "Size of x_dest: " << sizeof(x_dest)/sizeof(double) << endl;
  
  return 0; //ok
}

int wander(Model* mod, robot_t* robot){		
	// Drive
	Pose robotpose = robot->pos->GetGlobalPose();
	double d = (robotpose.x - destination.x)*(robotpose.x - destination.x) + (robotpose.y - destination.y)*(robotpose.y - destination.y);
	cout << "d = " << d << "\tdestCnt = " << destCnt << endl;
	if(d < thresh){
		destCnt++;
		destination.x = x_dest[destCnt];
		destination.y = y_dest[destCnt];
	}
	
	robot->pos->GoTo(destination);
	
	if(destCnt >= sizeof(x_dest)/sizeof(double) - 1)
		destCnt = -1;
	
	return 0; //run again
}

int poseUpdate(Model* mod, robot_t* robot){
	Pose pose = robot->pos->GetPose();

	printf( "Pose: [%.2f %.2f %.2f %.2f]\n",
	  pose.x, pose.y, pose.z, pose.a );	  


	
	return 0; // run again
}
