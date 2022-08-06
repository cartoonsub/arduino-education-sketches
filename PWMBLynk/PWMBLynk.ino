
#define BLYNK_TEMPLATE_ID "TMPLffcTD0eT"
#define BLYNK_DEVICE_NAME "Quickstart Device"
#define BLYNK_AUTH_TOKEN  "Ccm-FcGms7D_r32zVaNgBm3cabiltiEt"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <Wire.h>
#include <Thinary_AHT10.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "wifiHotSpot";
char pass[] = "zasada13422442";

BlynkTimer timer;
AHT10Class AHT10;

#define relay_pin D5 //  mosfet
#define pwm_pin D6 //  PWM
#define LED_BUILTIN 2

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
