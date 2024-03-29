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
/*control mode*/
int Control_mode;
/*PID parameter set*/
/*for load*/
double kp_l_load = 3,ki_l_load = 1.4,kd_l_load = 0;
double kp_r_load = 2.5,ki_r_load = 1.2,kd_r_load = 0;
/*f or not load*/
/*first one(btn)*/
double kp_l_Nload_btn = 1.3,ki_l_Nload_btn = 0.9,kd_l_Nload_btn = 0;
double kp_r_Nload_btn = 1.43,ki_r_Nload_btn = 0.85,kd_r_Nload_btn = 0;
/*next one for camara(震盪嚴重，追很快)*/
/*double kp_l_Nload = 1.4,ki_l_Nload = 0.9,kd_l_Nload = 0;
double kp_r_Nload = 1.3,ki_r_Nload = 0.85,kd_r_Nload = 0;*/
/*third one(camara)*/
double kp_l_Nload_cam = 1.4,ki_l_Nload_cam = 0.9,kd_l_Nload_cam = 0;
double kp_r_Nload_cam = 1.33,ki_r_Nload_cam = 0.85,kd_r_Nload_cam = 0;
/*has load(right)*/
double kp_l_load_Tright = 6, ki_l_load_Tright = 1.8, kd_l_load_Tright = 0;
double kp_r_load_Tright = 7, ki_r_load_Tright = 2, kd_r_load_Tright = 0;
/*has load(left)*/
double kp_l_load_Tleft = 7, ki_l_load_Tleft = 2, kd_l_load_Tleft = 0;
double kp_r_load_Tleft = 6, ki_r_load_Tleft = 1.8, kd_r_load_Tleft = 0;
/*others*/
double input_l ,output_l, setpoint_l;
double input_r ,output_r, setpoint_r;

void Get_Control_Mode(int Mode)
{
  Control_mode = Mode;
}

/* create PID object*/
/* has load */
PID mypid_left_load(&input_l, &output_l, &setpoint_l, kp_l_load, ki_l_load, kd_l_load, DIRECT); 
PID mypid_right_load(&input_r, &output_r, &setpoint_r, kp_r_load, ki_r_load, kd_r_load, DIRECT);
/* has load(turn right)*/
PID mypid_left_load_Tright(&input_l, &output_l, &setpoint_l, kp_l_load_Tright, ki_l_load_Tright, kd_l_load_Tright, DIRECT); 
PID mypid_right_load_Tright(&input_r, &output_r, &setpoint_r, kp_r_load_Tright, ki_r_load_Tright, kd_r_load_Tright, DIRECT);
/* has load(turn left)*/
PID mypid_left_load_Tleft(&input_l, &output_l, &setpoint_l, kp_l_load_Tleft, ki_l_load_Tleft, kd_l_load_Tleft, DIRECT); 
PID mypid_right_load_Tleft(&input_r, &output_r, &setpoint_r, kp_r_load_Tleft, ki_r_load_Tleft, kd_r_load_Tleft, DIRECT); 
/* not has load(btn)*/
PID mypid_left_Nload_btn(&input_l, &output_l, &setpoint_l, kp_l_Nload_btn, ki_l_Nload_btn, kd_l_Nload_btn, DIRECT); 
PID mypid_right_Nload_btn(&input_r, &output_r, &setpoint_r, kp_r_Nload_btn, ki_r_Nload_btn, kd_r_Nload_btn, DIRECT);
/* not has load(cam)*/
PID mypid_left_Nload_cam(&input_l, &output_l, &setpoint_l, kp_l_Nload_cam, ki_l_Nload_cam, kd_l_Nload_cam, DIRECT); 
PID mypid_right_Nload_cam(&input_r, &output_r, &setpoint_r, kp_r_Nload_cam, ki_r_Nload_cam, kd_r_Nload_cam, DIRECT);
/*PID set up */
void PID_setup()
{
   /*load*/
   mypid_right_load.SetMode(AUTOMATIC);
   mypid_left_load.SetMode(AUTOMATIC);

   /*no load(btn)*/
   mypid_right_Nload_btn.SetMode(AUTOMATIC);
   mypid_left_Nload_btn.SetMode(AUTOMATIC);

   /*not has load(cam)*/
   mypid_left_Nload_cam.SetMode(AUTOMATIC);
   mypid_right_Nload_cam.SetMode(AUTOMATIC);

   /* has load(turn righr)*/
   mypid_left_load_Tright.SetMode(AUTOMATIC);
   mypid_right_load_Tright.SetMode(AUTOMATIC);

   /* has load(turn left)*/
   mypid_left_load_Tleft.SetMode(AUTOMATIC);
   mypid_right_load_Tleft.SetMode(AUTOMATIC);
}

/* caculate PID left */
void PIDcontrol_left(int flag_left)
{
  input_l = Encoding_HZ_l;
  Output_l = output_l;
  setpoint_l = command_HZ_l;
  /* compute */
  if(flag_left == 0){         //no load
      if(Control_mode = 1)//btn mode
        mypid_left_Nload_btn.Compute();
      if(Control_mode = 3)//cam mode
        mypid_left_Nload_cam.Compute();  
    }
  else if(flag_left == 1){    //has load
      /*if(command_HZ_r > command_HZ_r) //turn left
        mypid_left_load_Tleft.Compute();
      if(command_HZ_r < command_HZ_r) //turn right
        mypid_left_load_Tright.Compute(); 
      else*/ 
        mypid_left_load.Compute();//go straight
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
  if(flag_right == 0){         //no load
      if(Control_mode = 1)//btn mode
        mypid_right_Nload_btn.Compute();
      if(Control_mode = 3)//cam mode
        mypid_right_Nload_cam.Compute();  
    }
  else if(flag_right == 1){   //has load
      /*if(command_HZ_r > command_HZ_r) //turn left
        mypid_right_load_Tleft.Compute();
      if(command_HZ_r < command_HZ_r)
        mypid_right_load_Tright.Compute(); //turn right
      else*/
        mypid_right_load.Compute();//go straight
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
