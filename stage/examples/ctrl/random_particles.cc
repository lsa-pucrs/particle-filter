/*
 This Stage control automatically generates a number of particle models to each robot.
 For animation purposes, each particle can be moved with GetGlobalPose() method
 Author: Alexandre Amory (amamory @ gmail com)
*/
#include "stage.hh"
#include <algorithm>

using namespace Stg;
using namespace std;

// Filter parameters
static double CovSonars = 2.5;
static double alpha[] = {1,25,25,0.01};

//Globalzão
int k = 0;

// weights vector
vector<double> weights;

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
static bool ranger_match( Model* hit, Model* finder, const void* dummy );
void odom_estimation(Pose* pose, Pose* bar_p, double displ, double rot);
void odom_variation(Pose* pose, Pose* bar_p, double* alpha);
double r2();
double random_normal();

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
  Pose robot_pos = w->GetModel(robot_name)->GetGlobalPose();
  
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
  
  printf( "\n\nCreating %d particles for robot %s\n", particles, robot_name.c_str() );
  // create the particles at robot starting position
  for(i=0; i<particles; i++ )
  {
		sprintf(particle_name, "%s_p%03d", robot_name.c_str(), i);
		Model* new_model = new Model(w,NULL,"model",particle_name);
		//Model* new_model = w->GetModel(particle_name);
		// the particles will have the color of the dummy model
		new_model->SetColor(mod->GetColor());
		// set the particle size to the dummy model size
		//new_model->SetGeom( Geom( Pose(), Stg::Size( 0.1, 0.1, 0.01 ) ));
		new_model->SetGeom(mod->GetGeom());
		// each particle have a random initial position within the floorplan area
		//new_model->SetGlobalPose( Pose::Random( -(floorplan_size_x/2), (floorplan_size_x/2), -(floorplan_size_y/2), (floorplan_size_y/2) ) );
		new_model->SetGlobalPose(robot_pos);
		printf("particle: %s; pose: %s\n", particle_name, new_model->GetGlobalPose().String().c_str());
		// set the particle to be transparent in the simulation
        new_model->SetBlobReturn (false);
        new_model->SetBoundary (false);
        new_model->SetFiducialReturn (0);
        new_model->SetGripperReturn (false);
        new_model->SetGuiMove (true);
        new_model->SetGuiNose (true);
        new_model->SetObstacleReturn (false);
        new_model->SetRangerReturn (-1.0);
        
        weights.push_back(1.0);
	 }
	 
	 srand(time(NULL));
	 
	 //Set robot's previous pose as initial pose
	 previous_pose[info->robot_name] = robot_pos;
	 
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
	 World* wo = pos->GetWorld();
	 Pose pose = pos->GetGlobalPose();
	 
	 char particle_name[50];
	 double total_weight = 0;
	 int i;
	 Model *p; // particle pointer
	 Model* robot_mod = wo->GetModel(robot->robot_name);
	 ModelRanger* rob_laser = (ModelRanger*)robot_mod->GetChild("ranger:0");
	
	 //static Pose previous_pose;
	 //static std::map< string, Pose> previous_pose;
	 // each robot has its own previous pose
	 Pose ppose = previous_pose[robot->robot_name];
	 double displacement = sqrt((pose.x-ppose.x)*(pose.x-ppose.x) + (pose.y-ppose.y)*(pose.y-ppose.y));
     double rot = pose.a - ppose.a;
     
     cout << "pose: " << pose.String() << endl;
	 cout << "ppose: " << ppose.String() << endl;
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
        
        // Particle pose estimation (returns bar_p)
        Pose bar_p;
        Pose particle_pose = p->GetGlobalPose();
		odom_estimation(&particle_pose, &bar_p, displacement, rot);
        
        // Particle pose RTR variation (returns pose)
        odom_variation(&particle_pose, &bar_p, alpha);
        
        // Set particle pose
        p->SetGlobalPose(particle_pose);
        
        cout << "\nFuture_pose: " << bar_p.String() << endl;
        cout << "Particle_pose: " << pose.String() << endl;
        cout << "Robot pose: " << robot_mod->GetGlobalPose().String() << endl;       
        cout << "Displacement: " << displacement << endl;
        
        // move the particle
	    //p->AddToPose(displacement); // displacement = barp
	    	    
	    // if the particle is out of floorplan bounds, then place it in a random place within the floorplan
        //if ((p->GetPose().x >= (robot->floorplan_size_x/2)) || (p->GetPose().x <= -(robot->floorplan_size_x/2)) ||
		//	 (p->GetPose().y >= (robot->floorplan_size_y/2)) || (p->GetPose().y <= -(robot->floorplan_size_y/2)))
			//p->SetGlobalPose( Pose::Random( -(robot->floorplan_size_x/2), (robot->floorplan_size_x/2), -(robot->floorplan_size_y/2), (robot->floorplan_size_y/2) ) );
		if ((p->GetPose().x >= (robot->floorplan_size_x)) || (p->GetPose().x < 0) ||
			 (p->GetPose().y >= (robot->floorplan_size_y)) || (p->GetPose().y <= 0))	
			p->SetGlobalPose(robot_mod->GetGlobalPose());
		else{
			//TODO: Raycasting das particulas
			// for each sensor	
			
			uint8_t sample_count = rob_laser->GetSensors().size();
			const double sample_incr = dtor(45.0); // 45 deg (dtor() to radians) increment 
			const double start_angle = dtor(-90.0);// -90 deg (dtor() radians) as starting angle
					
			//Pose particle_pose(p->GetGlobalPose());
			particle_pose.a += start_angle;  
			//particle_pose = rob_laser->LocalToGlobal(particle_pose);
			
			// assuming all sensors have same range
			double max_range = rob_laser->GetSensors()[0].range.max;
			Ray particle_ray(rob_laser,particle_pose,max_range,ranger_match,NULL,true);
			
			for(int n = 0; n<sample_count; n++)
			{
				const RaytraceResult rayresult = rob_laser->GetWorld()->Raytrace(particle_ray);
				meters_t laser_particle = rayresult.range;
				
				double laser_robot = rob_laser->GetSensors()[n].ranges[0];
				//cout << "Laser particle[" << n << "]: " << laser_particle;
				//cout << "\t\tLaser robot[" << n << "]: " << laser_robot << endl;
				
				// point the ray to the next angle:
				particle_ray.origin.a += sample_incr;
				
				// Calculate weight
				weights[i] *= exp(-((laser_robot - laser_particle)*(laser_robot - laser_particle)) / (2*(CovSonars*CovSonars))) / sqrt(2*M_PI*(CovSonars*CovSonars));
				total_weight += weights[i];
			}			
		}
		
	 }
	 
	 //cout << "\nTotal Weight: " << total_weight << endl;
	 
	 for (i=0; i<robot->number_particles; i++){
		 weights[i] /= total_weight;
		 //cout << "Weight of particle " << i << ": " << weights[i] << endl;
	 }
	 
	 //Resampling
	 if((k % 15) == 0){
		 vector<Pose> p_vect;
		 p_vect.resize(robot->number_particles);
		 double mw, beta = 0;
		 int index, i;
		 char particle_name[50];
		 
		 vector<double>::iterator point;
		 point = max_element(weights.begin(), weights.end());
		 mw = *point;
		 index = rand() % robot->number_particles; //Random from 0 to number_particles - 1
		 
		 for(i = 0; i<robot->number_particles; i++){
			 beta += r2()*mw*2;
			 
			 while(beta > weights[index]){
				 beta -= weights[index];
				 index++;
				 if(index > robot->number_particles-1){
					 index = 0;
				 }
			 }
			 
			 sprintf(particle_name, "%s_p%03d", robot->robot_name.c_str(), index);
			 Pose part_pose = pos->GetWorld()->GetModel(particle_name)->GetGlobalPose();
			 p_vect[i] = part_pose;
		 }
		 
		 for(i = 0; i<robot->number_particles; i++){
			 sprintf(particle_name, "%s_p%03d", robot->robot_name.c_str(), i);
			 pos->GetWorld()->GetModel(particle_name)->SetGlobalPose(p_vect[i]);
			 weights[i] = 1.0;
		 }
	 }
	 
	 // save the pose
     previous_pose[robot->robot_name] = pos->GetGlobalPose();
     
     k++;
     
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

