#include "NumberClock.h"
#include "Buttons.h"

NumberClock *my_clock = NULL;
Buttons twtw_button = Buttons(&buttonEvent, 150, 2);
Buttons setup_button = Buttons(&setupEvent, 150, 4);
Buttons incr_button = Buttons(&incrEvent, 150, 5);

void setup() {
  // put your setup code here, to run once:
  // my_clock.init();
  Serial.begin(9600);
  my_clock = new NumberClock();
}

void loop() {
  // put your main code here, to run repeatedly:
  updateBCL();
  my_clock->updating();
  twtw_button.checkStates();
  setup_button.checkStates();
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
  if (my_clock->setupState == 0) {
    my_clock->setUpMode();
    my_clock->setupState = 1;
  } else if (my_clock->setupState == 1) {
    my_clock->setupState = 2;
  } else if (my_clock->setupState == 2) {
    unsigned int t = my_clock->excSetUp();
    my_clock->setClockTime(t);
    my_clock->setupState = 0;
  }
}

void incrEvent() {
  if (my_clock->setupState == 1){
    my_clock->plusHour();
  } else if (my_clock->setupState == 2) {
    my_clock->plusMin();
  }
}
