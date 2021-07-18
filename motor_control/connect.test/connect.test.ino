#include "arduino_connect.h"
#include <esp32-hal-dac.h>
#include"pid.h"
#include"PID_v1.h"
///////control motor////////
#define motor_l 25
#define motor_r 26

int HZ_l;
int HZ_r;

int speed_X ,speed_Y;

int speed_left = 0; 
int speed_right = 0;


void ToGetValueFromSerial()
{
  for(int i=1;i<=4;i++)
  {
    if(i==1)
    {
      HZ_l=returnvalue(i);
      
    }
    else if(i==2)
    {
      HZ_r=returnvalue(i);
    
    }
    else if(i==3)
    {
      speed_X=returnvalue(i);
      /*Serial.print(speed_X);
      Serial.print(",");*/
    }
    else if(i==4)
    {
      speed_Y=returnvalue(i);
      /*Serial.print(speed_Y);
      Serial.print(",");*/
    }
    Serial.println(String(HZ_l)+","+String(HZ_r));
  }
}

void setup() 
{
 pinMode(motor_l,OUTPUT);
 pinMode(motor_r,OUTPUT);
 Serial.begin(115200);
 setup_hard_serial();
 SetPID();
 ////test////
 
 delay(500);
}

void loop() {
 /////Serial connect/////
  read_uart();
  strtochar();
  scan();
  ToGetValueFromSerial();
 /////PID control for motor///// 
  CommandHZ(7,7);
  EncodingHZ (HZ_l,HZ_r);
  pidcontrol_left();
  pidcontrol_right();
  OutputSignal();

}
