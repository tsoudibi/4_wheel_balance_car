#ifndef pid_h
#define pid_h
#include <esp32-hal-dac.h>
#include "PID_v1.h"
#include "arduino_connect.h"

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
/*PID parameter set*/
/*for load*/
double kp_l_load = 6,ki_l_load = 1.2,kd_l_load = 0;
double kp_r_load = 6,ki_r_load = 1.3,kd_r_load = 0;
/*f or not load*/
/*first one*/
/*double kp_l_Nload = 0.437,ki_l_Nload = 0.27,kd_l_Nload = 0;
double kp_r_Nload = 0.405,ki_r_Nload = 0.23,kd_r_Nload = 0;*/
/*next one for camara(震盪嚴重，追很快)*/
/*double kp_l_Nload = 1.4,ki_l_Nload = 0.9,kd_l_Nload = 0;
double kp_r_Nload = 1.3,ki_r_Nload = 0.85,kd_r_Nload = 0;*/
/*third one*/
double kp_l_Nload = 1.1,ki_l_Nload = 0.9,kd_l_Nload = 0;
double kp_r_Nload = 0.93,ki_r_Nload = 0.85,kd_r_Nload = 0;
/*this pid data will use when the car start to move*/
double kp_l_start = 1.1,ki_l_start = 0.9,kd_l_start = 0;
double kp_r_start = 0.93,ki_r_start = 0.85,kd_r_start = 0;
/*others*/
double input_l ,output_l, setpoint_l;
double input_r ,output_r, setpoint_r;

/* create PID object*/
/* has load */
PID mypid_left_load(&input_l, &output_l, &setpoint_l, kp_l_load, ki_l_load, kd_l_load, DIRECT); 
PID mypid_right_load(&input_r, &output_r, &setpoint_r, kp_r_load, ki_r_load, kd_r_load, DIRECT); 
/* not has load*/
PID mypid_left_Nload(&input_l, &output_l, &setpoint_l, kp_l_Nload, ki_l_Nload, kd_l_Nload, DIRECT); 
PID mypid_right_Nload(&input_r, &output_r, &setpoint_r, kp_r_Nload, ki_r_Nload, kd_r_Nload, DIRECT);
/* car start to move*/
PID mypid_left_start(&input_l, &output_l, &setpoint_l, kp_l_start, ki_l_start, kd_l_start, DIRECT); 
PID mypid_right_start(&input_r, &output_r, &setpoint_r, kp_r_start, ki_r_start, kd_r_start, DIRECT);

/*PID set up */
void PID_setup()
{
   /*load*/
   mypid_right_load.SetMode(AUTOMATIC);
   mypid_left_load.SetMode(AUTOMATIC);

   /*no load*/
   mypid_right_Nload.SetMode(AUTOMATIC);
   mypid_left_Nload.SetMode(AUTOMATIC);

   /*start to nove*/
   mypid_left_start.SetMode(AUTOMATIC);
   mypid_right_start.SetMode(AUTOMATIC);
}

/* caculate PID left */
void PIDcontrol_left(int flag_left)
{
  input_l = Encoding_HZ_l;
  Output_l = output_l;
  setpoint_l = command_HZ_l;
  /* compute */
  if(input_l != 0)
  {
    if(flag_left == 0){         //no load
      mypid_left_Nload.Compute();
    }
    else if(flag_left == 1){    //has load
      mypid_left_load.Compute();
    }
  }
  /*car start to move*/
  if(input_l == 0)
  {
    mypid_left_start.Compute();
  }
  /* convert output from HZ to DAC*/
  //Output_l_fin =  Output_l*0.7058+96.148;
  Output_l_fin =  Output_l*0.70581637+96.0889897;
}
/* caculate PID right */
void PIDcontrol_right(int flag_right)
{
  input_r = Encoding_HZ_r;
  Output_r = output_r;
  setpoint_r = command_HZ_r;
  /* compute */
  if(input_r != 0)
  {
    if(flag_right == 0){        //no load
      mypid_right_Nload.Compute();
    }
    else if(flag_right == 1){   //has load
      mypid_right_load.Compute();
    }
  }
  /*car start to move*/
  if(input_l == 0)
  {
    mypid_right_start.Compute();
  }  
  /* convert output from HZ to DAC*/
  //Output_r_fin =  Output_r*0.7539+96.166;
  Output_r_fin =  Output_r*0.73926222+94.241;
}
/*
 * HZ = 1.4114DAC-135.62 left
 * HZ = 1.3527DAC-127.48 right
 */

/* for other function, get the result (DAC)*/
int get_PID_result(char which){
  if (which == 'l') return Output_l_fin;
  if (which == 'r') return Output_r_fin;
}
/* gather all data needed and do the PID */
void PID(int command_hz_L,int command_hz_R,int encoder_hz_L,int encoder_hz_R,int flag)
{
  /* set parameter needed (command HZ and encoder HZ) */
  /* left */
  command_HZ_l = command_hz_L;
  Encoding_HZ_l = encoder_hz_L;
  PIDcontrol_left(flag);
  /*right*/
  command_HZ_r = command_hz_R;
  Encoding_HZ_r = encoder_hz_R;
  PIDcontrol_right(flag);
  Serial.println(String(Output_l_fin)+","+String(Output_r_fin)+","+String(command_HZ_l)+","+String(command_HZ_r));
}
#endif
