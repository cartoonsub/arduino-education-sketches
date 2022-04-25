
#include <Wire.h>
#include <Thinary_AHT10.h>
AHT10Class AHT10;

#define relay_pin D5 // пин мосфета / реле
#define LED_BUILTIN 2

void setup()
{
    Serial.begin(9600);
    Serial.setTimeout(50);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(relay_pin, OUTPUT);        // пин реле как выход
    digitalWrite(relay_pin, 0);

    Wire.begin();
    if(AHT10.begin(eAHT10Address_Low)) {
      Serial.println("Init AHT10 Sucess.");
    } else {
      Serial.println("Init AHT10 Failure.");
    }
}

void loop()
{
    if (Serial.available() > 0)
    {
        int value = Serial.parseInt();
        delay(1000);
        if (value == 0)
        {
            digitalWrite(relay_pin, 0);
            Serial.println("Выключен");
        }

        if (value == 1)
        {
            digitalWrite(relay_pin, 1);
            Serial.println("Включен");
        }

        delay(1000);
    }

    Serial.println(String("")+"Humidity(%RH):\t\t"+AHT10.GetHumidity()+"%");
    Serial.println(String("")+"Temperature(℃):\t"+AHT10.GetTemperature()+"℃");
    Serial.println(String("")+"Dewpoint(℃):\t\t"+AHT10.GetDewPoint()+"℃");

    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(relay_pin, 0);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(relay_pin, 1);
    delay(2000);
}
