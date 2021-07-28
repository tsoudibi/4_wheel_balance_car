/* arduino_connect.h
 * this header connect esp32 with two Arudino
 * one for encoder, send the current motor speed (all mode)
 * another for sensor, send the mass coordinate (all mode) and command HZ (only on sensor_contol mode)*/


#include <HardwareSerial.h>
#ifndef ARDUINO_CONNECT_H
#define ARDUINO_CONNECT_H

HardwareSerial Serialbyarduino_1(1);
HardwareSerial Serialbyarduino_2(2);

String get_string_1;
String get_string_2;

String get_string_1_Tmp;
String get_string_2_Tmp;

char store_string_1[50] ; 
char store_string_2[50] ;

int valuebyHX711[] = {0};//data return by HX711 

// control signal from arduino
int ISR_left = 0;
int ISR_right = 0;

int signal_X = 0;
int signal_Y = 0;

int Command_speed_l = 0;
int Command_speed_r = 0;


void setup_hard_serial(){
    //encoder
    Serialbyarduino_1.begin(115200, SERIAL_8N1, 32, 33);//baud rate, mode, RX, TX
    //sensor
    Serialbyarduino_2.begin(115200, SERIAL_8N1, 14, 15);//baud rate, mode, RX, TX
}

void read_uart_encoder(){
  while(Serialbyarduino_1.available()){
    get_string_1 = Serialbyarduino_1.readStringUntil('\n');
  }//encoder
  if(get_string_1 == NULL ) get_string_1 = get_string_1_Tmp;
  get_string_1_Tmp = get_string_1;
}

void read_uart_sensor(){
  while(Serialbyarduino_2.available()){
    get_string_2 = Serialbyarduino_2.readStringUntil('\n');
  }//sensor
  if(get_string_2 == NULL ) get_string_2 = get_string_2_Tmp;
  get_string_2_Tmp = get_string_2;
}


void str2char_encoder(){
  int store_string_len_1 = get_string_1.length() + 1;       //encoder
  get_string_1.toCharArray(store_string_1, store_string_len_1); //encoder
}
void str2char_sensor(){
  int store_string_len_2 = get_string_2.length() + 1;   //sensor
  get_string_2.toCharArray(store_string_2, store_string_len_2); //sensor
}

void split_encoder(){
  //////////////////////sensor Serial_1//////////////////////////
  char *arr_1;
  arr_1 = strtok(store_string_1, ",");
  int i = 0;
  int value_1[3];
  while (arr_1 != NULL)
  {
    value_1[++i] = atoi(arr_1); 
    arr_1 = strtok(NULL, ",");
  }
  
  ISR_left = value_1[1];
  ISR_right = value_1[2];
}
void split_sensor(){
  //////////////////////encoder Serial_2//////////////////////////
  char *arr_2;
  arr_2 = strtok(store_string_2, ",");
  int j = 0;
  int value_2[5];
  while (arr_2 != NULL)
  {
    value_2[++j] = atoi(arr_2); 
    arr_2 = strtok(NULL, ",");
  }

  signal_X = value_2[1];
  signal_Y = value_2[2];
  Command_speed_l = value_2[3];
  Command_speed_r = value_2[4];
}

int get_serial_data(int n)
{
  if(n == 1)
    return ISR_left;
  if(n == 2)
    return ISR_right;
  if(n == 3)
    return signal_X;
  if(n == 4)
    return signal_Y;
  if(n == 5)
    return Command_speed_l;
  if(n == 6)
    return Command_speed_r; 
}

void update_data_all(){
  read_uart_encoder();
  read_uart_sensor();
  str2char_encoder();
  str2char_sensor();
  split_encoder();
  split_sensor();
}

#endif /* ARDUINO_CONNECT_H */
