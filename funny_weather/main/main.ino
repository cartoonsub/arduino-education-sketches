#include <Wire.h>
#include <Thinary_AHT10.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "icons.h"

AHT10Class AHT10;

#define relay_pin D5 // mosfet
#define pwm_pin D6   // PWM
#define LED_BUILTIN 2

// Таймеры для логики (без delay)
unsigned long previousMillis = 0;
const long interval = 1000; // Опрос датчика раз в секунду

// Переключение экранов (State Machine)
unsigned long previousDisplayMillis = 0;
const long displayModeInterval = 3000; // Менять экран каждые 3 секунды
enum DisplayMode
{
  SHOW_CAT,
  SHOW_WEATHER
};
DisplayMode currentMode = SHOW_WEATHER;

// Переменные для данных
float currentTemp = 0.0;
float currentHum = 0.0;
bool showSmile = true;

// Настройки дисплея
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
  Serial.begin(115200);
  analogWriteFreq(2000);
  analogWriteRange(1023);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.clearDisplay();
  display.display();

  Wire.begin();
  if (AHT10.begin(eAHT10Address_Low))
  {
    Serial.println("Init AHT10 Success.");
  }
  else
  {
    Serial.println("Init AHT10 Failure.");
  }

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  pinMode(pwm_pin, OUTPUT);
  digitalWrite(relay_pin, LOW);
  digitalWrite(LED_BUILTIN, HIGH);

  analogWrite(pwm_pin, 102); // 10% PWM
}

void loop()
{
  unsigned long currentMillis = millis();

  // 1. Неблокирующий опрос датчика (раз в 1 секунду)
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    currentTemp = AHT10.GetTemperature();
    currentHum = AHT10.GetHumidity();

    // Дублируем в Serial
    Serial.print("T: ");
    Serial.print(currentTemp, 1);
    Serial.print("C | ");
    Serial.print("H: ");
    Serial.print(currentHum, 1);
    Serial.println("%");

    if (currentTemp > 20.0)
    {
      showSmile = true;
    } else {
      showSmile = false;
    }
  }

  // 2. Неблокирующий автомат для дисплея (смена режимов раз в 3 секунды)
  if (currentMillis - previousDisplayMillis >= displayModeInterval)
  {
    previousDisplayMillis = currentMillis;

    if (currentMode == SHOW_WEATHER)
    {
      currentMode = SHOW_CAT;
    }
    else
    {
      currentMode = SHOW_WEATHER;
    }
  }

  // 3. Динамический рендеринг экрана в зависимости от текущего режима
  if (currentMode == SHOW_WEATHER)
  {
    updateWeatherDisplay(currentTemp, currentHum);
  }
  else
  {
    updateCatDisplay();
  }
}

// Отрисовка погоды (без delay, с жесткими координатами)
void updateWeatherDisplay(float temp, float hum)
{
  display.clearDisplay();
  display.setTextColor(WHITE);

  // Строка температуры
  display.setCursor(0, 4);
  display.setTextSize(1);
  display.print("TEMP: ");

  display.setTextSize(2);
  display.print(temp, 1);
  display.setTextSize(1);
  display.print(" C");

  // Строка влажности
  display.setCursor(0, 36);
  display.setTextSize(1);
  display.print("HUM:  ");

  display.setTextSize(2);
  display.print(hum, 1);
  display.setTextSize(1);
  display.print(" %");

  display.display();
}

// Отрисовка кота (без delay)
void updateCatDisplay()
{
  display.clearDisplay();
  if (showSmile)
  {
    display.drawBitmap(0, 0, cat_happy_bmp, 128, 64, WHITE);
  }
  else
  {
    display.drawBitmap(0, 0, cat_sad_bmp, 128, 64, WHITE);
  }
  display.display();
}