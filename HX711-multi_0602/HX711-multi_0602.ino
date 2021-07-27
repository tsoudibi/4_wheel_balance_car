#include "HX711-multi.h"
#include "filters.h"
#include "motorspeed.h"

// Pins to the load cell amp
#define CLK A0      // clock pin to the load cell amp
#define DOUT1 A1    // data pin to the first lca
#define DOUT2 A2    // data pin to the second lca
#define DOUT3 A3    // data pin to the third lca
#define DOUT4 A4    // data pin to the fourth lca

//HX711 defines
#define TARE_TIMEOUT_SECONDS 4


//HX711 initials
byte DOUTS[4] = {DOUT1, DOUT2, DOUT3, DOUT4};
#define CHANNEL_COUNT sizeof(DOUTS)/sizeof(byte)
long int results[CHANNEL_COUNT];
HX711MULTI scales(CHANNEL_COUNT, DOUTS, CLK);
unsigned long tareStartTime = millis();

//mapping define
int map_Y = 0;
int map_X = 0;
//motor_set

void setup() {
  Serial.begin(115200);
  Serial.flush();
  reset_XY();
  tare();
}
void loop() {
  int acc;
  int test_time;
  sendRawData(); //this is for sending raw data, for where everything else is done in processing

  //speedOutput
  SpeedOutput(map_X,map_Y);

  //on serial data (any data) re-tare
  if (Serial.available()>0) {
    while (Serial.available()) {
    }
    tare();
  }
  
  reset_XY();
}
void weight(int i) {
  switch(i) {
    case 0:
      map_Y = map_Y - results[i]*1.3;
      map_X = map_X - results[i]*1.3;
    break;
    case 1:
      map_Y = map_Y - results[i]*1.53;
      map_X = map_X + results[i]*1.3;
    break;
    case 2:
      map_Y = map_Y + results[i];
      map_X = map_X + results[i];
    break;
    case 3:
      map_Y = map_Y + results[i]*1.1818;
      map_X = map_X - results[i]*1.153;
    break;

  }
  if(abs(map_X)<10)
    map_X = 0;
  if(abs(map_Y)<10)
    map_Y = 0;
   /*map_Y = map(map_Y,-20000,20000,-1023,1023);
   map_X = map(map_X,-20000,20000,-1023,1023);*/
}
//避免一直無限增加
void reset_XY() {
  map_Y = 0;
  map_X = 0;
}
void sendRawData() {
  scales.read(results);
  for (int i=0; i<scales.get_count(); ++i) {
    results[i] = results[i]/100;
    
    results[i] = Alpha_Filter(results[i]);

    if(results[i] > 0)
     results[i] = 0;
    if(results[i] < -10000)
     results[i] = -10000;

    weight(i);
    delay(10);
  }  
  delay(10);
}
void tare() {
  bool tareSuccessful = false;
  while (!tareSuccessful && millis()<(tareStartTime+TARE_TIMEOUT_SECONDS*1000)) {
    tareSuccessful = scales.tare(20,10000);  //reject 'tare' if still ringing
  }
}
