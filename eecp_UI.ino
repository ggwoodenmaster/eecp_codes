#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h> // DO NOT CHANGE INCLUDE ORDER!!!

#define LCD_DC 9
#define LCD_CS 10
//#define ICSP_SCLK 13
//#define ICSP_MISO 12
//#define ICSP_MOSI 11

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

int CurrentPage;

Adafruit_FT6206 ts = Adafruit_FT6206();
Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_DC);

void setup() {
  Serial.begin(9600);
  tft.begin();
  if (!ts.begin(40)) { 
    Serial.println("Unable to start touchscreen.");
  } else { 
    Serial.println("Touchscreen started.");
  } 
    tft.setRotation(1);
    tft.fillScreen(CYAN);
    delay(500);
    drawHome();
    CurrentPage = 0;
}

void loop() {
    //Serial.println(ts.touched());
    if (ts.touched()) {
        Serial.println("Touched!");
        TS_Point p = ts.getPoint();
        int y = p.x;
        p.y = map(p.y, 0, 320, 320, 0);
        int x = p.y;
        Serial.print("Current x = "); Serial.println(x);
        Serial.print("Current y = "); Serial.println(y);
        Serial.print("Current z = "); Serial.println(p.z);
        delay(500);
            if (CurrentPage == 0) {
                if (x>= 60 && x<= 260 && y>= 180 && y<= 220) {
                    CurrentPage = 1;
                    drawPage1();
                }
            }
    }
}

void drawHome() {
    tft.fillScreen(BLACK);
    tft.drawRoundRect(0, 0, 319, 240, 8, WHITE);    //Page border

    tft.fillRoundRect(60, 180, 200, 40, 8, RED);
    tft.drawRoundRect(60, 180, 200, 40, 8, WHITE);  

    tft.fillRoundRect(60, 130, 200, 40, 8, RED);    
    tft.drawRoundRect(60, 130, 200, 40, 8, WHITE);

    tft.fillRoundRect(60, 80, 200, 40, 8, RED);
    tft.drawRoundRect(60, 80, 200, 40, 8, WHITE);  

    tft.setCursor(60, 20);
    tft.setTextSize(2);
    tft.setFont();
    tft.setTextColor(WHITE);
    tft.print("EECP Internal V1.0");
    tft.setCursor(70, 50);
    tft.setTextSize(2);
    tft.setTextColor(CYAN);
    tft.print("EG4301 Project");
    tft.setTextColor(BLACK);
    tft.setCursor(65, 195);
    tft.print("Set pressure");

    tft.setCursor(65, 145);
    tft.print("Set ON-OFF time");

    tft.setCursor(65, 95);
    tft.print("Set cycle");
}

void drawPage1() {
    tft.fillScreen(BLACK);
    tft.fillRoundRect(60, 180, 200, 40, 8, RED);
    tft.drawRoundRect(60, 180, 200, 40, 8, WHITE);
    tft.setCursor(65, 195);
    tft.print("BACK");
}