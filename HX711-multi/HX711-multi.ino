#include "HX711-multi.h"

// Pins to the load cell amp
#define CLK A0      // clock pin to the load cell amp
#define DOUT1 A1    // data pin to the first lca
#define DOUT2 A2    // data pin to the second lca
#define DOUT3 A3    // data pin to the third lca
#define DOUT4 A4    // data pin to the third lca

#define BOOT_MESSAGE "MIT_ML_SCALE V0.8"

#define TARE_TIMEOUT_SECONDS 4

#define FILTER_A 0.01

//#define motor1 2

//#define motor2 3

int Filter(float a);

byte DOUTS[1] = {DOUT1, DOUT2, DOUT3, DOUT4};///////////

#define CHANNEL_COUNT sizeof(DOUTS)/sizeof(byte)

long int results[CHANNEL_COUNT];

HX711MULTI scales(CHANNEL_COUNT, DOUTS, CLK);

int fblocation = 0;
int rllocation = 0;

int motor1 = 0;
int motor2 = 0;

int Value;

void setup() {
  Serial.begin(115200);
  Serial.println(BOOT_MESSAGE);
  Serial.flush();
  
  pinMode(11,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  
  reset();
    
  tare();
}
//motor 控制
void motor() {
  int mode;
  
  /////////////前進時//////////////////
  
  if(rllocation < 200 &
  & rllocation >= 0){
    if(fblocation < 200 && fblocation > -200)
     mode = 1;//原地不動
    if(fblocation > 200 && fblocation < 400 || fblocation > -400 && fblocation < -200) 
     mode = 2;//直線加速
    if(fblocation < -400)
     mode = 3;//左轉
    if(fblocation > 400)
     mode = 4;//右轉        
  }
  if(rllocation>200 && rllocation <= 400){
    if(fblocation < 400 && fblocation >= -400)
     mode = 2;//直線加速
    if(fblocation < -400)
     mode = 3;//左轉
    if(fblocation >= 400)
     mode = 4;//右轉
    }
  if(rllocation > 400) {
    if(fblocation < 150 && fblocation >= -150)
     mode = 2;//直線前進
    if(fblocation < -150)
     mode = 3;//左轉
    if(fblocation > 150)
     mode = 4;//右轉  
    }
    
  ///////////////後退時////////////////////
    
  if(rllocation <= 0 && rllocation > -200 ){
    if(fblocation < 200 && fblocation > -200)
     mode = 1;//原地不動
    if(fblocation > 200 && fblocation < 400 || fblocation > -400 && fblocation < -200) 
     mode = 2;//直線加速
    if(fblocation < -400)
     mode = 3;//左轉
    if(fblocation > 400)
     mode = 4;//右轉        
  }
  if(rllocation < -200 && rllocation >= -400){
    if(fblocation < 400 && fblocation >= -400)
     mode = 2;//直線加速
    if(fblocation < -400)
     mode = 3;//左轉
    if(fblocation >= 400)
     mode = 4;//右轉
    }
  if(rllocation < -400) {
    if(fblocation < 150 && fblocation >= -150)
     mode = 2;//直線前進
    if(fblocation < -150)
     mode = 3;//左轉
    if(fblocation > 150)
     mode = 4;//右轉  
    }   
  return mode;    
}
   

  unsigned long tareStartTime = millis();
void tare() {
  bool tareSuccessful = false;
  while (!tareSuccessful && millis()<(tareStartTime+TARE_TIMEOUT_SECONDS*1000)) {
    tareSuccessful = scales.tare(20,10000);  //reject 'tare' if still ringing
  }
}

void sendRawData() {
  scales.read(results);
  for (int i=0; i<scales.get_count(); ++i) {
    results[i] = results[i]/100;
    
    results[i] = Filter[results[i]];

    if(results[i] > 0)
     results[i] = 0;
    if(results[i] < -10000)
     results[i] = -10000;

    results[i] = map(results[i],-10000,0,-1023,0);

    //Serial.println(-results[i]);
    
    
    weight(i);
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
   /*fblocation = map(fblocation,-20000,20000,-1023,1023);
   rllocation = map(rllocation,-20000,20000,-1023,1023);*/
}

// 濾波
int Filter(float a) {
  int NewValue;
  NewValue = a;
  Value = (int)((float)NewValue * FILTER_A + (0.5 - FILTER_A) * (float)Value);
  return Value;
}

/*void motoroutput(int mode){
  switch(mode){
    case 1;
      motor1 = 0;
      motor2 = 0;
      break;  
    case 2;
      
    case 3;
    
    case 4;
    }
  };*/


//避免一直無限增加
void reset() {
  fblocation = 0;
  rllocation = 0;
}
void loop() {
  
  int x,y;
  
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
  //轉換PWM訊號給馬達
  
      
 /* analogWrite(motor1,x);
  analogWrite(motor2,y);

  Serial.println("馬達");
  Serial.print("左:,右:");
  Serial.print(x);
  Serial.print("     ");
  Serial.print(y);*/
  
  reset();
}
