#ifndef BTN_CONTROL_H
#define BTN_CONTROL_H

#include "esp32_http.h"
#include "motor_control.h"

void btn_control(){
  if (http_GET("new")=="1"){
    String mov=http_GET("movement");
    int L=get_control_signal('l');
    int R=get_control_signal('r');
      if (mov== "forward"){
          set_control_signal(L+15,R+15);
          Serial.print("[move]forward");}else
      if (mov== "left"){
          set_control_signal(L,R+10);
          Serial.print("[move]left");}else
      if (mov=="right"){
          set_control_signal(L+10,R);
          Serial.print("[move]right");}else
      if (mov== "stop"){
          set_control_signal(0,0);
          Serial.print("[move]stop");
      }else{
        Serial.println("[ERRO] wrong movent command");
      }
  }
  motor_control();
}

#endif /* BTN_CONTROL_H */
