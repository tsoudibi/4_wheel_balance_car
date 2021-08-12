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

int first_state=0;
int next_state=0;
int first_state2=0;
int next_state2=0;

void ISR_setup1()
{
  pinMode(ledPin, OUTPUT);

  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  // Set timer1_counter to the correct value for our interrupt interval
  //timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz 64911
  //timer1_counter = 64286;   // preload timer 65536-16MHz/256/50Hz 64286
  timer1_counter = 64911;   // preload timer 65536-16MHz/256/2Hz 34286
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

  
  TCNT1 = timer1_counter;   // preload timer
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
  
  next_state=digitalRead(8);
  next_state2=digitalRead(9);

  if(next_state != first_state )
  {
    time_1 = millis();
    queue.push(time_1);
    
 
   // Serial.println("-------");
   while(time_1 - queue.peek()>1000){
    queue.pop();
   }
    first_state = next_state ;
    HZ_l = (queue.count()/2*1.0019)-1.0984;
  }

  
  if(next_state2 != first_state2)
  {
    time_2 = millis();
    queue2.push(time_2);
    while(time_2 - queue2.peek()>1000){
      queue2.pop();
    }
    first_state2 = next_state2;
    HZ_r = (queue2.count()/2*1.0019)-1.0984;; 
  }
  Serial.print("left");
  Serial.print(HZ_l);
  Serial.print("rignt");
  Serial.println(HZ_r);
}
  

  



void ISR_loop()
{
  // your program here...
}


int fre_return()
{
  return HZ_l;
}
   









#endif
