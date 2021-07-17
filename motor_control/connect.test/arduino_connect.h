#include <HardwareSerial.h>



#ifndef ARDUINO_CONNECT_H
#define ARDUINO_CONNECT_H

HardwareSerial Serialbyarduino(1);
HardwareSerial Serialbyarduino_2(2);

String get_string;
String get_string_2;

String get_string_T;
String get_string_2_T;

char store_string[50] ; 
char store_string_2[50] ;

void MissingDataToDeal(String str1,String str2);

int valuebyHX711[] = {0};//data return by HX711 

// control signal from arduino
int ISR_left = 0;
int ISR_right = 0;

int signal_X = 0;
int signal_Y = 0;

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

   MissingDataToDeal(get_string,get_string_2);
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
  
  ISR_left = value[1];
  ISR_right = value[2];
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

  signal_X = value_2[1];
  signal_Y = value_2[2];
  
}

int returnvalue(int n)
{
  if(n == 1)
    return ISR_left;
  if(n == 2)
    return ISR_right;
  if(n == 3)
    return signal_X;
  if(n == 4)
    return signal_Y; 
}

void MissingDataToDeal(String str1,String str2)
{
  if(str1 == NULL)
  {
    str1 = get_string_T;
  }
  if(str2 == NULL)
  {
    str1 = get_string_2_T;
  }
}
#endif /* ARDUINO_CONNECT_H */
