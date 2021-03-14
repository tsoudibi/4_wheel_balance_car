#include "control.h"
#include "Arduino.h"

MOTOR::MOTOR(int accpin,int revpin){
  acc_pin=accpin;
  rev_pin=revpin;
}
MOTOR::~MOTOR() {
  free(acc_pin);
  free(rev_pin);
  free(acc);
  free(rev);

}

void MOTOR::go(int acc){
  analogWrite(acc_pin,acc);
}

void MOTOR::reverse_go(int acc,int rev){
  analogWrite(acc_pin,0);
  digitalWrite(rev_pin,~rev);
  analogWrite(acc_pin,acc);
}
void MOTOR::shut_down(){
  analogWrite(acc_pin,0);
}

int MOTOR::get_acc(){
  return acc;
}
int MOTOR::get_rev(){
  return rev;
}
