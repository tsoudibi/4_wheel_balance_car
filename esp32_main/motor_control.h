/*
 * ALERT!!
 * this code is for temperatly usage
 * the rearl motor_control.h must use class for better space usage
 */
#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <esp32-hal-dac.h>//for contronl
/*control setting*/
#define motor_l 25
#define motor_r 26
#define rev_pin 35

int signal_L = 50;
int signal_R = 50;

void motor_setup(){
  pinMode(motor_l,OUTPUT);
  pinMode(motor_r,OUTPUT);
  pinMode(rev_pin,OUTPUT);
}

void motor_control(){
  int speed_l;
  int speed_r;
  speed_l = 1.4*signal_L ;
  speed_r = 1.4*signal_R ;
  
  if(speed_l > 0 && speed_r > 0)
  {
    dacWrite(motor_l,speed_l);
    dacWrite(motor_r,speed_r);
  }
  else{
    dacWrite(motor_l,0);
    dacWrite(motor_r,0);
  }
  /*Serial.print(speed_l);
  Serial.print(",");
  Serial.println(speed_r);*/
  Serial.println("[sped] :"+String(speed_l)+","+String(speed_r));
  //time_now = millis();
  //Serial.println( "[HTTP] post:"+http_POST(speed_l,speed_r)+" ,used time:"+(millis()-time_now));
}

int get_control_signal(char which){
  if (which=='r'){
    return signal_R;
  }
  if (which=='l'){
    return signal_L;
  }
}

void set_control_signal(int L,int R){
  signal_R=R;
  signal_L=L;
}

#endif /* MOTOR_CONTROL_H */
