#ifndef BTN_CONTROL_H
#define BTN_CONTROL_H

#include "esp32_http.h"
#include "motor_control.h"
#include "arduino_connect.h"
#include "pid.h"

#define threshold 20

/* save the HZ of motor should be */
/*The smallest speed is 7*/
int HZ_L = 0;
int HZ_R = 0;

void btn_PID(int L, int R);

void btn_control(int HZ_L,int HZ_R){
  /* update data from encoder and sensor */
  update_data_all();
  
  if (http_GET("new")=="1"){
    /* get movement from sever */
    String mov=http_GET("movement");

    
    /* with different movement, make different adjust, change the command*/
      if (mov== "forward"){
          HZ_L = HZ_L + 2;
          HZ_R = HZ_R + 2;
          Serial.println("[move]forward");}else
      if (mov== "left"){
          HZ_L = HZ_L - 1;
          HZ_R = HZ_R + 1;          
          Serial.println("[move]left");}else
      if (mov=="right"){
          HZ_L = HZ_L + 1;
          HZ_R = HZ_R - 1;
          Serial.println("[move]right");}else
      if (mov== "stop"){
          HZ_L = 0;
          HZ_R = 0;
          Serial.println("[move]stop");
      }else{
        Serial.println("[ERRO] wrong movent command");
      }
        /* consider threshold, adjust command HZ*/
  if (HZ_L >= threshold) HZ_L = threshold;
  if (HZ_R >= threshold) HZ_R = threshold;
  if (HZ_L <= 0) HZ_L = 0;
  if (HZ_R <= 0) HZ_R = 0;
  }

  /* print data and send to server */
  /*time_now = millis();
  Serial.println( "[HTTP] post:"+http_POST(HZ_L,HZ_R,ISR_HZ_L,ISR_HZ_R,map_x,map_y)+" ,used time:"+(millis()-time_now));*/
  
  /*PID control*/
  btn_PID(HZ_L,HZ_R);
}


/* consider pid control */
void btn_PID(int L, int R){
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

 
//y = 1.4114x-135.62 left
//y = 1.3527x-127.48 right
  
  /* do PID */
  set_EncodingHZ_PID(ISR_HZ_L,ISR_HZ_R);
  set_CommandHZ_PID(command_L,command_R);
  PIDcontrol_left();
  PIDcontrol_right();
  /* after PID */
  int DAC_L = get_PID_result('l');
  int DAC_R = get_PID_result('r');
  /* RUN the motor */
  motor_control(DAC_L,DAC_R);
  /*  save it */
  HZ_L = L;
  HZ_R = R;


}

int return_command_hz_btn(char which)
{
  if (which == 'l')
    return HZ_L;
  if (which == 'r')
    return HZ_R;
}

#endif /* BTN_CONTROL_H */
