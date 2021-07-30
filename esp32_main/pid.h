#ifndef pid_h
#define pid_h
#include <esp32-hal-dac.h>
#include "PID_v1.h"

/* HZ from encoder */
int Encoding_HZ_l;
int Encoding_HZ_r;
/* HZ wanted */
int command_HZ_r;
int command_HZ_l;
/* output (HZ)*/
double Output_l;
double Output_r;
/* output (DAC)*/
double Output_l_fin;
double Output_r_fin;
/* PID parameter */
double kp_l=1.5,ki_l=0.4,kd_l=0.1;
double kp_r=1.7,ki_r=0.4,kd_r=0.1;
double input_l ,output_l, setpoint_l;
double input_r ,output_r, setpoint_r;

/* create PID object*/
PID mypid_left(&input_l, &output_l, &setpoint_l, kp_l, ki_l, kd_l, DIRECT); 
PID mypid_right(&input_r, &output_r, &setpoint_r, kp_r, ki_r, kd_r, DIRECT); 

/* seup */
void PID_setup()
{
   mypid_right.SetMode(AUTOMATIC);
   mypid_left.SetMode(AUTOMATIC);
}

/* caculate PID left */
void PIDcontrol_left()
{
  input_l = Encoding_HZ_l;
  Output_l = output_l;
  setpoint_l = command_HZ_l;
  /* compute */
  mypid_left.Compute();
  /* convert output from HZ to DAC*/
  Output_l_fin =  Output_l*0.7058+96.148;
}

/* caculate PID right */
void PIDcontrol_right()
{
  input_r = Encoding_HZ_r;
  Output_r = output_r;
  setpoint_r = command_HZ_r;
  /* compute */
  mypid_right.Compute();
  /* convert output from HZ to DAC*/
  Output_r_fin =  Output_r*0.7539+96.166;
}

/* for other function, get the result (DAC)*/
int get_PID_result(char which){
  if (which == 'l') return Output_l_fin;
  if (which == 'r') return Output_r_fin;
}
/* gather all data needed and do the PID */
void PID(int command_hz_L,int command_hz_R,int encoder_hz_L,int encoder_hz_R)
{
  /* set parameter needed (command HZ and encoder HZ) */ 
  command_HZ_l = command_hz_L;
  command_HZ_r = command_hz_R;
  Encoding_HZ_l = encoder_hz_L;
  Encoding_HZ_r = encoder_hz_R;
  /* do the PID */
  PIDcontrol_left();
  PIDcontrol_right();
}
#endif
