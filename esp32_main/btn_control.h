#ifndef BTN_CONTROL_H
#define BTN_CONTROL_H

#include "esp32_http.h"
#include "motor_control.h"
#include "arduino_connect.h"
#include "pid.h"

#define threshold 20

/* save the HZ of motor should be */
int HZ_L;
int HZ_R;

void btn_PID(int L, int R);

void btn_control(){
  if (http_GET("new")=="1"){
    /* get movement from sever */
    String mov=http_GET("movement");
    /* get current HZ from motor_control.h */
    HZ_L = get_control_signal('l');
    HZ_R = get_control_signal('r');
    /* with different movement, make different adjust*/
      if (mov== "forward"){
          btn_PID(HZ_L+2,HZ_R+2);
          Serial.print("[move]forward");}else
      if (mov== "left"){
          btn_PID(HZ_L-1,HZ_R+1);
          Serial.print("[move]left");}else
      if (mov=="right"){
          btn_PID(HZ_L+1,HZ_R-1);
          Serial.print("[move]right");}else
      if (mov== "stop"){
          btn_PID(0,0);
          Serial.print("[move]stop");
      }else{
        Serial.println("[ERRO] wrong movent command");
      }
  }
}


/* consider pid control */
void btn_PID(int L, int R){
  /* update data from encoder and sensor */
  update_data_all();
  /* encoder HZ from arduino*/
  int ISR_HZ_L = get_serial_data(1);
  int ISR_HZ_R = get_serial_data(2);
  /* motor control command from button */
  int command_L = L;
  int command_R = R;
  /* mass center at map from arduino */
  int map_x = get_serial_data(3);
  int map_y = get_serial_data(4);
  /* consider threshold, adjust command HZ*/
  if (command_L >= threshold) command_L = threshold;
  if (command_R >= threshold) command_R = threshold;
  if (command_L <= 0) command_L = 0;
  if (command_R <= 0) command_R = 0;
  /* do PID */
  set_EncodingHZ_PID(ISR_HZ_L,ISR_HZ_R);
  set_CommandHZ_PID(command_L,command_R);
  PIDcontrol_left();
  PIDcontrol_right();
  /* after PID */
  int HZ_L = get_PID_result('l');
  int HZ_R = get_PID_result('r');
  /* RUN the motor */
  motor_control(HZ_L,HZ_R);
  /* print data and send to server */
  time_now = millis();
  Serial.println( "[HTTP] post:"+http_POST(HZ_L,HZ_R,ISR_HZ_L,ISR_HZ_R,map_x,map_y)+" ,used time:"+(millis()-time_now));
}

#endif /* BTN_CONTROL_H */
