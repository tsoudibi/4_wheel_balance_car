#include "HX711-multi.h"
#include "control.h"
#include "filters.h"

// Pins to the load cell amp
#define CLK A0      // clock pin to the load cell amp
#define DOUT1 A1    // data pin to the first lca
#define DOUT2 A2    // data pin to the second lca
#define DOUT3 A3    // data pin to the third lca
#define DOUT4 A4    // data pin to the fourth lca

//HX711 defines
#define TARE_TIMEOUT_SECONDS 4

//motors defines
#define acc_pin_1 2
#define rev_pin_1 3
#define acc_pin_2 4
#define rev_pin_2 5
MOTOR motor_L(acc_pin_1,rev_pin_1);
MOTOR motor_R(acc_pin_2,rev_pin_2);

//HX711 initials
byte DOUTS[4] = {DOUT1, DOUT2, DOUT3, DOUT4};
#define CHANNEL_COUNT sizeof(DOUTS)/sizeof(byte)
long int results[CHANNEL_COUNT];
HX711MULTI scales(CHANNEL_COUNT, DOUTS, CLK);
unsigned long tareStartTime = millis();


//mapping define
int map_Y = 0;
int map_X = 0;


void setup() {
  Serial.begin(115200);
  Serial.flush();
  resetX_Y();
  tare();
  Serial.print("tare finished");

  //motor_L.go(123);
  //motor_R.go(123);
}
void loop() {

  sendRawData(); //this is for sending raw data, for where everything else is done in processing
  Serial.print(map_X);
  Serial.print(",");
  Serial.println(map_Y);

  //on serial data (any data) re-tare
  if (Serial.available()>0) {
    while (Serial.available()) {
      Serial.read();
    }
    tare();
  }
  //轉換PWM訊號給馬達

  resetX_Y();
}
void weight(int i) {
  switch(i) {
    case 0:
      map_Y = map_Y - results[i];
      map_X = map_X - results[i];
    break;
    case 1:
      map_Y = map_Y - results[i];
      map_X = map_X + results[i];
    break;
    case 2:
      map_Y = map_Y + results[i];
      map_X = map_X + results[i];
    break;
    case 3:
      map_Y = map_Y + results[i];
      map_X = map_X - results[i];
    break;

  }
   /*map_Y = map(map_Y,-20000,20000,-1023,1023);
   map_X = map(map_X,-20000,20000,-1023,1023);*/
}
//避免一直無限增加
void resetX_Y() {
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

   // results[i] = map(results[i],-10000,0,-1023,0);

    //Serial.println(-results[i]);
    
    
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
//motor 控制
void motor() {
  int mode;
  
  /////////////前進時//////////////////
  
  if(map_X < 200 && map_X >= 0){
    if(map_Y < 200 && map_Y > -200)
     mode = 1;//原地不動
    if(map_Y > 200 && map_Y < 400 || map_Y > -400 && map_Y < -200) 
     mode = 2;//直線加速
    if(map_Y < -400)
     mode = 3;//左轉
    if(map_Y > 400)
     mode = 4;//右轉        
  }
  if(map_X>200 && map_X <= 400){
    if(map_Y < 400 && map_Y >= -400)
     mode = 2;//直線加速
    if(map_Y < -400)
     mode = 3;//左轉
    if(map_Y >= 400)
     mode = 4;//右轉
    }
  if(map_X > 400) {
    if(map_Y < 150 && map_Y >= -150)
     mode = 2;//直線前進
    if(map_Y < -150)
     mode = 3;//左轉
    if(map_Y > 150)
     mode = 4;//右轉  
    }
    
  ///////////////後退時////////////////////
    
  if(map_X <= 0 && map_X > -200 ){
    if(map_Y < 200 && map_Y > -200)
     mode = 1;//原地不動
    if(map_Y > 200 && map_Y < 400 || map_Y > -400 && map_Y < -200) 
     mode = 2;//直線加速
    if(map_Y < -400)
     mode = 3;//左轉
    if(map_Y > 400)
     mode = 4;//右轉        
  }
  if(map_X < -200 && map_X >= -400){
    if(map_Y < 400 && map_Y >= -400)
     mode = 2;//直線加速
    if(map_Y < -400)
     mode = 3;//左轉
    if(map_Y >= 400)
     mode = 4;//右轉
    }
  if(map_X < -400) {
    if(map_Y < 150 && map_Y >= -150)
     mode = 2;//直線ㄥ前進
    if(map_Y < -150)
     mode = 3;//左轉
    if(map_Y > 150)
     mode = 4;//右轉  
    }   
  return mode;    
}
