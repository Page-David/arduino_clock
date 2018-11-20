#include "NumberClock.h"

NumberClock *my_clock = NULL;

void setup() {
  // put your setup code here, to run once:
  // my_clock.init();
  my_clock = new NumberClock();
}

void loop() {
  // put your main code here, to run repeatedly:
  my_clock->updating();
}
