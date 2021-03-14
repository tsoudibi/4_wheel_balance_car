#ifndef CONTROL_h
#define CONTROL_h

#include "Arduino.h"




class MOTOR
{
  private:
    int acc_pin;
    int rev_pin;
    int acc;
    int rev;

  public:
    
    MOTOR(int accpin,int revpin);
    ~MOTOR();
    void go(int acc);
    void reverse_go(int acc,int rev);
    void shut_down();
    
    int get_acc();
    int get_rev();
  
};
#endif 
