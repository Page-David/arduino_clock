class Buzzer {
  unsigned char *melody;
  unsigned int *noteDurations;
  int cur_i;
  int len;

  unsigned long previousMillis;

  public:
    Buzzer() {
      volume(0x1E);
      working = false;
    }
    boolean working;
    // Though destruction is needed to prevent memory leak, but in our application
    // we will not delete BackgroundBuzzer instance, so comment it out anyway.
    ~Buzzer() {delete melody; delete noteDurations;}
    void volume(unsigned char vol);
    void play(unsigned char Track);
    void updating();
    void buzz(unsigned char *m, unsigned int *nD, int l);
};

void Buzzer::volume(unsigned char vol) {
  unsigned char volume[5] = {0xAA,0x13,0x01,vol,vol+0xBE};
  Serial.write(volume, 5);
}

void Buzzer::updating() {
  unsigned long currentMillis = millis();
  if (working && (currentMillis - previousMillis > 800 * noteDurations[cur_i])) {
    if (++cur_i == len) {
      working = false;
      free(melody);
      free(noteDurations);
    } else {
      play(melody[cur_i]);
      previousMillis = currentMillis;
    }
  }
}

void Buzzer::buzz(unsigned char *m, unsigned int *nD, int l) {
  melody = m;
  noteDurations = nD;
  cur_i = 0;
  len = l;

  melody = (unsigned char *)malloc(l * sizeof(m[0]));
  noteDurations = (unsigned int *)malloc(l * sizeof(nD[0]));

  for (int i = 0; i < l; ++i) {
    melody[i] = m[i];
    noteDurations[i] = nD[i];
  }

  play(melody[cur_i]);
  working = true;
  previousMillis = millis();
}

void Buzzer::play(unsigned char Track) {
  unsigned char play[6] = {0xAA,0x07,0x02,0x00,Track,Track+0xB3};
  Serial.write(play,6);
}
