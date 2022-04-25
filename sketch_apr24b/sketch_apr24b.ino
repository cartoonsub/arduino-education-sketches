/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID           "TMPLffcTD0eT"
#define BLYNK_DEVICE_NAME           "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "Ccm-FcGms7D_r32zVaNgBm3cabiltiEt"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "wifiHotSpot";
char pass[] = "zasada13422442";

BlynkTimer timer;

#include <Wire.h>
#include <Thinary_AHT10.h>
AHT10Class AHT10;

#define relay_pin D5 // пин мосфета / реле
#define LED_BUILTIN 2

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  Serial.println("V0 change");
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();
  Serial.println(value);
  if (value == 1) {
    Serial.println("V0 change 1");
    digitalWrite(relay_pin, 1);
    }
  if (value == 0) {
    digitalWrite(relay_pin, 0);
    }
  // Update state
  Blynk.virtualWrite(V1, value);
}

BLYNK_WRITE(V6)
{
  // Set incoming value from pin V6 to a variable
  int value = param.asInt();
if (value == 1) {
    digitalWrite(LED_BUILTIN, HIGH);
    }
  if (value == 0) {
    digitalWrite(LED_BUILTIN, LOW);
    }
  // Update state
  Blynk.virtualWrite(V6, value);
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
  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
  Wire.begin();
  if(AHT10.begin(eAHT10Address_Low)) {
      Serial.println("Init AHT10 Sucess.");
    } else {
      Serial.println("Init AHT10 Failure.");
    }

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(relay_pin, OUTPUT);        // пин реле как выход
    digitalWrite(relay_pin, 0);
}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
