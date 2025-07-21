
#include <Wire.h>
#include <Thinary_AHT10.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
AHT10Class AHT10;

#define relay_pin D5 //  mosfet
#define pwm_pin D6 //  PWM
#define LED_BUILTIN 2

// Timer variables
unsigned long previousMillis = 0;
const long interval = 1000; // 1 second interval

// Relay timer variables
unsigned long previousRelayMillis = 0;
const long relayInterval = 2000; // 2 second interval
bool relayState = false;

// Fan speed timer variables
unsigned long previousFanMillis = 0;
const long fanInterval = 10000; // 10 second interval
int fanSpeed = 10; // Start at 10%
int fanStep = 10; // Increase by 10% each time


// display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
  Serial.begin(115200);
  analogWriteFreq(2000);   // Set PWM frequency to 2kHz (better for fans)
  analogWriteRange(1023);  // Set PWM range to 0-1023

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(2000); // Pause for 2 seconds
  testscrolltext(0.0, 0.0);
  // Clear the buffer
  display.clearDisplay();
    
  Wire.begin();
  if (AHT10.begin(eAHT10Address_Low)) {
    Serial.println("Init AHT10 Success.");
  } else {
    Serial.println("Init AHT10 Failure.");
  }

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  pinMode(pwm_pin, OUTPUT);
  digitalWrite(relay_pin, 0);
  digitalWrite(LED_BUILTIN, HIGH);
  
  // Set initial fan speed to 10% (10% of 1023 = 102)
  analogWrite(pwm_pin, 102);
}

void loop()
{
  unsigned long currentMillis = millis();
  
  // Fan speed timer - change every 10 seconds
  if (currentMillis - previousFanMillis >= fanInterval) {
    previousFanMillis = currentMillis;
    
    // Calculate PWM value (fanSpeed% of 1023)
    int pwmValue = (fanSpeed * 1023) / 100;
    analogWrite(pwm_pin, pwmValue);
    
    Serial.print("Fan speed: ");
    Serial.print(fanSpeed);
    Serial.print("% (PWM: ");
    Serial.print(pwmValue);
    Serial.println(")");
    
    // Increase speed by 10%
    fanSpeed += fanStep;
    
    // Reset to 10% when reaching 110%
    if (fanSpeed > 100) {
      fanSpeed = 10;
    }
  }
  
  // Relay timer - blink every 2 seconds
  if (currentMillis - previousRelayMillis >= relayInterval) {
    previousRelayMillis = currentMillis;
    relayState = !relayState;
    // digitalWrite(relay_pin, relayState);
    digitalWrite(relay_pin, HIGH); // Turn relay on
    Serial.print("Relay state: ");
    Serial.println(relayState ? "ON" : "OFF");
  }
  
  // Sensor reading timer - every 1 second
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Read sensor data
    float temp = AHT10.GetTemperature();
    float hum = AHT10.GetHumidity();
    
    // Print to console
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println("°C");
    
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.println("%");
    
    Serial.println("-----------------------------");
    
    // Display on screen
    testscrolltext(temp, hum);
  }
}

void testscrolltext(float temp, float hum) {
  display.clearDisplay();

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
    display.println("Temp: ");
    display.println(temp);
    display.println("C. Hum: ");
    display.println(hum);
    display.println("%");
    display.display();
}
