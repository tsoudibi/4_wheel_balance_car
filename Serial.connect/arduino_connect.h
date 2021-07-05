#include <HardwareSerial.h>



#ifndef ARDUINO_CONNECT_H
#define ARDUINO_CONNECT_H

HardwareSerial Serialbyarduino(1);
HardwareSerial Serialbyarduino_2(2);

String get_string;
String get_string_2;

char store_string[50] ; 
char store_string_2[50] ;

int valuebyHX711[] = {0};//data return by HX711 

// control signal from arduino
int signal_1 = 0;
int signal_2 = 0;

int signal_3 = 0;
int signal_4 = 0;

/*[HTTP]debug*/
unsigned long int time_now;

void setup_hard_serial(){
    Serialbyarduino.begin(115200, SERIAL_8N1, 14, 15);//baud rate, mode, RX, TX
    Serialbyarduino_2.begin(115200, SERIAL_8N1, 32, 33);//baud rate, mode, RX, TX
}

void read_uart(){
  while(Serialbyarduino.available()){
    get_string = Serialbyarduino.readStringUntil('\n');
  }
  while(Serialbyarduino_2.available()){
    get_string_2 = Serialbyarduino_2.readStringUntil('\n');
  }
}


void strtochar(){
  int store_string_len = get_string.length() + 1;
  int store_string_len_2 = get_string_2.length() + 1;  
  
  get_string.toCharArray(store_string, store_string_len);
  get_string_2.toCharArray(store_string_2, store_string_len_2);
}

void scan(){
  ////////////////////Serial_1//////////////////////////
  char *arr;
  arr = strtok(store_string, ",");
  int i = 0;
  int value[3];
  while (arr != NULL)
  {
    value[++i] = atoi(arr); 
    arr = strtok(NULL, ",");
  }

  signal_1 = value[1];
  signal_2 = value[2];
  ////////////////////Serial_2//////////////////////////
  char *arr_2;
  arr_2 = strtok(store_string_2, ",");
  int j = 0;
  int value_2[3];
  while (arr_2 != NULL)
  {
    value_2[++j] = atoi(arr_2); 
    arr_2 = strtok(NULL, ",");
  }

  signal_3 = value_2[1];
  signal_4 = value_2[2];
  
}

int returnvalue(int n)
{
  if(n == 1)
    return signal_1;
  if(n == 2)
    return signal_2;  
}
int returnvalue_2(int n)
{
  if(n == 1)
    return signal_3;
  if(n == 2)
    return signal_4;  
}

#endif /* ARDUINO_CONNECT_H */
