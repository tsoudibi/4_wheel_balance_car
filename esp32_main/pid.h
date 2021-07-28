#ifndef pid_h
#define pid_h
#include <esp32-hal-dac.h>
#include "PID_v1.h"

#define motor_l 25
#define motor_r 26
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
double kp_l=10,ki_l=0,kd_l=0;
double kp_r=10,ki_r=0,kd_r=0;
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

/* set CommandHZ */
void set_CommandHZ_PID(int hz_l,int hz_r){
  command_HZ_l = hz_l;
  command_HZ_r = hz_r;
}

/* set EncodingHZ */
void set_EncodingHZ_PID(int HZ_l,int HZ_r)
{
  Encoding_HZ_l = HZ_l;
  Encoding_HZ_r = HZ_r;
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
/*To deal with all PId function and get output signal*/
void PID(int command_hz_L,int command_hz_R,int encoder_L,int encoder_R)
{
  set_CommandHZ_PID(command_hz_L,command_hz_R);
  set_EncodingHZ_PID(encoder_L,encoder_R);
  PIDcontrol_left();
  PIDcontrol_right();
}
#endif
