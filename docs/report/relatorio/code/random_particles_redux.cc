for (i=0; i<robot->number_particles; i++){
	// Get particle name
	sprintf(particle_name, "%s_p%03d", robot->robot_name.c_str(), i);
	p = pos->GetWorld()->GetModel(particle_name);

	// Particle pose estimation (returns bar_p)
	Pose bar_p;
	Pose particle_pose = p->GetGlobalPose();
	odom_estimation(&particle_pose, &bar_p, displacement, rot);
	// Particle pose RTR variation (returns pose)
	odom_variation(&particle_pose, &bar_p, alpha);
	// Set particle pose
	p->SetGlobalPose(particle_pose);

	// Check if outside map
	if ((p->GetPose().x >= (robot->floorplan_size_x)) || (p->GetPose().x < 0) ||
		(p->GetPose().y >= (robot->floorplan_size_y)) || (p->GetPose().y <= 0))	
		p->SetGlobalPose(robot_mod->GetGlobalPose());
	else{
		// Raytracing for each sensor
		uint8_t sample_count = rob_laser->GetSensors().size();
		const double sample_incr = dtor(45.0); // 45 deg (dtor() to radians) increment 
		const double start_angle = dtor(-90.0);// -90 deg (dtor() radians) as starting angle
		particle_pose.a += start_angle;
		// Assuming all sensors have same range
		double max_range = rob_laser->GetSensors()[0].range.max;
		Ray particle_ray(rob_laser,particle_pose,max_range,ranger_match,NULL,true);

		for(int n = 0; n<sample_count; n++)
		{
			const RaytraceResult rayresult = rob_laser->GetWorld()->Raytrace(particle_ray);
			meters_t laser_particle = rayresult.range;
			double laser_robot = rob_laser->GetSensors()[n].ranges[0];
			particle_ray.origin.a += sample_incr;
			// Calculate weight
			weights[i] *= exp(-((laser_robot - laser_particle)*(laser_robot - laser_particle)) / (2*(CovSonars*CovSonars))) / sqrt(2*M_PI*(CovSonars*CovSonars));
			total_weight += weights[i];
		}			
	}

}

// Normalize weights
for (i=0; i<robot->number_particles; i++){
	weights[i] /= total_weight;
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

	//Rearrange particles
	for(i = 0; i<robot->number_particles; i++){
		sprintf(particle_name, "%s_p%03d", robot->robot_name.c_str(), i);
		pos->GetWorld()->GetModel(particle_name)->SetGlobalPose(p_vect[i]);
		weights[i] = 1.0;
	}
}