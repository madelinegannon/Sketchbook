#include "PD_Controller.h"

PD_Controller::PD_Controller()
{
	params.setName("PD_Parameters");
	params.add(kp.set("Proportional_Gains", 1, 0, 500));	// gains for Propotional Component
	params.add(kd.set("Derivative_Gains", 15, 0, 100));     // gains for Derivitive Component
	params.add(steering_scalar.set("Steering_Scalar", 1.25, 0, 5));
}

PD_Controller::~PD_Controller()
{
}

//inline void PD_Controller::setup()
//{
//	params.setName("PD_Parameters");
//	params.add(kp.set("Proportional_Gains", 1, 0, 500));	// gains for Propotional Component
//	params.add(kd.set("Derivative_Gains", 15, 0, 100));     // gains for Derivitive Component
//	params.add(steering_scalar.set("Steering_Scalar", 1.25, 0, 5));
//}

void PD_Controller::update(float setpoint)
{
	this->setpoint = setpoint;
	pd_val += (kp * (setpoint - smoothed_val) + kd * (-1 * pd_val)) * time_diff;
	smoothed_val += pd_val * steering_scalar;
}

void PD_Controller::reset(float setpoint)
{
	this->setpoint = setpoint;
	this->smoothed_val = setpoint;
	this->pd_val = 0;
}
