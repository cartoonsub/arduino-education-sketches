
#define relay_pin D1 //  mosfet
#define relay_pin2 D2 //  mosfet
#define LED_BUILTIN 2


void setup()
{
  Serial.begin(9600);
  Serial.println("Init AHT10 Failure.");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  pinMode(relay_pin2, OUTPUT);
 
  
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  delay(3000);
  digitalWrite(relay_pin, 0);
  digitalWrite(relay_pin, 1);
  digitalWrite(LED_BUILTIN, LOW);
  delay(3000);
  digitalWrite(relay_pin, 1);
  digitalWrite(relay_pin, 0);
  digitalWrite(LED_BUILTIN, HIGH);
}
