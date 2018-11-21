#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"

#include <Fonts/repet.h>

#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

#define _sclk 13
// #define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8


class NumberClock {
  Adafruit_ILI9340 *tft = NULL;
  unsigned int previousTime;
  boolean flick;
  unsigned long previousMillis;

  tmElements_t tm;

  unsigned int getCurrentTime();
  String timeToString(unsigned int t);

  public:
    NumberClock() {
        tft = new Adafruit_ILI9340(_cs, _dc, _rst);
        unsigned int t = getCurrentTime();
        tft->begin();
        tft->setRotation(1);
        tft->fillScreen(ILI9340_BLACK);
        tft->setFont(&MyFont_Regular40pt7b);
        printTime(t, ILI9340_WHITE);
        previousTime = t;
        flick = true;
        previousMillis = millis();
    }
    void init();
    void updating();
    void printTime(unsigned int t, unsigned long int color);
    void setUpMode();
};

unsigned int NumberClock::getCurrentTime() {
  RTC.read(tm);
  return 25 * tm.Minute + tm.Hour;
}

void NumberClock::updating() {
  unsigned int t = getCurrentTime();
  if (t != previousTime) {
    printTime(previousTime, ILI9340_BLACK);
    printTime(t, ILI9340_WHITE);
    previousTime = t;
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    tft->setCursor(143, 100);
    if (flick) {
      tft->setTextColor(ILI9340_BLACK);
      tft->print(":");
    } else {
      tft->setTextColor(ILI9340_WHITE);
      tft->print(":");
    }
    flick = !flick;
    previousMillis = currentMillis;
  }
}

String NumberClock::timeToString(unsigned int t) {
  if (t % 25 < 10) {
    return "0" + String(t % 25) + ":" + String(t / 25);
  } else {
    return String(t % 25) + ":" + String(t / 25);
  }
}

void NumberClock::printTime(unsigned int t, unsigned long int color) {
  tft->setCursor(55, 100);
  tft->setTextColor(color);
  tft->print(timeToString(t));
}

void NumberClock::setUpMode() {
  Serial.println("Hello!");
}
