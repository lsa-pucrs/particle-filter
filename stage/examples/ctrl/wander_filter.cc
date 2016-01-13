/**** Wander program for stage robot
* Based on Jan Stevens' code (http://www.fritz-hut.com/2013/01/31/particle-filter-implementation-in-player-stage/)
* and stage's wander example.
**************************************/

#include "stage.hh"
#include <vector>

using namespace Stg;
using namespace std;

static const meters_t avoidDistance = 0.5;
static const int avoidDuration = 10;
static const int maxSpeed = 1;
static const int maxTurn = 60;
static int avoidTurnSpeed = 50;

typedef struct
{
  ModelPosition* pos;
  ModelRanger* laser;
  
  int avoidCount;
  
  vector<meters_t> sonars;
  
} robot_t;

typedef struct{
	double x;
	double y;
	double a;
	double w;
} particles_t;

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
  
  return 0; //ok
}

int wander(Model* mod, robot_t* robot){
	double fSpeed, tSpeed, currDistance = 0.0;
	bool avoidRight = false, avoidLeft = false;
	vector<particles_t> particles;
	const int Np = 100;
	
	/*Put particles in random positions
	for(uint8_t i = 0; i<Np; i++){
		particles_t particle;
		particle.x = (double)rand()/(RAND_MAX)*(15)-7.5; //TODO: Mudar para tamanho do mapa
		particle.y = (double)rand()/(RAND_MAX)*(8)-4;	  //Aqui também
		particle.a = (double)rand()/(RAND_MAX) * 2.0 * M_PI;
		particle.w = 0.0;
		
		particles.push_back(particle);
	}*/
	
	//Get sample_count
	uint8_t sample_count = robot->laser->GetSensors().size();
	if(sample_count == 0){
		return 0;
	}
	
	//Print some debug data
	printf("---- Some debug data ----\nsample_count: %d\n", sample_count);
	printf("avoidDistance: %.2f\n", avoidDistance);
	
	//Check for obstacles
	for(uint32_t i = 0; i < sample_count; i++) {
		vector<meters_t> scan = robot->laser->GetSensors()[i].ranges;	//Get sensors
        robot->sonars.push_back(scan[0]);
        currDistance = robot->sonars[i];
        printf("scan[%d]: %.2f\n", i, currDistance);
        if(currDistance < avoidDistance) {
            if(i >= 2){
				avoidLeft = true;
			}else{
				avoidRight = true;
			}
		}
    }
	
	robot->sonars.clear();
	if( avoidLeft || avoidRight ){		//Near obstacle
		robot->pos->SetXSpeed(0.0);
		printf("Avoiding obstacle, count %d\n", robot->avoidCount);
		if(robot->avoidCount < 1){
			if(avoidLeft){
				robot->avoidCount = rand() % avoidDuration + avoidDuration;
				robot->pos->SetTurnSpeed(avoidTurnSpeed);
				avoidLeft = false;
			} else if(avoidRight){
				robot->avoidCount = rand() % avoidDuration + avoidDuration;
				robot->pos->SetTurnSpeed(-avoidTurnSpeed);
				avoidRight = false;
			}
		}
		robot->avoidCount--;
	} else{					//Not near obstacle
		fSpeed = maxSpeed;
		//tSpeed is between -maxTurn and +maxTurn
		tSpeed = rand()%(2*maxTurn);
		tSpeed -= maxTurn;
		robot->pos->SetXSpeed(fSpeed);	 
		robot->pos->SetTurnSpeed(tSpeed);
	}
	
	return 0; //run again
}

int poseUpdate(Model* mod, robot_t* robot){
	Pose pose = robot->pos->GetPose();

	printf( "Pose: [%.2f %.2f %.2f %.2f]\n",
	  pose.x, pose.y, pose.z, pose.a );

	return 0; // run again
}
