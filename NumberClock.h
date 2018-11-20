#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"

#include <Fonts/repet.h>

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

  unsigned int getCurrentTime();
  String timeToString(unsigned int t);

  public:
    NumberClock() {
        tft = new Adafruit_ILI9340(_cs, _dc, _rst);
        unsigned int t = getCurrentTime();
        tft->begin();
        tft->setRotation(1);
        tft->fillScreen(ILI9340_BLACK);
        tft->setCursor(0, 50);
        tft->setTextColor(ILI9340_WHITE);
        tft->setFont(&MyFont_Regular40pt7b);
        tft->print(timeToString(t));
        previousTime = t;
        flick = true;
        previousMillis = millis();
    }
    void init();
    void updating();
};

unsigned int NumberClock::getCurrentTime() {
  return 25 * 30 + 9;
}

void NumberClock::updating() {
  unsigned int t = getCurrentTime();
  if (t != previousTime) {
    tft->setCursor(0, 50);
    tft->setTextColor(ILI9340_BLACK);
    tft->print(timeToString(previousTime));
    tft->setTextColor(ILI9340_WHITE);
    tft->print(timeToString(t));
    previousTime = t;
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    tft->setCursor(88, 56);
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
