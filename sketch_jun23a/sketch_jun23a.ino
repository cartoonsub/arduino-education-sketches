
#define LED_BUILTIN 2


void setup()
{
  Serial.begin(9600);
  Serial.println("Init AHT10 Failure.");

  pinMode(LED_BUILTIN, OUTPUT);
 
  
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Fucked up");
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
}
