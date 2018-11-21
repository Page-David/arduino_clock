#include "NumberClock.h"

NumberClock *my_clock = NULL;

void setup() {
  // put your setup code here, to run once:
  // my_clock.init();
  my_clock = new NumberClock();
  Serial.begin(9600);
  attachInterrupt(0, setUpCallback, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  updateBCL();
  my_clock->updating();
}

void setUpCallback() {
  delay(100);
  my_clock->setUpMode();
}

void updateBCL() {
  analogWrite(10, 0.05 * analogRead(A0));
  // Serial.println(0.05 * analogRead(A0));
}
