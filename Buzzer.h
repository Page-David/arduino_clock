class Buzzer {
  public:
    boolean working;
    Buzzer() {
      working = false;
    }
    void buzz();
};

void Buzzer::buzz() {
  Serial.println("hi");
}
