#include <Servo.h>
#define LED_BUILTIN 2
Servo servo;
int counter = 1;

void setup() {
  counter = 0;
  Serial.begin(9600);
  Serial.setTimeout(50);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  servo.attach(14); //D5 
  delay(2000); // ждем 2 секунды
  Serial.println("Start: " + String(servo.read()));
  
}

// the loop function runs over and over again forever
void loop() {
  counter = counter +1;
  Serial.println("Количество циклов: " + String(counter));
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);

  if (Serial.available()) {
     int value = Serial.parseInt();
     Serial.println("input: " + String(value));
     if (value != 0) {
      servo.write(value);
     }
     
  }
}
