#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"

#include <Fonts/repet.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>

#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

#include <EEPROM.h>

#include "Buzzer.h"

#include "DHT.h"

#define DHTPIN 7

#define _sclk 13
// #define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

DHT dht(DHTPIN, DHT22);

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

  int *melody;
  int *noteDurations;
  int noteLength;

  public:
    unsigned int previousTime;
    char setupState;
    int alarmSetupState;
    NumberClock() {
        tft = new Adafruit_ILI9340(_cs, _dc, _rst);
        unsigned int t = getCurrentTime();
        tft->begin();
        tft->setRotation(1);
        tft->fillScreen(ILI9340_BLACK);
        tft->setFont(&MyFont_Regular40pt7b);
        tft->drawLine(10, 150, 310, 150, ILI9340_WHITE);
        tft->drawLine(128, 160, 128, 225, ILI9340_WHITE);
        printTime(t, ILI9340_WHITE);
        previousTime = t;
        flick = true;
        previousMillis = millis();
        twtw = false;
        setupEnabled = false;
        temp_t = 0;
        setupState = 0;
        alarmSetupState = 0;
        readAlarms();
        getNextAlarm();
        printNextAlarm(ILI9340_WHITE);
        ifbuzzed = false;
        my_buzzer = Buzzer();
        dht.begin();
        printDHT();
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
    void printNextAlarm(unsigned long int color);
    void showPrompt(String s, boolean clean);
    void clearUp();
    void alarmWriteOut(unsigned int (&a)[3]);
    void changeAlarmEnb();
    void setMelody(int *m, int *l, const int len);
    void testAlarm();
    void printDHT();
    String stringCut(float x);
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
    if (ifbuzzed) {
      getNextAlarm();
      printNextAlarm(ILI9340_WHITE);
      ifbuzzed = false;
    }
    printDHT();
  }
  if (!setupEnabled && !ifbuzzed && my_alarm_enb.enabled
    && t == my_alarm.alarmTimes[nextAlarmIdx]) {
    my_buzzer.buzz(melody, noteDurations, noteLength);
    ifbuzzed = true;
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    tft->setCursor(143, 100);
    if (flick) {
      tft->setTextColor(ILI9340_BLACK);
      if (setupEnabled) {
        if (setupState == 1) {
          tft->fillRect(30, 47, 120, 55, ILI9340_BLACK);
          tft->fillRect(280, 87, 31, 20, ILI9340_BLACK);
        }
        else {
          tft->fillRect(180, 47, 95, 55, ILI9340_BLACK);
        }
      } else if (ifbuzzed) {
        tft->fillRect(30, 47, 300, 55, ILI9340_BLACK);
      } else {
        tft->print(":");
      }
    } else {
      tft->setTextColor(ILI9340_WHITE);
      if (setupEnabled) {
        printTime(temp_t, ILI9340_WHITE);
      } else if (ifbuzzed) {
        printTime(previousTime, ILI9340_WHITE);
      } else {
        tft->print(":");
      }
    }
    flick = !flick;
    previousMillis = currentMillis;
  }
  my_buzzer.updating();
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
      t = t - 720; // 720 = 60 * 12
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
  getNextAlarm();
  printNextAlarm(ILI9340_WHITE);
  if (ifbuzzed) {
    ifbuzzed = false;
  }
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
  clearUp();
  twtw = !twtw;
  printTime(previousTime, ILI9340_WHITE);
  printNextAlarm(ILI9340_WHITE);
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

void NumberClock::printNextAlarm(unsigned long int color) {
  tft->setTextColor(color);
  tft->setFont(&FreeMonoBoldOblique12pt7b);
  tft->setCursor(20, 200);
  tft->fillRect(22, 185, 100, 18, ILI9340_BLACK);
  if (my_alarm_enb.enabled) {
    unsigned int t = my_alarm.alarmTimes[nextAlarmIdx];
    boolean tw = false;
    if (twtw && t / 60 > 12) {
      t = t - 720; // 708 = 60 * 12
      tw = true;
    }
    tft->print(timeToString(t));
    if (twtw) {
      if (tw) tft->print("PM");
      else tft->print("AM");
    }
  } else {
    tft->print("ALA");
    tft->setCursor(75, 200);
    tft->print("OFF");
  }
  tft->setFont(&MyFont_Regular40pt7b);
}

void NumberClock::showPrompt(String s, boolean clean) {
  if (clean) {
    tft->setTextColor(ILI9340_BLACK);
  } else {
    tft->setTextColor(ILI9340_WHITE);
  }
  tft->setFont(&FreeMonoBoldOblique12pt7b);
  tft->setCursor(45, 25);
  tft->print(s);
  tft->setFont(&MyFont_Regular40pt7b);
}

void NumberClock::clearUp() {
  tft->fillRect(30, 47, 320, 55, ILI9340_BLACK);
}

void NumberClock::alarmWriteOut(unsigned int (&a)[3]) {
  for (int i = 0; i < 3; ++i) my_alarm.alarmTimes[i] = a[i];
  unsigned int temp;
  if (a[0] > a[1]) {
    temp = a[0];
    a[0] = a[1];
    a[1] = temp;
  }
  if (a[2] < a[0]) {
    temp = a[2];
    a[2] = a[1];
    a[1] = a[0];
    a[0] = temp;
  } else if (a[2] < a[1]) {
    temp = a[2];
    a[2] = a[1];
    a[1] = temp;
  }
  for (int i = 0; i < 6; ++i) {
    EEPROM.write(i, my_alarm.b[i]);
  }
  getNextAlarm();
  printNextAlarm(ILI9340_WHITE);
}

void NumberClock::changeAlarmEnb() {
  my_alarm_enb.enabled = !my_alarm_enb.enabled;
  EEPROM.write(6, my_alarm_enb.b[0]);
  printNextAlarm(ILI9340_WHITE);
}

void NumberClock::setMelody(int *m, int *l, const int len) {
  melody = m;
  noteDurations = l;
  noteLength = len;
}

void NumberClock::testAlarm() {
  if (!ifbuzzed) my_buzzer.buzz(melody, noteDurations, noteLength);
}

void NumberClock::printDHT() {
  tft->setTextColor(ILI9340_WHITE);
  tft->setFont(&FreeMonoBoldOblique12pt7b);
  tft->fillRect(135, 185, 168, 18, ILI9340_BLACK);
  tft->setCursor(135, 200);
  tft->print(stringCut(dht.readTemperature()) + "*C");
  tft->setCursor(240, 200);
  tft->print(stringCut(dht.readHumidity()));
  tft->setFont(&MyFont_Regular40pt7b);
}

String NumberClock::stringCut(float x) {
  char result[6] = "";
  dtostrf(x, 4, 1, result);
  return result;
}
