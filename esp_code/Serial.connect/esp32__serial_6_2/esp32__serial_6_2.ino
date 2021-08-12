#include "arduino_connect.h"
#include "esp32_http.h"
#include <esp32-hal-dac.h>//for contronl
/*control setting*/
#define motor_l 25
#define motor_r 26
#define rev_pin 35

void setup(){
  Serial.begin(115200);
  setup_hard_serial();
  
  //motor set
  pinMode(motor_l,OUTPUT);
  pinMode(motor_r,OUTPUT);
  pinMode(rev_pin,OUTPUT);

  //wifi connection
  wifi_connect();
  http_INIT();
}
void loop(){
  digitalWrite(rev_pin,LOW);
  /*read_uart();
  strtochar();
  scan();*/
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
  switch(control_mode){
    case 1:
      btn_control();
      break;
  }
}


void motor_control(){
  int speed_l;
  int speed_r;
  speed_l = 1.4*get_control_signal('l') ;
  speed_r = 1.4*get_control_signal('r') ;
  
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
  time_now = millis();
  Serial.println( "[HTTP] post:"+http_POST(speed_l,speed_r)+" ,used time:"+(millis()-time_now));
}


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
