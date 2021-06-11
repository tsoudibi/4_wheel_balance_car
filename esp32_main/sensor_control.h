#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H
#include <HardwareSerial.h>
#include "esp32_http.h"
#include "motor_control.h"

HardwareSerial Serialbyarduino(1);
HardwareSerial Serialbyardunio_2(2);

String get_string; //已接收字串
char store_string[50] ; //存取字串 

int valuebyHX711[] = {0};//data return by HX711 



/*[HTTP]debug*/
unsigned long int time_now;

void setup_hard_serial(){
    Serialbyarduino.begin(115200, SERIAL_8N1, 14, 15);//baud rate, mode, RX, TX
}



void sensor_control(){
  /*section 1: read from UART*/
  while(Serialbyarduino.available()){
    get_string = Serialbyarduino.readStringUntil('\n');
  }
  /*section 2: change string into char array*/
  int store_string_len = get_string.length() + 1;
  get_string.toCharArray(store_string, store_string_len);
  /*section 3: split the array*/
  char *arr;
  arr = strtok(store_string, ",");//split store string
  int i = 0;
  int value[4];
  while (arr != NULL){
        value[++i] = atoi(arr); 
        arr = strtok(NULL, ",");
    }
  // control signal from arduino
  int signal_L = value[3];
  int signal_R = value[4];
  set_control_signal(signal_L,signal_R);
  // mass center at map
  int map_x,map_y;
  map_x = value[1];
  map_y = value[2];
  /*section 4: print data and send to server*/
  time_now = millis();
  Serial.println( "[sens] map:("+String(map_x)+", "+String(map_y)+") speed:("+String(signal_L)+", "+String(signal_R)+") ");
  Serial.println( "[HTTP] post:"+http_POST(signal_L,signal_R,0,0,map_x,map_y)+" ,used time:"+(millis()-time_now));
  motor_control();
}




#endif /* SENSOR_CONTROL_H */
