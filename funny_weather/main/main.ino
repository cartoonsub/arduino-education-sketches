#include <Wire.h>
#include <Thinary_AHT10.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "icons.h"

AHT10Class AHT10;

#define LED_BUILTIN 2

// Пины для первой шины (Дисплей 1 + AHT10)
#define SDA_1 D2
#define SCL_1 D1

// Пины для второй шины (Дисплей 2)
#define SDA_2 D6
#define SCL_2 D5

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

  // Инициализация первой шины (Дисплей 1 + AHT10)
  Wire.begin(SDA_1, SCL_1);
  if (AHT10.begin(eAHT10Address_Low)) {
    Serial.println("Init AHT10 Success.");
  } else {
    Serial.println("Init AHT10 Failure.");
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Display 1 failed"));
  }
  display.clearDisplay();
  display.display();

  // Инициализация второй шины (Дисплей 2)
  Wire.begin(SDA_2, SCL_2);
  if (!display2.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Display 2 failed"));
  }
  display2.clearDisplay();
  display2.display();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  // Убедимся, что по умолчанию шина 1 активна
  Wire.begin(SDA_1, SCL_1);
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Опрос датчика раз в секунду
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Переключаемся на первую шину для датчика
    Wire.begin(SDA_1, SCL_1); 
    currentTemp = AHT10.GetTemperature();
    currentHum = AHT10.GetHumidity();

    Serial.print("T: "); Serial.print(currentTemp, 1);
    Serial.print("C | H: "); Serial.print(currentHum, 1);
    Serial.println("%");

    showSmile = (currentTemp > 20.0);

    // Обновляем второй дисплей сразу
    updateDisplay2(currentTemp, currentHum);
    
    // Если мы в режиме погоды на 1-м дисплее, он тоже должен перерисоваться
    if (currentMode == SHOW_WEATHER) {
      forceRedraw = true; 
    }
  }

  // 2. Смена режимов первого дисплея раз в 3 секунды
  if (currentMillis - previousDisplayMillis >= displayModeInterval) {
    previousDisplayMillis = currentMillis;
    currentMode = (currentMode == SHOW_WEATHER) ? SHOW_CAT : SHOW_WEATHER;
    forceRedraw = true; // Перерисовываем экран сразу после смены режима
  }

  // 3. Рендеринг первого дисплея (включая анимацию)
  if (currentMode == SHOW_CAT) {
    // Анимация только по таймеру ИЛИ при принудительной перерисовке (смена режима)
    if ((currentMillis - previousAnimMillis >= animInterval) || forceRedraw) {
      previousAnimMillis = currentMillis;
      currentFrame = (currentFrame + 1) % 4;
      updateCatDisplay();
      forceRedraw = false; // Сбрасываем флаг только после отрисовки
    }
  } else { // Режим SHOW_WEATHER
    if (forceRedraw) {
      updateWeatherDisplay(currentTemp, currentHum);
      forceRedraw = false;
    }
  }
}

// --- Функции отрисовки (для первого дисплея) ---
void updateWeatherDisplay(float temp, float hum) {
  // Переключаемся на первую шину перед отрисовкой
  Wire.begin(SDA_1, SCL_1); 

  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setCursor(0, 4);
  display.setTextSize(1);
  display.print("TEMP: ");
  display.setTextSize(2);
  display.print(temp, 1);
  display.setTextSize(1);
  display.print(" C");

  display.setCursor(0, 36);
  display.setTextSize(1);
  display.print("HUM:  ");
  display.setTextSize(2);
  display.print(hum, 1);
  display.setTextSize(1);
  display.print(" %");

  display.display();
}

void updateCatDisplay() {
  // Переключаемся на первую шину перед отрисовкой анимации
  Wire.begin(SDA_1, SCL_1); 

  display.clearDisplay();

  if (showSmile) {
    if (currentFrame == 0)
      display.drawBitmap(0, 0, cat_happy_bmp, 128, 64, WHITE);
    else if (currentFrame == 1 || currentFrame == 3)
      display.drawBitmap(0, 0, cat_happy_blink_bmp, 128, 64, WHITE);
    else if (currentFrame == 2)
      display.drawBitmap(0, 0, cat_happy_closed_bmp, 128, 64, WHITE);
  } else {
    display.drawBitmap(0, 0, cat_sad_bmp, 128, 64, WHITE);
  }

  display.display();
}

// --- Функция отрисовки (для второго дисплея) ---
void updateDisplay2(float temp, float hum) {
  // Переключаемся на вторую шину перед отрисовкой
  Wire.begin(SDA_2, SCL_2); 
  
  display2.clearDisplay();
  display2.setTextColor(WHITE);
  display2.setTextSize(2);
  display2.setCursor(0, 10);
  display2.print(temp, 1);
  display2.print(" C");
  display2.setCursor(0, 36);
  display2.print(hum, 1);
  display2.print(" %");
  
  display2.display(); 
}