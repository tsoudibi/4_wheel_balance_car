#include <HardwareSerial.h>
#include "esp32_http.h"


#ifndef ARDUINO_CONNECT_H
#define ARDUINO_CONNECT_H

HardwareSerial Serialbyarduino(1);
HardwareSerial Serialbyardunio_2(2);

String get_string; //已接收字串
char store_string[50] ; //存取字串 
//int asciitonumber;
//int get_int;
//int valuenumber = 2;
int valuebyHX711[] = {0};//data return by HX711 

// control signal from arduino
int signal_L = 50;
int signal_R = 50;


/*[HTTP]debug*/
unsigned long int time_now;

void setup_hard_serial(){
    Serialbyarduino.begin(115200, SERIAL_8N1, 14, 15);//baud rate, mode, RX, TX
}

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
  // control signal from arduino
  signal_L = value[3];
  signal_R = value[4];
  // mass center at map
  int map_x,map_y;
  map_x = value[1];
  map_y = value[2];
  time_now = millis();
  Serial.println( "[sens] map:("+String(map_x)+", "+String(map_y)+") speed:("+String(signal_L)+", "+String(signal_R)+") ");
  Serial.println( "[HTTP] post:"+http_POST(signal_L,signal_R,map_x,map_y)+" ,used time:"+(millis()-time_now));
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


#endif /* ARDUINO_CONNECT_H */
