#include "sensor_control.h"
#include "esp32_http.h"
#include "motor_control.h"
#include "btn_control.h"

#include "arduino_connect.h"
#include <esp32-hal-dac.h>
#include "pid.h"
#include "PID_v1.h"


void setup(){
  
  pinMode(motor_l,OUTPUT);
  pinMode(motor_r,OUTPUT);
  Serial.begin(115200);
  //serial set
  setup_hard_serial();
  
  //motor set
  motor_setup();

  //wifi connection set
  wifi_connect();
  http_INIT();

  //PID set
  PID_setup();
}

void loop(){
  /*make one motor reverse*/
  digitalWrite(rev_pin,LOW);
  /*from server, get control mode*/
  int control_mode=0;
  String response=http_GET("control_mode");
  if (response=="button"){
    control_mode=1;
  }else if (response=="sensor"){
    control_mode=2;
  }else if (response=="camera"){
    control_mode=3;
  }else{
    Serial.println("[ERRO] wrong mode response from sever");
    Serial.println(response);
  }
  /*depend on mode pick a function to run*/
  switch(control_mode){
    case 1:
      btn_control();
      break;
    case 2:
      sensor_control();
      break;
  }
}
