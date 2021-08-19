#ifndef ISR_tumer1_h
#define ISR_tumer1_h

#include "Arduino.h"
#include "Queue.h"


Queue<unsigned long> queue(200);
Queue<unsigned long> queue2(200);
/* 
Example Timer1 Interrupt
Flash LED every second
*/

#define ledPin 13
int timer1_counter;
float HZ_l;
float HZ_r;

int first_vol_l=0;
int next_vol_l=0;
int first_vol_r=0;
int next_vol_r=0;
unsigned long timer_watchdog=0;
void ISR_setup()
{
  pinMode(ledPin, OUTPUT);
  
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  // Set timer1_counter to the correct value for our interrupt interval
  //timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz 64911
  //timer1_counter = 64286;   // preload timer 65536-16MHz/256/50Hz 64286
  timer1_counter = 65470;   // preload timer 65536-16MHz/256/2Hz 34286
  //HZ=65536/(timer1_counter-65536)*-1;
                            //timer_counter = 65536 - 65536/frequency 500hz太大 300hz和偵測 
                            //差4.多倍
                            /*300HZ 65317
                             *1000HZ 65470-* 2000HZ 65503
                             */
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts ();             // enable all interrupts
}

unsigned long time_2  ;
unsigned long time_1  ;

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{  //測量訊息

  timer_watchdog = millis();
  TCNT1 = timer1_counter;   // preload timer
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
  
  next_vol_l=digitalRead(6);
  next_vol_r=digitalRead(7);
  
  if(next_vol_l != first_vol_l )
  {
    time_1 = millis();
    queue.push(time_1);
    
 
   // Serial.println("-------");
   while(time_1 - queue.peek()>1500){
    queue.pop();
   }
    first_vol_l = next_vol_l ;
    
  }

  
  if(next_vol_r != first_vol_r)
  {
    time_2 = millis();
    queue2.push(time_2);
    while(time_2 - queue2.peek()>1500){
      queue2.pop();
    }
    first_vol_r = next_vol_r;
    
  }

  if(timer_watchdog - queue.peek()>2500)
  {
    queue.pop();
  }
  if(timer_watchdog - queue2.peek()>2500)
  {
    queue2.pop();
  }
  HZ_r = ((queue2.count()/2*1.0019)-1.0984)/1.5;
  HZ_l = ((queue.count()/2*1.0019)-1.0984)/1.5;
  //避免出現負數//
  if(HZ_r < 0)
  {
    HZ_r = 0;
  }
  if(HZ_l < 0)
  {
    HZ_l = 0;
  }
  Serial.print(HZ_l);
  Serial.print(",");
  Serial.println(HZ_r);
} 
  

  




   









#endif
