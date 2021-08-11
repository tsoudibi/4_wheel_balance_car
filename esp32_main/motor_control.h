/*
 * ALERT!!
 * this code is for temperatly usage
 * the rearl motor_control.h must use class for better space usage
 */
#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <esp32-hal-dac.h>//for contronl
/*pid*/
#include "pid.h"
/*control setting*/
#define motor_l 26
#define motor_r 25
#define rev_pin 35

/* real HZ command to motor*/
int signal_L = 1;
int signal_R = 1;


void motor_setup(){
  pinMode(motor_l,OUTPUT);
  pinMode(motor_r,OUTPUT);
  pinMode(rev_pin,OUTPUT);
}

void motor_control(int L, int R){
  signal_L = L;
  signal_R = R;
  if(L > 0 && R > 0){
    dacWrite(motor_l,L);
    dacWrite(motor_r,R);
  }
  else{
    dacWrite(motor_l,0);
    dacWrite(motor_r,0);
  }
}

/*
 * HZ = 1.4114DAC-135.62 left
 * HZ = 1.3527DAC-127.48 right
 */

int get_control_signal(char which){
  if (which=='r'){
    return signal_R;
  }
  if (which=='l'){
    return signal_L;
  }
}



#endif /* MOTOR_CONTROL_H */
