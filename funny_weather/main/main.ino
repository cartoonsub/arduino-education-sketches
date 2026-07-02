#include <Wire.h>
#include <Thinary_AHT10.h>
#include <U8g2lib.h>
#include "icons.h"

AHT10Class AHT10;

#define LED_BUILTIN 2

// === ИНИЦИАЛИЗАЦИЯ ДИСПЛЕЕВ (U8g2) ===
// Дисплей 1 на первой шине (SCL = D1, SDA = D2)
U8G2_SSD1306_128X64_NONAME_F_SW_I2C display1(U8G2_R0, /* clock=*/ D1, /* data=*/ D2, /* reset=*/ U8X8_PIN_NONE);

// Дисплей 2 на второй шине (SCL = D3, SDA = D4)
U8G2_SSD1306_128X64_NONAME_F_SW_I2C display2(U8G2_R0, /* clock=*/ D3, /* data=*/ D4, /* reset=*/ U8X8_PIN_NONE);

// Таймеры
unsigned long previousMillis = 0;
const long interval = 1000;

unsigned long previousDisplayMillis = 0;
const long displayModeInterval = 3000;

enum DisplayMode { SHOW_CAT, SHOW_WEATHER };
DisplayMode currentMode = SHOW_WEATHER;
bool forceRedraw = true;

float currentTemp = 0.0;
float currentHum = 0.0;
bool showSmile = true;

unsigned long previousAnimMillis = 0;
const long animInterval = 150;
int currentFrame = 0;

void setup() {
  Serial.begin(115200);

  // 1. Инициализация аппаратной шины для датчика AHT10
  Wire.begin(D2, D1); // SDA = D2, SCL = D1
  if (AHT10.begin(eAHT10Address_Low)) {
    Serial.println("Init AHT10 Success.");
  } else {
    Serial.println("Init AHT10 Failure.");
  }

  // 2. Инициализация дисплеев
  display1.begin();
  display2.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.println("Setup completed!");
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Опрос датчика раз в секунду
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    currentTemp = AHT10.GetTemperature();
    currentHum = AHT10.GetHumidity();

    Serial.print("T: "); Serial.print(currentTemp, 1);
    Serial.print("C | H: "); Serial.print(currentHum, 1);
    Serial.println("%");

    showSmile = (currentTemp > 20.0);

    // Обновляем второй дисплей
    updateDisplay2(currentTemp, currentHum);
    
    if (currentMode == SHOW_WEATHER) {
      forceRedraw = true; 
    }
  }

  // 2. Смена режимов первого дисплея раз в 3 секунды
  if (currentMillis - previousDisplayMillis >= displayModeInterval) {
    previousDisplayMillis = currentMillis;
    currentMode = (currentMode == SHOW_WEATHER) ? SHOW_CAT : SHOW_WEATHER;
    forceRedraw = true;
  }

  // 3. Рендеринг первого дисплея
  if (currentMode == SHOW_CAT) {
    if ((currentMillis - previousAnimMillis >= animInterval) || forceRedraw) {
      previousAnimMillis = currentMillis;
      currentFrame = (currentFrame + 1) % 4;
      updateCatDisplay();
      forceRedraw = false;
    }
  } else { 
    if (forceRedraw) {
      updateWeatherDisplay(currentTemp, currentHum);
      forceRedraw = false;
    }
  }
}

// Первый дисплей — погода
void updateWeatherDisplay(float temp, float hum) {
  display1.clearBuffer();
  
  // Верхняя строка (Температура)
  display1.setFont(u8g2_font_ncenB08_tr); // Мелкий шрифт
  display1.setCursor(0, 20); // Y = 20 (базовая линия)
  display1.print("TEMP: ");
  
  display1.setFont(u8g2_font_ncenB14_tr); // Крупный шрифт
  display1.print(temp, 1);
  
  display1.setFont(u8g2_font_ncenB08_tr);
  display1.print(" C");

  // Нижняя строка (Влажность)
  display1.setCursor(0, 55); // Y = 55 (базовая линия)
  display1.print("HUM:  ");
  
  display1.setFont(u8g2_font_ncenB14_tr);
  display1.print(hum, 1);
  
  display1.setFont(u8g2_font_ncenB08_tr);
  display1.print(" %");
  
  display1.sendBuffer();
}

// Первый дисплей — кот с анимацией
void updateCatDisplay() {
  display1.clearBuffer();
  
  if (showSmile) {
    if (currentFrame == 0) display1.drawXBMP(0, 0, 128, 64, cat_happy_bmp);
    else if (currentFrame == 1 || currentFrame == 3) display1.drawXBMP(0, 0, 128, 64, cat_happy_blink_bmp);
    else if (currentFrame == 2) display1.drawXBMP(0, 0, 128, 64, cat_happy_closed_bmp);
  } else {
    display1.drawXBMP(0, 0, 128, 64, cat_sad_bmp);
  }
  
  display1.sendBuffer();
}

// Второй дисплей
void updateDisplay2(float temp, float hum) {
  display2.clearBuffer();
  
  display2.setFont(u8g2_font_ncenB14_tr); // Крупный шрифт для второго экрана
  
  display2.setCursor(0, 25);
  display2.print(temp, 1);
  display2.print(" C");
  
  display2.setCursor(0, 55);
  display2.print(hum, 1);
  display2.print(" %");
  
  display2.sendBuffer(); 
}