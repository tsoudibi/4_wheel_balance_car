#include"control.h"
#include"Queue.h"
#include<esp32-hal-dac.h>
Queue<unsigned long> myQ_l = Queue<unsigned long>(1000);
Queue<unsigned long> myQ_r = Queue<unsigned long>(1000);

int HZ_l,HZ_r;
int a_l,b_l;
int a_r,b_r;

const double kp =2, ki = 0.00005, kd = 0;

double pwm;

void isr();

double PIDcontrol(double order,double in)
{ 
  unsigned long current_time;
  unsigned long previous_time = 0;
  double time_difference;
  double error;
  double last_error;
  double accumulated_error=0,rate_of_error;
  current_time=millis();
  time_difference=(double)(current_time - previous_time);
  error=order-in;
  accumulated_error+=error*time_difference;
  rate_of_error=(error-last_error)/time_difference;
  double out=(in+(kp*error+ki*accumulated_error+kd*rate_of_error))*0.4485+94.382;
  Serial.println(out);
  last_error=error;
  previous_time=current_time;

  return out; 
}

void setup() {
  Serial.begin(9600);
  //ISR_setup();
  pinMode(26,OUTPUT);
  pinMode(25,OUTPUT);
  pinMode(14,OUTPUT);
  pinMode(18,INPUT);//encoding l
  pinMode(19,INPUT);//emcoding r
  pwm = 100;
  dacWrite(25,pwm);
  dacWrite(26,pwm);
  Serial.print("test begin");
}

void loop() {
  //dacWrite(25,10);
  double hz_l = pwm*2.22-209.4;
  double hz_r = pwm*2.22-209.4;

  isr(); 
  Serial.println("-----------");
  HZ_l = myQ_l.count();
  HZ_r = myQ_r.count();
  Serial.print(HZ_l);
  Serial.print(",");
  Serial.println(HZ_r);

  dacWrite(25,PIDcontrol(hz_l,HZ_l));
  dacWrite(26,PIDcontrol(hz_r,HZ_r));
}

void isr()
{
  unsigned long interrupt_time[2];
  a_l = digitalRead(18);
  if(a_l != b_l){
      interrupt_time[0] = millis();
      myQ_l.push(interrupt_time[0]);
      while(interrupt_time[0] - myQ_l.peek() > 1000){
         myQ_l.pop();
         }
      }
      //Serial.println(HZ_l);  
   b_l = a_l;
   interrupt_time[0] = NULL;
  a_r = digitalRead(19);
  if(a_r != b_r){
      interrupt_time[1] = millis();
      myQ_r.push(interrupt_time[1]);
      while(interrupt_time[1] - myQ_r.peek() > 1000){
         myQ_r.pop();
      }
     }     
  b_r = a_r;
  interrupt_time[1] = NULL;
}
