#include "control.h"
#include <Arduino.h>

#include<esp32-hal-dac.h>

MOTOR::MOTOR(int accpin,int revpin){
  acc_pin=accpin;
  rev_pin=revpin;
}
MOTOR::~MOTOR() {

}

void MOTOR::go(int acc){
  dacWrite(acc_pin,acc);
}

void MOTOR::reverse_go(int acc,bool rev){
  dacWrite(acc_pin,0);
  digitalWrite(rev_pin,~rev);
  dacWrite(acc_pin,acc);
}
void MOTOR::shut_down(){
  dacWrite(acc_pin,0);
}

int MOTOR::get_acc(){
  return acc;
}
int MOTOR::get_rev(){
  return rev;
}
