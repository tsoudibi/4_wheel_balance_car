#include <HardwareSerial.h>

HardwareSerial Serialbyarduino(1);

String get_string; //已接收字串
char store_string[10] ; //存取字串 
int asclltonumber;
int get_int;

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
  while (arr != NULL){
   Serial.print("arr ");
   Serial.print(arr);
   Serial.println("");
   arr = strtok(NULL, ","); 
  }  
}



void setup(){
  Serial.begin(115200);
  Serialbyarduino.begin(115200, SERIAL_8N1, 14, 15);//baud rate, mode, RX, TX
}

void loop(){
  read_uart();
  strtochar();
  scan();
}
