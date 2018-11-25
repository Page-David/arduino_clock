class Buttons {
  int buttonPin;
  unsigned int interval;
  unsigned long previousMillis;
  boolean previousPressed;
  void (*event)();
  
  public:
    Buttons(void (*foo)(), int i, int bP) {
      buttonPin = bP;
      previousMillis = millis();
      previousPressed = false;
      event = foo;
      interval = i;
    }
    void checkStates();
};

class LongShortButtons {
  int buttonPin;
  unsigned int shortInterval;
  unsigned int longInterval;
  unsigned long previousMillis;
  boolean previousPressed;
  boolean shortPressed;
  boolean limited;
  void (*shortEvent)();
  void (*longEvent)();
  
  public:
    LongShortButtons(void (*foo)(), void (*bar)(), int i1, int i2, int bP) {
      buttonPin = bP;
      previousMillis = millis();
      previousPressed = false;
      shortPressed = false;
      shortEvent = foo;
      longEvent = bar;
      shortInterval = i1;
      longInterval = i2;
    }
    void checkStates();
};

void Buttons::checkStates() {
  if (digitalRead(buttonPin)) {
    if (previousPressed){
      if (millis() - previousMillis >= interval) {
        event();
        previousPressed = false;
      }
    } else {
      previousMillis = millis();
      previousPressed = true;   
    }
  } else {
    previousPressed = false;
  }
}

void LongShortButtons::checkStates() {
  if (digitalRead(buttonPin)) {
    if (previousPressed && !limited){
      if (millis() - previousMillis >= longInterval) {
        longEvent();
        previousPressed = false;
        shortPressed = false;
        limited = true;
      } else if (millis() - previousMillis >= shortInterval) {
        shortPressed = true;
      }
    } else {
      previousMillis = millis();
      previousPressed = true;   
    }
  } else {
    if (shortPressed) {
      shortEvent();
      shortPressed = false;
    }
    if (limited) limited = false;
    previousPressed = false;
  }
}
