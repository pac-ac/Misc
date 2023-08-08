const double KINEMATICS_GRAVITY_EARTH = -9.8;


double KINEMATICS_VELOCITY_UAT(double init_velocity, 
		double acceleration, 
		double time) { 

	return init_velocity + acceleration*time;
}



double KINEMATICS_DISPLACEMENT_UVT(double init_velocity,
		double velocity,
		double time) {

	return ((init_velocity+velocity)/2.0) * time;
}



double KINEMATICS_DISPLACEMENT_UAT(double init_velocity,
		double acceleration,
		double time) {

	return init_velocity*time + 0.5*acceleration*(time*time);
}



double KINEMATICS_VELOCITY_SQUARED_UAD(double init_velocity,
		double acceleration,
		double displacement) {

	return (init_velocity*init_velocity) + 2.0*acceleration*displacement;
}
