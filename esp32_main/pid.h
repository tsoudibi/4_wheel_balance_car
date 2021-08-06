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
double kp_l=1.5,ki_l=0.8,kd_l=0.1;
double kp_r=1.5,ki_r=0.55,kd_r=0.1;
double input_l ,output_l, setpoint_l;
double input_r ,output_r, setpoint_r;
/**/
int commanderror = 0;

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
  /* let PID make change smoothy */
  /* left */
  if(command_HZ_l != command_hz_L)
  {
    commanderror = (abs(command_HZ_l - command_hz_L))/2;
    /*if commanderror is too small that is near equal to 0*/
    if(commanderror < 0.02){
      command_HZ_l = command_hz_L;
      commanderror = 0;}
    else{
      command_HZ_l = command_HZ_l + commanderror;}
  }
  PIDcontrol_left();
  /*right*/
  if(command_HZ_r != command_hz_R)
  {
    commanderror = abs((command_HZ_r - command_hz_R))/2;
    Serial.println("123");
    Serial.println(String(command_hz_L)+","+String(command_hz_R)+","+String(command_HZ_l)+","+String(command_HZ_r)+","+String(commanderror));
    /*if commanderror is too small that is near equal to 0*/
    if(commanderror < 0.02){
      command_HZ_r = command_hz_R;
      commanderror = 0;}
    else{
      command_HZ_r = command_HZ_r - commanderror;
      }
  }
  PIDcontrol_right();
  //Serial.println(String(command_hz_L)+","+String(command_hz_R)+","+String(command_HZ_l)+","+String(command_HZ_r));
}
#endif
