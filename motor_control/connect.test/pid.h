#ifndef pid_h
#define pid_h
#include <esp32-hal-dac.h>
#include "PID_v1.h"

#define motor_l 25
#define motor_r 26

////set PID////
int Encoding_HZ_l;
int Encoding_HZ_r;

int command_HZ_r;
int command_HZ_l;

int Output_l;
int Output_r;

int Output_l_fin;
int Output_r_fin;

double kp=1.5,ki=0.5,kd=0;  
double input ,output, setpoint;

PID mypid(&input, &output, &setpoint, kp, ki, kd, DIRECT); 


void speedtoHZ(int speed_l,int speed_r)
{
  //將速度訊息改為頻率
  command_HZ_l = speed_l*1.4114-135.62;
  command_HZ_r = speed_r*1.3257-127.48;
}
void SetPID()
{
   mypid.SetMode(AUTOMATIC);
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
  mypid.Compute();
}


void pidcontrol_right()
{
  input = Encoding_HZ_r;
  Output_r = output;
  setpoint = command_HZ_r;

  mypid.Compute();
}
void OutputSignal()
{
  Output_r_fin =  Output_r*0.7058+96.148;
  Output_l_fin =  Output_l*0.7539+96.166;
  Serial.print(Output_l_fin);
  Serial.print(",");
  Serial.println(Output_r_fin);
  dacWrite(motor_l,Output_l_fin);
  dacWrite(motor_r,Output_r_fin);
}
#endif
