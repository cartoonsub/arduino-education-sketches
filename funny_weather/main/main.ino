#include <Wire.h>
#include <Thinary_AHT10.h>
#include <U8g2lib.h>
#include "icons.h"

AHT10Class AHT10;

#define LED_BUILTIN 2

#define BUTTON_1 D0
#define BUTTON_2 D5
#define BUTTON_3 D6
#define BUTTON_4 D7

bool button1Pressed = false;
bool button2Pressed = false;
bool button3Pressed = false;
bool button4Pressed = false;

// === ИНИЦИАЛИЗАЦИЯ ДИСПЛЕЕВ (U8g2) ===
// Дисплей 1 (ПОГОДА, 128x32) на первой шине (SCL = D1, SDA = D2)
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C display1(U8G2_R0, /* clock=*/D1, /* data=*/D2, /* reset=*/U8X8_PIN_NONE);

// Дисплей 2 (КОТ, 128x64) на второй шине (SCL = D3, SDA = D4)
U8G2_SSD1306_128X64_NONAME_F_SW_I2C display2(U8G2_R0, /* clock=*/D3, /* data=*/D4, /* reset=*/U8X8_PIN_NONE);

// Таймер для датчика и погоды
unsigned long previousMillis = 0;
const long interval = 1000;

// Таймер для анимации кота
unsigned long previousAnimMillis = 0;
const long animInterval = 150;
int currentFrame = 0;

float currentTemp = 0.0;
float currentHum = 0.0;
bool showSmile = true;

void setup()
{
  Serial.begin(115200);

  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_4, INPUT);

  // 1. Инициализация аппаратной шины для датчика AHT10
  Wire.begin(D2, D1); // SDA = D2, SCL = D1
  if (AHT10.begin(eAHT10Address_Low))
  {
    Serial.println("Init AHT10 Success.");
  }
  else
  {
    Serial.println("Init AHT10 Failure.");
  }

  // 2. Инициализация дисплеев
  display1.begin();
  display2.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("Setup completed! Starting loops...");
}

void loop()
{
  unsigned long currentMillis = millis();

  bool button1State = digitalRead(BUTTON_1);
  if (button1State == LOW) {
    button1Pressed = !button1Pressed; // Toggle the state
    delay(200); // Debounce delay
  }

  // === 1. Опрос датчика и обновление Дисплея 1 (Раз в секунду) ===
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    currentTemp = AHT10.GetTemperature();
    currentHum = AHT10.GetHumidity();

    Serial.print("T: ");
    Serial.print(currentTemp, 1);
    Serial.print("C | H: ");
    Serial.print(currentHum, 1);
    Serial.println("%");

    showSmile = (currentTemp > 20.0);

    // Отрисовываем погоду
    updateWeatherDisplay(currentTemp, currentHum, button1Pressed);
  }

  // === 2. Анимация Дисплея 2 (Каждые 150 мс) ===
  if (currentMillis - previousAnimMillis >= animInterval)
  {
    previousAnimMillis = currentMillis;
    currentFrame = (currentFrame + 1) % 4;

    // Отрисовываем котика
    updateCatDisplay();
  }
}

// --- ФУНКЦИЯ ДЛЯ ДИСПЛЕЯ 1 (ПОГОДА 128x32) ---
void updateWeatherDisplay(float temp, float hum, bool button1Pressed)
{
  if (button1Pressed) {
      showTempOnlyOnDisplay(temp);
  } else {
    showFullInfoOnDisplay(temp, hum);
  }
}

void showFullInfoOnDisplay(float temp, float hum)
{
  display1.clearBuffer();

  // Шрифт среднего размера (~10 пикселей в высоту). Идеально для 2 строк.
  display1.setFont(u8g2_font_ncenB10_tr);

  // Строка 1: Температура (Y = 14)
  display1.setCursor(0, 14);
  display1.print("T: ");
  display1.print(temp, 1);
  display1.print(" C");

  // Строка 2: Влажность (Y = 32)
  display1.setCursor(0, 32);
  display1.print("H: ");
  display1.print(hum, 1);
  display1.print(" %");

  display1.sendBuffer();
}

void showTempOnlyOnDisplay(float temp)
{
  display1.clearBuffer();

  // Шрифт большого размера (~20 пикселей в высоту). Идеально для одной строки.
  display1.setFont(u8g2_font_ncenB18_tr);

  // Строка 1: Температура (Y = 24)
  display1.setCursor(0, 24);
  display1.print("T: ");
  display1.print(temp, 1);
  display1.print(" C");

  display1.sendBuffer();
}

// --- ФУНКЦИЯ ДЛЯ ДИСПЛЕЯ 2 (КОТИК 128x64) ---
void updateCatDisplay()
{
  display2.clearBuffer();

  // todo - костыль - решить проблему с инверсией XBM-массивов. Временное решение: инвертируем цвета при отрисовке.
  // Магия для инвертированных XBM-массивов:
  display2.setBitmapMode(0); // Включаем "сплошной" режим отрисовки
  display2.setDrawColor(0);  // Инвертируем цвета: единицы (фон) тушим, нули (кота) зажигаем

  if (showSmile)
  {
    if (currentFrame == 0)
      display2.drawXBMP(0, 0, 128, 64, cat_happy_bits);
    else if (currentFrame == 1 || currentFrame == 3)
      display2.drawXBMP(0, 0, 128, 64, cat_happy_blink_bits);
    else if (currentFrame == 2)
      display2.drawXBMP(0, 0, 128, 64, cat_happy_closed_bits);
  }
  else
  {
    display2.drawXBMP(0, 0, 128, 64, cat_sad_bits);
  }

  // Обязательно возвращаем настройки в норму для остального текста/графики, если они будут
  display2.setDrawColor(1);
  display2.setBitmapMode(1);

  display2.sendBuffer();
}