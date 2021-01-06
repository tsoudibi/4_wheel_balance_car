#include "HX711-multi.h"

// Pins to the load cell amp
#define CLK A0      // clock pin to the load cell amp
#define DOUT1 A1    // data pin to the first lca
#define DOUT2 A2    // data pin to the second lca
#define DOUT3 A3    // data pin to the third lca
#define DOUT4 A4    // data pin to the third lca-

#define BOOT_MESSAGE "MIT_ML_SCALE V0.8"

#define TARE_TIMEOUT_SECONDS 4

#define FILTER_A 0.01

int Filter(float a);

byte DOUTS[4] = {DOUT1, DOUT2, DOUT3, DOUT4};

#define CHANNEL_COUNT sizeof(DOUTS)/sizeof(byte)

long int results[CHANNEL_COUNT];

HX711MULTI scales(CHANNEL_COUNT, DOUTS, CLK);

int fblocation = 0;
int rllocation = 0;

int Value;

void setup() {
  Serial.begin(115200);
  Serial.println(BOOT_MESSAGE);
  Serial.flush();
  pinMode(11,OUTPUT);

  reset();
    
  tare();
}


void tare() {
  bool tareSuccessful = false;

  unsigned long tareStartTime = millis();
  while (!tareSuccessful && millis()<(tareStartTime+TARE_TIMEOUT_SECONDS*1000)) {
    tareSuccessful = scales.tare(20,10000);  //reject 'tare' if still ringing
  }
}

void sendRawData() {
  scales.read(results);
  for (int i=0; i<scales.get_count(); ++i) {
    results[i] = results[i]/10;
    /*Serial.print( -results[i]);  
    Serial.print( (i!=scales.get_count()-1)?",":"\n");*/

    results[i] = Filter[results[i]];

    if(results[i] > 0)
     results[i] = 0;

    /*Serial.print("濾波後:");
    Serial.print( -results[i]);  
    Serial.print( (i!=scales.get_count()-1)?",":"\n");*/
    
    weight(i);
   /* Serial.print("check");
    Serial.print(rllocation);
    Serial.print(" ");
    Serial.println(fblocation);*/
    delay(10);
  }  
  delay(10);
}

void weight(int i) {
  switch(i) {
    case 0:
    fblocation = fblocation - results[i];
    rllocation = rllocation - results[i];
    break;
    case 1:
    fblocation = fblocation - results[i];
    rllocation = rllocation + results[i];
    break;
    case 2:
    fblocation = fblocation + results[i];
    rllocation = rllocation - results[i];
    break;
    case 3:
    fblocation = fblocation + results[i];
    rllocation = rllocation + results[i];
    break;

  }

}

// 濾波
int Filter(float a) {
  int NewValue;
  NewValue = a;
  Value = (int)((float)NewValue * FILTER_A + (1.0 - FILTER_A) * (float)Value);
  return Value;
}

//避免一直無限增加
void reset() {
  fblocation = 0;
  rllocation = 0;
}
void loop() {
  
  sendRawData(); //this is for sending raw data, for where everything else is done in processing
  Serial.println("重心：");
  Serial.print("x:,Y:");
  Serial.print(rllocation);
  Serial.print("    ");
  Serial.println(fblocation);

  //on serial data (any data) re-tare
  if (Serial.available()>0) {
    while (Serial.available()) {
      Serial.read();
    }
    tare();
  }
  reset();
}
