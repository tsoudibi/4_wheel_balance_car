#ifndef FILTERS_h
#define FILTERS_h
#include "Arduino.h"


/*alpha filter*/
#define ALPHA 0.02
int Alpha_Filter(float a) {
  int NewValue;
  NewValue = a;
  int Value = (int)((float)NewValue * ALPHA + (0.5 - ALPHA) * (float)Value);
  return Value;
}


#endif j
