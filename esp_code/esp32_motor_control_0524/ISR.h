#include"Queue.h"

#define ledpin 13

Queue<unsigned long> queue = Queue<unsigned long>(1000);
unsigned long interrupt_time ;
int HZ;
static unsigned long last_time = 0;

void ICACHE_RAM_ATTR frequencyInterrupt()
{
 int a,b;
 Serial.println("123");
 a = digitalRead(2);
 while(a != b)
 {
  interrupt_time = millis();
  queue.push(interrupt_time);
  while(interrupt_time - queue.peek() > 1000)
  {
    queue.pop();  
  }
 }
 HZ = (queue.count()/2*1.0019)-1.0984;
 Serial.println(HZ);
 b = a;
}

void ISR_setup()
{
  Serial.println("ISR set up");
  pinMode(ledpin,OUTPUT);
  timer1_isr_init();
  timer1_attachInterrupt(frequencyInterrupt);
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
  timer1_write(1000);  
}
void ISR_loop()
{

  
}
