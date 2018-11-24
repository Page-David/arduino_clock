#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"

#include <Fonts/repet.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>

#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

#include <EEPROM.h>

#include "Buzzer.h"

#define _sclk 13
// #define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

union alarmData {
  unsigned int alarmTimes[3];
  byte b[6];
};

union alarmEnabled {
  boolean enabled;
  byte b[1];
};

class NumberClock {
  Adafruit_ILI9340 *tft = NULL;
  unsigned int previousTime;
  boolean flick;
  unsigned long previousMillis;

  tmElements_t tm;

  unsigned int getCurrentTime();
  String timeToString(unsigned int t);

  boolean twtw;

  boolean setupEnabled;
  unsigned int temp_t;

  alarmData my_alarm;
  alarmEnabled my_alarm_enb;
  int nextAlarmIdx;
  boolean ifbuzzed;
  Buzzer my_buzzer;

  public:
    char setupState;
    NumberClock() {
        tft = new Adafruit_ILI9340(_cs, _dc, _rst);
        unsigned int t = getCurrentTime();
        tft->begin();
        tft->setRotation(1);
        tft->fillScreen(ILI9340_BLACK);
        tft->setFont(&MyFont_Regular40pt7b);
        tft->drawLine(10, 140, 310, 140,ILI9340_WHITE);
        printTime(t, ILI9340_WHITE);
        previousTime = t;
        flick = true;
        previousMillis = millis();
        twtw = false;
        setupEnabled = false;
        temp_t = 0;
        setupState = 0;
        readAlarms();
        getNextAlarm();
        ifbuzzed = false;
        my_buzzer = Buzzer();
    }
    void init();
    void updating();
    void printTime(unsigned int t, unsigned long int color);
    void setUpMode();
    unsigned int excSetUp();
    void setClockTime(unsigned int t);
    String toTwoDigits(unsigned int t);
    void changeTwtwState();
    void plusMin();
    void plusHour();
    void readAlarms();
    void getNextAlarm();
};

unsigned int NumberClock::getCurrentTime() {
  RTC.read(tm);
  return 60 * tm.Hour + tm.Minute;
}

void NumberClock::updating() {
  unsigned int t = getCurrentTime();
  if (t != previousTime && !setupEnabled) {
    printTime(previousTime, ILI9340_BLACK);
    printTime(t, ILI9340_WHITE);
    previousTime = t;
    if (ifbuzzed) ifbuzzed = false;
  }
  if (!setupEnabled && my_alarm_enb.enabled && t == my_alarm.alarmTimes[nextAlarmIdx]) {
    my_buzzer.buzz();
    ifbuzzed = true;
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    tft->setCursor(143, 100);
    if (flick) {
      tft->setTextColor(ILI9340_BLACK);
      if (setupEnabled){
        if (setupState == 1) {
          tft->fillRect(30, 47, 120, 55, ILI9340_BLACK);
          tft->fillRect(280, 87, 31, 20, ILI9340_BLACK);
        }
        else {
          tft->fillRect(180, 47, 95, 55, ILI9340_BLACK);
        }
      } else {
        tft->print(":");
      }
    } else {
      tft->setTextColor(ILI9340_WHITE);
      if (setupEnabled) {
        printTime(temp_t, ILI9340_WHITE);
      } else {
        tft->print(":");
      }
    }
    flick = !flick;
    previousMillis = currentMillis;
  }
}

String NumberClock::toTwoDigits(unsigned int t) {
  if (t < 10) {
    return "0" + String(t);
  } else {
    return String(t);
  }
}

String NumberClock::timeToString(unsigned int t) {
  return toTwoDigits(t / 60) + ":" + toTwoDigits(t % 60);
}

void NumberClock::printTime(unsigned int t, unsigned long int color) {
  tft->setTextColor(color);
  if (twtw) {
    tft->setFont(&FreeMonoBoldOblique12pt7b);
    tft->setCursor(280, 100);
    if (t / 60 <= 12) {
      tft->print("AM");
    } else {
      t = t - 708; // 708 = 60 * 12
      tft->print("PM");
    }
    tft->setFont(&MyFont_Regular40pt7b);
  }
  tft->setCursor(55, 100);
  tft->print(timeToString(t));
}

void NumberClock::setUpMode() {
  setupEnabled = true;
  temp_t = previousTime;
}

unsigned int NumberClock::excSetUp() {
  setupEnabled = false;
  return temp_t;
}

void NumberClock::setClockTime(unsigned int t) {
  tft->fillRect(30, 47, 320, 55, ILI9340_BLACK);
  previousTime = t;
  printTime(t, ILI9340_WHITE);
  RTC.read(tm);
  tm.Minute = t % 60;
  tm.Hour = t / 60;
  RTC.write(tm);
}

void NumberClock::plusHour() {
  if (temp_t / 60 == 23) {
    temp_t = temp_t % 60;
  } else {
    temp_t = temp_t + 60;
  }
}

void NumberClock::plusMin() {
  if (temp_t % 60 == 59) {
    temp_t = (temp_t / 60) * 60;
  } else {
    temp_t = temp_t + 1;
  }
}

void NumberClock::changeTwtwState() {
  tft->fillRect(30, 47, 320, 55, ILI9340_BLACK);
  twtw = !twtw;
  printTime(previousTime, ILI9340_WHITE);
}

void NumberClock::readAlarms() {
  for (int i = 0; i < 6; ++i) {
    my_alarm.b[i] = EEPROM.read(i);
  }
  my_alarm_enb.b[0] = EEPROM.read(6);
}

void NumberClock::getNextAlarm() {
  if (previousTime > my_alarm.alarmTimes[2] || previousTime < my_alarm.alarmTimes[0]){
    nextAlarmIdx = 0;
  } else if (previousTime > my_alarm.alarmTimes[1]) {
    nextAlarmIdx = 2;
  } else {
    nextAlarmIdx = 1;
  }
}
