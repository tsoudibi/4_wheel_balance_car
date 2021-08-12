#ifndef pid_h
#define pid_h
#include <esp32-hal-dac.h>
#include "PID_v1.h"

#define motor_l 25
#define motor_r 26
int Encoding_HZ_l;
int Encoding_HZ_r;

int command_HZ_r;
int command_HZ_l;

double Output_l;
double Output_r;

double Output_l_fin;
double Output_r_fin;

double kp_l=1.2,ki_l=2,kd_l=0.3;
double kp_r=1.2,ki_r=0.9,kd_r=0.3;

double input ,output, setpoint;
PID mypid_left(&input, &output, &setpoint, kp_l, ki_l, kd_l, DIRECT); 
PID mypid_right(&input, &output, &setpoint, kp_r, ki_r, kd_r, DIRECT); 
/*void speedtoHZ(int speed_l,int speed_r)
{
  //將速度訊息改為頻率
  command_HZ_l = speed_l*1.4114-135.62;
  command_HZ_r = speed_r*1.3257-127.48;
}*/

void CommandHZ(int hz_l,int hz_r)
{
  command_HZ_l = hz_l;
  command_HZ_r = hz_r;
}

void SetPID()
{
   mypid_right.SetMode(AUTOMATIC);
   mypid_left.SetMode(AUTOMATIC);
}
void EncodingHZ(int HZ_l,int HZ_r)
{
  Encoding_HZ_l = HZ_l;
  Encoding_HZ_r = HZ_r;
}

void pidcontrol_left()
{
  input = Encoding_HZ_l;
  Output_l = output;
  setpoint = command_HZ_l;
  /*Serial.print(Output_l);
  Serial.print(",");*/
  mypid_left.Compute();
}


void pidcontrol_right()
{
  input = Encoding_HZ_r;
  Output_r = output;
  /*Serial.print(Output_r);
  Serial.print(",");*/
  setpoint = command_HZ_r;

  mypid_right.Compute();
}
void OutputSignal()
{
  Output_r_fin =  Output_r*0.7058+96.148;
  Output_l_fin =  Output_l*0.7539+96.166;
  /*Serial.print(Output_l_fin);
  Serial.print(",");
  Serial.println(Output_r_fin);*/
  dacWrite(motor_l,Output_l_fin);
  dacWrite(motor_r,Output_r_fin);
}
#endif
