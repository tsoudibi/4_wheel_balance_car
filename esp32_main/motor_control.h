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
  /*Serial.print(speed_l);
  Serial.print(",");
  Serial.println(speed_r);*/
  Serial.println("[motr] :"+String(signal_L)+","+String(signal_R));
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


#endif /* MOTOR_CONTROL_H */
