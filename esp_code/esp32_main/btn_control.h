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

void btn_control(int HZ_L_btn,int HZ_R_btn){
  /* update data from encoder and sensor */
  update_data_all();
  
  /* get movement from sever */
  String mov = server_gather_http("movement");

  /* with different movement, make different adjust, change the command*/
  if (mov == "forward"){
    HZ_L = HZ_L_btn + 3;
    HZ_R = HZ_R_btn + 3;
    Serial.println("[click] forward"+String(HZ_L)+String(HZ_R));
  }else if (mov == "left"){
    HZ_L = HZ_L_btn - 3;
    HZ_R = HZ_R_btn + 3;          
    Serial.println("[click] left");
  }else if (mov =="right"){
    HZ_L = HZ_L_btn + 3;
    HZ_R = HZ_R_btn - 3;
    Serial.println("[click] right");
  }else if (mov == "stop"){
    HZ_L = 0;
    HZ_R = 0;
    Serial.println("[click] stop");
  }else if (mov == "None"){
    /* no new button has been pressed*/
  }else{
    /* other expection*/
    Serial.println("[ERROR] wrong movent command, http code: " + mov);
  }
  /* consider threshold, adjust command HZ*/
  if (HZ_L >= threshold) HZ_L = threshold;
  if (HZ_R >= threshold) HZ_R = threshold;
  if (HZ_L <= 0) HZ_L = 0;
  if (HZ_R <= 0) HZ_R = 0;

  /* print data and send to server */
  /*time_now = millis();
  Serial.println( "[HTTP] post:"+http_POST(HZ_L,HZ_R,ISR_HZ_L,ISR_HZ_R,map_x,map_y)+" ,used time:"+(millis()-time_now));*/
  
}


int return_command_hz_btn(char which)
{
  if (which == 'l')
    return HZ_L;
  if (which == 'r')
    return HZ_R;
}

#endif /* BTN_CONTROL_H */
