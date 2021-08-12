#ifndef pid_h
#define pid_h
#include <esp32-hal-dac.h>
#include "PID_v1.h"

/* HZ from encoder */
int Encoding_HZ_l;
int Encoding_HZ_r;
/* HZ wanted */
int command_HZ_r = 0;
int command_HZ_l = 0;
/* output (HZ)*/
double Output_l;
double Output_r;
/* output (DAC)*/
double Output_l_fin;
double Output_r_fin;
/* PID parameter */
/*double kp_l,ki_l,kd_l;
double kp_r,ki_r,kd_r;*/
/* for button */
double kp_l=0.6,ki_l=0.8,kd_l=0;
double kp_r=0.6,ki_r=0.5,kd_r=0;
/*for sensor*/
/*double kp_l=3,ki_l=1.3,kd_l=0;
double kp_r=3,ki_r=1.2,kd_r=0;*/

double input_l ,output_l, setpoint_l;
double input_r ,output_r, setpoint_r;

/*void get_control_mode(int control_mode_PID)
{
  int control_Mode = 0;
  control_Mode = control_mode_PID;
  if(control_Mode == 1 || control_Mode == 2)
  {
    kp_l=3;ki_l=1.3;kd_l=0;
    kp_r=3;ki_r=1.2;kd_r=0;
    Serial.println("mode"+String(control_Mode));
  }
  else if(control_Mode == 3)
  {
    kp_l=1.6;ki_l=0.6;kd_l=0;
    kp_r=1.7;ki_r=0.8;kd_r=0;
    Serial.println("mode"+String(control_Mode));
  }
}*/

/* create PID object*/
PID mypid_left(&input_l, &output_l, &setpoint_l, kp_l, ki_l, kd_l, DIRECT); 
PID mypid_right(&input_r, &output_r, &setpoint_r, kp_r, ki_r, kd_r, DIRECT); 

/* set up */
void PID_setup()
{
   mypid_right.SetMode(AUTOMATIC);
   mypid_left.SetMode(AUTOMATIC);
}
/*to get mode to change parameter*/


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
  /* let PID make change smoothy */
  /* left */
  command_HZ_l = command_hz_L;
  Encoding_HZ_l = encoder_hz_L;
  PIDcontrol_left();
  /*right*/
  command_HZ_r = command_hz_R;
  Encoding_HZ_r = encoder_hz_R;
  PIDcontrol_right();
  //Serial.println(String(command_hz_L)+","+String(command_hz_R)+","+String(command_HZ_l)+","+String(command_HZ_r));
}
#endif
