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

void Buttons::checkStates() {
  if (digitalRead(buttonPin)) {
    if (previousPressed){
      if(millis() - previousMillis >= interval) {
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
