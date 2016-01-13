/*
 This Stage control automatically generates a number of particle models to each robot.
 For animation purposes, each particle can be moved with GetGlobalPose() method
 Author: Alexandre Amory (amamory @ gmail com)
*/
#include "stage.hh"
using namespace Stg;
using namespace std;

// pass parameter to the callback function
typedef struct
{
  double floorplan_size_x;
  double floorplan_size_y;
  string robot_name;
  int number_particles;
} robot_t;

// forward declare
int PositionUpdate( ModelPosition* pos, robot_t* robot );
void split( const std::string& text, const std::string& separators, std::vector<std::string>& words);

// static variables
// each robot in the simulation has its own previous pose
std::map< string, Pose> previous_pose;

// Stage calls this when the model starts up
extern "C" int Init( Model* mod, CtrlArgs* args )
{
  World* w = mod->GetWorld();
  char particle_name[50];
  string robot_name;
  meters_t floorplan_size_x, floorplan_size_y;
  int particles = 0;
  int i;

  // tokenize the worldfile ctrl argument string into words
  std::vector<std::string> words;
  split( args->worldfile, std::string(" \t"), words );			
  // expecting the number of particles
  if (words.size() != 4){
    printf("\n\nAn exception occurred.\n Usage: random_particles <number of particles> <robot_name> <flooplan_name>\n\n");
    assert( 0 );
    return 1;
  }
  
  // the 2nd word must be the number of particles
  if (sscanf( words[1].c_str(), "%d", &particles) == 0){
    printf("\n\nAn exception occurred.\n Usage: random_particles <number of particles> <robot_name>\n\n");
    assert( 0 );
    return 1;
  }
  if (particles <= 0 || particles > 999){
    printf("\n\nAn exception occurred.\n Usage: random_particles <number of particles> <robot_name>.\nNumber of particles must be between 1 and 999.\n\n");
    assert( 0 );
    return 1;
  }
  
  // check the robot name
  Model* rn = w->GetModel(words[2]);
  if (rn == 0){
    printf("\n\nAn exception occurred.\n robot not found.\n\n");
    assert( 0 );
    return 1;
  }
  robot_name = words[2];
  
  // get the flooplan model to figure out its size
  Model* fp = w->GetModel(words[3]);
  if (fp == 0){
    printf("\n\nAn exception occurred.\n flooplan not found.\n\n");
    assert( 0 );
    return 1;
  }
  // the size is reduce so it is possible to check when a particle is out of bound
  floorplan_size_x = fp->GetGeom().size.x * 0.95;
  floorplan_size_y = fp->GetGeom().size.y * 0.95;
  printf("\n\nFloorplan size is %03f x %03f\n", floorplan_size_x, floorplan_size_y);
  
  // this part does not do much. just some testing
  /*
  Model* child = mod->GetWorld()->GetModel( "p000" );		
  assert(child);
  printf("\n\n\nmodel id: %d \n\n\n", child->GetId());
  child->Print("puck");
  child->SetPose( Pose::Random( -8, 8, -8, 8 ) );
  printf("model pose: %s\n", child->GetGlobalPose().String().c_str());
  printf("model type: %s\n", child->GetModelType().c_str());
  child->SetGeom( Geom( Pose(), Stg::Size( 0.1, 0.1, 0.1 ) ));
  //child->SetColor( Color(0,0,1,1) );  // blue
  */
  /*
  // tentativa de pegar o nome do world filename. apesar de filename ser publico eu nao consigo acessa-lo
  Worldfile *wf = child->GetWorld()->GetWorldFile();
  printf("model filename: %s\n", wf->filename.c_str());
  printf("model filename: %s\n", child->GetWorld()->GetWorldFile()->filename.c_str());
  */
  /*
  Model* child = mod->GetWorld()->LoadModel(Worldfile* wf, int entity );
  World * w = mod->GetWorld();
  Model* new_model = new Model(w,child->Root(),child->GetModelType(), "p001" );
  w->AddModel(new_model);
  */

  // every time the robot moves, the particle's position must be updated
  robot_t* info = new robot_t;
  info->floorplan_size_x = floorplan_size_x;
  info->floorplan_size_y = floorplan_size_y;
  info->robot_name = robot_name;
  info->number_particles = particles;
  rn->AddCallback( Model::CB_UPDATE, (model_callback_t)PositionUpdate, info );
  rn->Subscribe();
  
  printf( "\n\nCreating %d random particles for robot %s\n", particles, robot_name.c_str() );
  // create the particles at random positions
  for(i=0; i<particles; i++ )
  {
		sprintf(particle_name, "%s_p%03d", robot_name.c_str(), i);
		Model* new_model = new Model(w,NULL,"model", particle_name );
		// the particles will have the collor of the dummy model
		new_model->SetColor(mod->GetColor());
		// set the particle size to the dummy model size
		//new_model->SetGeom( Geom( Pose(), Stg::Size( 0.1, 0.1, 0.01 ) ));
		new_model->SetGeom(mod->GetGeom());
		// each particle have a random initial position within the floorplan area
		new_model->SetGlobalPose( Pose::Random( -(floorplan_size_x/2), (floorplan_size_x/2), -(floorplan_size_y/2), (floorplan_size_y/2) ) );
		printf("particle: %s; pose: %s\n", particle_name, new_model->GetGlobalPose().String().c_str());
		// set the particle to be transparent in the simulation
        new_model->SetBlobReturn (false);
        new_model->SetBoundary (false);
        new_model->SetFiducialReturn (0);
        new_model->SetGripperReturn (false);
        new_model->SetGuiMove (true);
        new_model->SetGuiNose (false);
        new_model->SetObstacleReturn (false);
        new_model->SetRangerReturn (-1.0);

	 }
  return 0; //ok
}
/*
 update particle's position based on the robot's position
 - If the particle is outside the floorplan area, then randomly place it back into the floorplan
 - the particle are found by name, using the pattern <robot_name>_pXXX
 - como saber a posicao anterior do robo p setar a posicao das particulas
*/
int PositionUpdate( ModelPosition* pos, robot_t* robot )
{  
	 Pose pose = pos->GetPose();
	 char particle_name[50];
	 int i;
	 Model *p; // particle pointer

	 //static Pose previous_pose;
	 static std::map< string, Pose> previous_pose;
	 // each robot has its own previous pose
	 Pose ppose = previous_pose[robot->robot_name];
	 Pose displacement = Pose(pose.x-ppose.x,pose.y-ppose.y,pose.z-ppose.z, pose.a-ppose.a);
      
	 //printf("robot pose: %s\n", pose.String().c_str());
	 //printf("previous pose: %s\n", ppose.String().c_str());
	 /*
	 printf("callback attributes: %02f, %02f, %s, %d\n",
		  robot->floorplan_size_x,
		  robot->floorplan_size_y,
		  robot->robot_name.c_str(),
		  robot->number_particles);
     */
     // update the particle position according to the robot's movement
     for (i=0; i<robot->number_particles; i++){
		// get particle name
		sprintf(particle_name, "%s_p%03d", robot->robot_name.c_str(), i);
        p = pos->GetWorld()->GetModel(particle_name);
        if (p == 0){
      	   printf("\n\nAn exception occurred.\n particle not found.\n\n");
      	   assert( 0 );
      	   return 1;
        }
        // move the particle
	    p->AddToPose(displacement);
	    
	    // if the particle is out of foorplan bounds, then place it in a random place within the floorplan
        if ((p->GetPose().x >= (robot->floorplan_size_x/2)) || (p->GetPose().x <= -(robot->floorplan_size_x/2)) ||
			 (p->GetPose().y >= (robot->floorplan_size_y/2)) || (p->GetPose().y <= -(robot->floorplan_size_y/2)))
			p->SetGlobalPose( Pose::Random( -(robot->floorplan_size_x/2), (robot->floorplan_size_x/2), -(robot->floorplan_size_y/2), (robot->floorplan_size_y/2) ) );
	 }
	 // save the pose
     previous_pose[robot->robot_name] = pos->GetPose();
     return 0;
}

void split( const std::string& text, const std::string& separators, std::vector<std::string>& words)
{
  const int n = text.length();
  int start, stop;
  start = text.find_first_not_of(separators);
  while ((start >= 0) && (start < n))
		{
			stop = text.find_first_of(separators, start);
			if ((stop < 0) || (stop > n)) stop = n;
			words.push_back(text.substr(start, stop - start));
			start = text.find_first_not_of(separators, stop+1);
		}
}
