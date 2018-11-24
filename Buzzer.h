class Buzzer {
  int *melody;
  int *noteDurations;
  int cur_i;
  int len;

  unsigned long previousMillis;

  public:
    boolean working;
    // Though destruction is needed to prevent memory leak, but in our application
    // we will not delete BackgroundBuzzer instance, so comment it out anyway.
    // ~BackgroundBuzzer() {delete melody; delete noteDurations;}
    void updating();
    void buzz(int *m, int *nD, int l);
};

void Buzzer::updating() {
  unsigned long currentMillis = millis();
  if (working && (currentMillis - previousMillis > 1300 / noteDurations[cur_i])) {
    if (++cur_i == len) {
      noTone(3);
      working = false;
    } else {
      tone(3, melody[cur_i], 1000 / noteDurations[cur_i]);
      previousMillis = currentMillis;
    }
  }
}

void Buzzer::buzz(int *m, int *nD, int l) {
  melody = m;
  noteDurations = nD;
  cur_i = 0;
  len = l;

  tone(3, melody[cur_i], 1000 / noteDurations[cur_i]);

  working = true;
  previousMillis = millis();
}
