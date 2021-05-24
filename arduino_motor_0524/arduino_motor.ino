#include "control.h"
#include "ISR.h"
#include <stdlib.h>

//motors defines
#define acc_pin_1 A6
#define rev_pin_1 3
#define acc_pin_2 4
#define rev_pin_2 5
MOTOR motor_L(acc_pin_1,rev_pin_1);
MOTOR motor_R(acc_pin_2,rev_pin_2);

void setup() {
  Serial.begin(115200);
  Serial.flush();
  Serial.print("tare finished");
  ISR_setup();//initial ISR
  pinMode(9,OUTPUT);
}

void loop() {
  analogWrite(9,100);
  
}
