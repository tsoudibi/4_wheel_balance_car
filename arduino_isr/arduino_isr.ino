#include"ISR_timer1.h"
#include <avr/wdt.h>  

void setup() {
  Serial.begin(115200);
  wdt_enable(WDTO_1S);
  ISR_setup1();
 // ISR_setup2();
}

void loop() {
  wdt_reset();
}
