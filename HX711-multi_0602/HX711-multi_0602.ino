#include "HX711-multi.h"
#include "filters.h"

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
  resetX_Y();
  tare();
}
void loop() {
  int acc;
  int test_time;
  sendRawData(); //this is for sending raw data, for where everything else is done in processing
  
  Serial.print(map_X);
  Serial.print(",");
  Serial.print(map_Y);
  Serial.print(",");
  //on serial data (any data) re-tare
  if (Serial.available()>0) {
    while (Serial.available()) {
    }
  tare();
  }
  speeddef(motor());
  resetX_Y();
}
void weight(int i) {
  switch(i) {
    case 0:
      map_Y = 0.7*(map_Y - results[i]);
      map_X = 0.7*(map_X - results[i]);
    break;
    case 1:
      map_Y = 0.7*(map_Y - results[i]);
      map_X = 0.7*(map_X + results[i]);
    break;
    case 2:
      map_Y = 0.5*(map_Y + results[i]);
      map_X = 0.5*(map_X + results[i]);
    break;
    case 3:
      map_Y = 0.5*(map_Y + results[i]);
      map_X = 0.5*(map_X - results[i]);
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
int motor() {
  int motor_mode;
  /////////////前進時//////////////////
  
  if(map_Y < -300 && map_Y >= -350){
    if(map_X < 100 && map_X >=-100)
    motor_mode = 1;//原地不動 
    /*if(map_X >= 110 && map_X <150 || map_X < -50 && map_X > -150) 
    motor_mode = 2;//直線加*/
    if(map_X > 100)
    motor_mode = 4;//右轉
    if(map_X < -100)
    motor_mode = 3;//左轉      
  }
  if(map_Y > -300 && map_Y <= 200){
    if(map_X < 45 && map_X >=-45)
      motor_mode = 2;//直線加速
    if(map_X < -45)
      motor_mode = 3;//左轉
    if(map_X >= 45)
      motor_mode = 4;//右轉
    }
  if(map_Y > 200) {
    if(map_X < 150 && map_X >= -150)
     motor_mode = 2;//直線前進
    if(map_X < -150)
     motor_mode = 3;//左轉
    if(map_X > 150)
     motor_mode = 4;//右轉  
    }
    
  ///////////////後退時////////////////////
    
  if(map_Y > -200 && map_Y <= -300){
    if(map_X < 100 && map_X >=-100)
    motor_mode = 1;//原地不動
    if(map_X >= 100 && map_X < 200 || map_X < -100 && map_X > -200) 
    motor_mode = 5;//直線加速
    if(map_X > 200)
    motor_mode = 7;//右轉
    if(map_X < -200)
    motor_mode = 6;//左轉      
  }
  if(map_Y < -300 && map_Y >= -400){
    if(map_X < 200 && map_X >=-200)
      motor_mode = 5;//直線加速
    if(map_X < -200)
      motor_mode = 6;//左轉
    if(map_X >= 200)
      motor_mode = 7;//右轉
    }
  if(map_Y < -400) {
    if(map_X < 150 && map_X >= -150)
     motor_mode = 5;//直線前進
    if(map_X < -150)
     motor_mode = 6;//左轉
    if(map_X > 150)
     motor_mode = 7;//右轉  
    }
  return motor_mode;      
}

void speeddef(int motor_mode){
  int speed_l;
  int speed_r;
  switch(motor_mode){
    case(1):
      speed_l = 0;
      speed_r = 0;
    break;
    case(2):
      speed_l = (map_Y+100)*0.6;
      speed_r = (map_Y+100)*0.6;
    break;
    case(3):
      speed_l = 0.7*(map_Y+100);
      speed_r = 0.7*(map_Y+100)-map_X;
    break;
    case(4):
      speed_l = 0.7*(map_Y+100)+map_X;
      speed_r = 0.7*(map_Y+100);
    break;
    /*case(5):
      speed_l = -(map_Y+100);
      speed_r = -(map_Y+100);
     break; 
    case(6):
      speed_l = 0.7*map_X-0.7*map_Y;
      speed_r = 0.7*map_Y;
    break;
    case(7):
      speed_l = 0.7*map_X-0.7*map_Y;
      speed_r = 0.7*map_Y;*/        
  }
  Serial.print(speed_l);
  Serial.print(",");
  Serial.println(speed_r);
}