static bool ranger_match( Model* hit, 
			  Model* finder,
			  const void* dummy )
{
  (void)dummy; // avoid warning about unused var
  
  // Ignore the model that's looking and things that are invisible to
  // rangers 
  
  // small optimization to avoid recursive Model::IsRelated call in common cases
  if( (hit == finder->Parent()) || (hit == finder) ) return false;
  
  return( (!hit->IsRelated( finder )) && (sgn(hit->vis.ranger_return) != -1 ) );
}

void odom_estimation(Pose* pose, Pose* bar_p, double displ, double rot){
	bar_p->a = pose->a + rot;
	bar_p->x = pose->x + displ*cos((pose->a + bar_p->a)/2);
	bar_p->y = pose->y + displ*sin((pose->a + bar_p->a)/2);
}

void odom_variation(Pose* p, Pose* bar_p, double* a){
	double rot_1 = atan2((bar_p->y - p->y),(bar_p->x - p->x))-p->a;
	double trans = sqrt((bar_p->y - p->y)*(bar_p->y - p->y) + (bar_p->x - p->x)*(bar_p->x - p->x));
	double rot_2 = bar_p->a - p->a - rot_1;
	
	double drot_1 = rot_1 - (a[0]*(rot_1*rot_1) + a[1]*(trans*trans))*random_normal();
	double dtrans = trans - (a[2]*(trans*trans) + a[3]*((rot_1*rot_1)+(rot_2*rot_2)))*random_normal();
	double drot_2 = rot_2 - (a[0]*(rot_2*rot_2) + a[1]*(trans*trans))*random_normal();
	
	p->x += dtrans*cos(p->a+drot_1);
	p->y += dtrans*sin(p->a+drot_1);
	p->a += drot_1 + drot_2;
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
