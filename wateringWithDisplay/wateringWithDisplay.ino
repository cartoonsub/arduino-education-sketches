
#define BLYNK_TEMPLATE_ID "TMPLffcTD0eT"
#define BLYNK_DEVICE_NAME "Quickstart Device"
#define BLYNK_AUTH_TOKEN  "Ccm-FcGms7D_r32zVaNgBm3cabiltiEt"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <Wire.h>
#include <Thinary_AHT10.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// blynk
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "wifiHotSpot";
char pass[] = "zasada13422442";

BlynkTimer timer;
AHT10Class AHT10;

#define relay_pin D5 //  mosfet
#define pwm_pin D6 //  PWM
#define LED_BUILTIN 2


// display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

BLYNK_WRITE(V0)
{
  int value = param.asInt();
  if (value == 1) {
    Serial.println("V0 change 1");
    digitalWrite(relay_pin, 1);
    delay(60000);
    digitalWrite(relay_pin, 0);
    Blynk.virtualWrite(V0, 0);
    }

  if (value == 0) {
    digitalWrite(relay_pin, 0);
  }

  Blynk.virtualWrite(V1, value);
}

BLYNK_WRITE(V7)
{
  int value = param.asInt();
  if (value == 1) {
    digitalWrite(relay_pin, 1);
  }

  if (value == 0) {
    digitalWrite(relay_pin, 0);
  }
}

BLYNK_WRITE(V6)
{
  int value = param.asInt();
  if (value == 1) {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  if (value == 0) {
    digitalWrite(LED_BUILTIN, LOW);
  }

  Blynk.virtualWrite(V6, value);
}

BLYNK_WRITE(V9)
{
  int value = param.asInt();
  analogWrite(pwm_pin, value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);

  Blynk.virtualWrite(V4, AHT10.GetTemperature());
  Blynk.virtualWrite(V5, AHT10.GetHumidity());
}

void setup()
{
  Serial.begin(115200);
  analogWriteFreq(10000);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(2000); // Pause for 2 seconds
  testscrolltext();
  // Clear the buffer
  display.clearDisplay();
    
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, myTimerEvent);
  Wire.begin();
  if (AHT10.begin(eAHT10Address_Low)) {
    Serial.println("Init AHT10 Sucess.");
  } else {
    Serial.println("Init AHT10 Failure.");
  }

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, 0);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  Blynk.run();
  timer.run();
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println(F("let's go travel"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}
