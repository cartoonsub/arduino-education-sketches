void setup() {
   Serial.begin(9600);
    Serial.setTimeout(50);
}

void loop() {
  // put your main code here, to run repeatedly:
if (Serial.available() > 0)
  {
    int value = Serial.parseInt();
    Serial.println(String(value));
    delay(1000);
    }
}
