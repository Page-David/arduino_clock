#include "NumberClock.h"
#include "Buttons.h"
#include "pitch.h"
#include <EEPROM.h>

union alarmMusic {
  int idx;
  byte b[2];
};

NumberClock *my_clock = NULL;
Buttons twtw_button = Buttons(&buttonEvent, 150, 2);
Buttons setup_button = Buttons(&setupEvent, 150, 4);
Buttons incr_button = Buttons(&incrEvent, 150, 5);
LongShortButtons alarm_button = LongShortButtons(&alarmShort, &alarmLong, 150, 2000, 6);

int melody[3][8] = {{NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4},
                    {NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, 0},
                    {NOTE_A4, NOTE_B4, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_E5, NOTE_B4, 0}};
int noteDurations[3][8] = {{4, 8, 8, 4, 4, 4, 4, 4},
                           {4, 4, 4, 4, 4, 4, 2, 0},
                           {4, 4, 2, 4, 2, 2, 1, 0}};

unsigned int a[3] = {0, 0, 0};
String promptString = "Alarm";

alarmMusic my_alamu;

void setup() {
  // put your setup code here, to run once:
  // my_clock.init();
  Serial.begin(9600);
  my_clock = new NumberClock();
  my_clock->setMelody(melody[1], noteDurations[1], 8);
  my_alamu.b[0] = EEPROM.read(7);
  my_alamu.b[1] = EEPROM.read(8);
  // Serial.println(my_alamu.idx);
}

void loop() {
  // put your main code here, to run repeatedly:
  updateBCL();
  my_clock->updating();
  twtw_button.checkStates();
  setup_button.checkStates();
  incr_button.checkStates();
  alarm_button.checkStates();
}

void updateBCL() {
  analogWrite(10, 0.05 * analogRead(A0));
  // Serial.println(0.05 * analogRead(A0));
}

void buttonEvent() {
  my_clock->changeTwtwState();
}

void setupEvent() {
  if (my_clock->alarmSetupState == 0) {
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
  } else if (my_clock->setupState == 1) {
    my_clock->setupState = 2;
  } else if (my_clock->setupState == 2) {
    my_clock->setupState = 1;
  }
}

void incrEvent() {
  if (my_clock->setupState != 0){
    if (my_clock->setupState == 1){
      my_clock->plusHour();
    } else if (my_clock->setupState == 2) {
      my_clock->plusMin();
    }
  } else {
    changeAlarmMusic();
  }
}

void alarmShort() {
  if (my_clock->alarmSetupState != 0) {
    my_clock->clearUp();
    my_clock->showPrompt(promptString + (my_clock->alarmSetupState), true);
    if (my_clock->alarmSetupState == 3) {
      a[2] = my_clock->excSetUp();
      my_clock->previousTime = 1600;
      my_clock->alarmWriteOut(a);
      my_clock->setupState = 0;
      my_clock->alarmSetupState = 0;
    } else {
      my_clock->setupState = 1;
      a[my_clock->alarmSetupState - 1] = my_clock->excSetUp();
      my_clock->showPrompt(promptString + (++(my_clock->alarmSetupState)), false);
      my_clock->setUpMode();
    }
  } else {
    my_clock->changeAlarmEnb();
  }
}

void alarmLong() {
  if (my_clock->setupState == 0 && my_clock->alarmSetupState == 0) {
    my_clock->showPrompt(promptString + (++(my_clock->alarmSetupState)), false);
    my_clock->setupState = 1;
    my_clock->setUpMode();
  }
}

void changeAlarmMusic() {
  if (my_alamu.idx == 2) my_alamu.idx = 0;
  else ++(my_alamu.idx);
  EEPROM.write(7, my_alamu.b[0]);
  EEPROM.write(8, my_alamu.b[1]);
  my_clock->setMelody(melody[my_alamu.idx], noteDurations[my_alamu.idx], 8);
  my_clock->testAlarm();
}
