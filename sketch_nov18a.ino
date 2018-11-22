#include "NumberClock.h"
#include "Buttons.h"

NumberClock *my_clock = NULL;
Buttons twtw_button = Buttons(&buttonEvent, 150, 2);
Buttons *setup_button = NULL;
Buttons incr_button = Buttons(&incrEvent, 150, 5);

char setupState = 0;

void setup() {
  // put your setup code here, to run once:
  // my_clock.init();
  my_clock = new NumberClock();
  setup_button = new Buttons(&setupEvent, 150, 4);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  updateBCL();
  my_clock->updating();
  twtw_button.checkStates();
  setup_button->checkStates();
  incr_button.checkStates();
}

void updateBCL() {
  analogWrite(10, 0.05 * analogRead(A0));
  // Serial.println(0.05 * analogRead(A0));
}

void buttonEvent() {
  my_clock->changeTwtwState();
}

void setupEvent() {
  if (setupState == 0) {
    my_clock->setUpMode();
    setupState = 1;
  }
}

void incrEvent() {
  Serial.println(1);
}
