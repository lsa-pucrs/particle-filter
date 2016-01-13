/**** Wander program for stage robot
* Based on Jan Stevens' code (http://www.fritz-hut.com/2013/01/31/particle-filter-implementation-in-player-stage/)
* and stage's wander example.
**************************************/

#include "stage.hh"
using namespace Stg;

static const meters_t avoidDistance = 1.5;
static const int avoidDuration = 10;
static const int maxSpeed = 1;
static const int maxTurn = 60;

typedef struct
{
  ModelPosition* pos;
  ModelRanger* laser;
  
  int avoidCount;
  
} robot_t;

//Poses for driving around
Pose firstPose;
Pose secondPose;
Pose thirdPose;
Pose fourthPose;
Pose fifthPose;
Pose sixthPose;
Pose seventhPose;
Pose eightPose;
Pose ninthPose;

int laserUpdate(Model* mod, robot_t* robot);
int drive(Model* mod, robot_t* robot);
	
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
  
  robot->pos->AddCallback( Model::CB_UPDATE, (model_callback_t)drive, robot );
  robot->pos->Subscribe(); // starts the position updates
  
  robot->laser = (ModelRanger*)mod->GetChild( "ranger:0" );
  robot->laser->AddCallback( Model::CB_UPDATE, (model_callback_t)laserUpdate, robot );
  robot->laser->Subscribe(); // starts the ranger updates
  
  //Set initial speeds
  robot->pos->SetXSpeed(0.8);
  robot->pos->SetTurnSpeed(0);
  
  //Define poses
  firstPose.x = -4.0; firstPose.y = -5.0;
  secondPose.x = -3.48; secondPose.y = -4.27;
  return 0; //ok
}

int laserUpdate(Model* mod, robot_t* robot){
	//Get sensors
	const std::vector<meters_t>& scan = robot->laser->GetSensors()[0].ranges;
	
	//Get sample_count
	uint8_t sample_count = scan.size();
	if(sample_count == 0){
		return 0;
	}
	
	//Print some debug data
	printf("---- Some debug data ----\nsample_count: %d\n", sample_count);
	printf("avoidDistance: %.2f\n", avoidDistance);
	for(uint32_t i = 0; i < sample_count; i++)
        printf("scan[%d]: %.2f\n", i, scan[i]);
	
	return 0; //run again
}

int drive(Model* mod, robot_t* robot){
	Pose pose = robot->pos->GetPose();

	printf( "Pose: [%.2f %.2f %.2f %.2f]\n",
	  pose.x, pose.y, pose.z, pose.a );

	//Check robot pose
	if(pose.x >= firstPose.x && pose.y >= firstPose.y){
		robot->pos->SetTurnSpeed(1);
	}else if(pose.a >= firstPose.a){
		robot->pos->SetTurnSpeed(0);
	}else if(pose.x == thirdPose.x && pose.y == thirdPose.y){
	}else if(pose.x == fourthPose.x && pose.y == fourthPose.y){
	}else if(pose.x == fifthPose.x && pose.y == fifthPose.y){
	}else if(pose.x == sixthPose.x && pose.y == sixthPose.y){
	}else if(pose.x == seventhPose.x && pose.y == seventhPose.y){
	}else if(pose.x == eightPose.x && pose.y == eightPose.y){
	}else if(pose.x == ninthPose.x && pose.y == ninthPose.y){
	}
	
	return 0; // run again
}
