# 4_wheel_balance_car
#include <HardwareSerial.h>
#include <esp32-hal-dac.h>
#include "esp32_http.h"

HardwareSerial Serialbyarduino(1);
HardwareSerial Serialbyardunio_2(2);

String get_string; //已接收字串
char store_string[50] ; //存取字串 
int asclltonumber;
int get_int;
int valuenumber = 2;
int valuebyHX711[] = {0};
int motor_l = 25;
int motor_r = 26;
//dara return by HX711 
int x = 0;
int y = 0;
int rev_pin = 35;

/*[HTTP]debug*/
unsigned long int time_now;
 
void read_uart(){
  while(Serialbyarduino.available()){
    get_string = Serialbyarduino.readStringUntil('\n');
  }
  
}

void strtochar(){
  int store_string_len = get_string.length() + 1;
  get_string.toCharArray(store_string, store_string_len);
}



void scan(){
  char *arr;
  arr = strtok(store_string, ",");
  int i = 0;
  int value[4];
  while (arr != NULL){
        value[++i] = atoi(arr); 
        arr = strtok(NULL, ",");
    }
  x = value[3];
  y = value[4];
  int map_x,map_y;
  map_x = value[1];
  map_y = value[2];
  time_now = millis();
  Serial.println( "[HTTP] post:"+http_POST(x,y,map_x,map_y)+" ,used time:"+(millis()-time_now));
  Serial.print(map_x);
  Serial.print(",");
  Serial.print(map_y);
  Serial.print(",");
  Serial.print(x);
  Serial.print(",");
  Serial.println(y);  
}

void motor_control(){
  int speed_l;
  int speed_r;
  speed_l = 1.4*x ;
  speed_r = 1.4*y ;
  
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
}

void setup(){
  Serial.begin(115200);
  Serialbyarduino.begin(115200, SERIAL_8N1, 14, 15);//baud rate, mode, RX, TX
  
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
  if (http_GET("control_mode")=="button"){
    if (http_GET("new")=="1"){
      String mov=http_GET("movment");
      if (mov== "forward"){
          x+=15;
          y+=15;}else
      if (mov== "left"){
          x+=10;}else
      if (mov=="right"){
          y+=10;}else
      if (mov== "stop"){
          x=0;
          y=0;
      }else{
        Serial.println("error");
      }
    }
  }
    motor_control();
}
