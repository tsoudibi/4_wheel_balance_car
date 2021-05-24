/*double Kp=;
double Ki=;
double Kd=0;*/
#ifndef "pid.h"
#define "pid.h"
unsigned long current_time,previous_time;
double time_difference;
double error;
double last_error;
double accumulated_error=0,rate_of_error;


double PIDcontrol(double order,double in,double kp,double ki,double kd)
{ 
  
  current_time=millis();
  time_difference=(double)(current_time - previous_time);
  error=order-in;
  accumulated_error+=error*time_difference;
  rate_of_error=(error-last_error)/time_difference;
  double out=Kp*error+Ki*accumulated_error+Kd*rate_of_error;

  last_error=error;
  previous_time=current_time;

  return out;
  
}
#endif
