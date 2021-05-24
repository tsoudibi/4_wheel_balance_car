#ifndef ISR_h
#define ISR_h

#include "Arduino.h"
#include "Queue.h"



Queue<unsigned long> queue = Queue<unsigned long>(1000);
Queue<unsigned long> queue_2 = Queue<unsigned long>(1000);
/* 
Example Timer1 Interrupt
Flash LED every second
*/

#define ledPin 13
int timer1_counter;
  int first_state=0;
  int next_state=0;
  int c = queue.count();
  int t=0;
  int HZ=0;
  int print_HZ=1;
  int allvalue;
  int offset; 
  

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
  HZ=65536/(timer1_counter-65536)*-1;
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

unsigned long time_2  = 0;
unsigned long time_1  ;

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  TCNT1 = timer1_counter;   // preload timer
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
  next_state=digitalRead(8);

  if(next_state != first_state )
  {
    time_1 = millis();
    queue.push(time_1);
    
 
   // Serial.println("-------");
   while(time_1 - queue.peek()>1000){
    queue.pop();
   }
  Serial.println ((queue.count()/2*1.0019 )-1.0984 );
  first_state = next_state ;
  HZ = (queue.count()/2*1.0019)-1.0984;
  
  /*queue_2.push(HZ);
  while(queue_2.count()>100)
  {
    allvalue += queue_2.peek();
    queue_2.pop();
    offset = allvalue / 100;
    //Serial.println(allvalue);
  }*/
  
  }
  

  ///
  
  //Serial.println(t);
  /*t+=1;
  if(first_state!=next_state){
    c+=1;
  }
  if(t>=1000){
    int test_frequency=c/2;
    //test_frequency = 1.0545*fitted_frequency + 0.0395
    int fitted_frequency = (test_frequency-0.0395)/1.0545;
    //Serial.println(millis());
    Serial.println(fitted_frequency);
    t=0;
    c=0;
  }
  first_state=next_state;*/
  

}

void ISR_loop()
{
  // your program here...
}


int fre_return()
{
  return HZ;
}
   









#endif
