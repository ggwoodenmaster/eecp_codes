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

#define BLACK        0x0000  /*   0,   0,   0 */
#define BLUE         0x001F  /*   0,   0, 255 */
#define RED          0xF800  /* 255,   0,   0 */
#define GREEN        0x07E0  /*   0, 255,   0 */
#define CYAN         0x07FF  /*   0, 255, 255 */
#define MAGENTA      0xF81F  /* 255,   0, 255 */
#define YELLOW       0xFFE0  /* 255, 255,   0 */
#define WHITE        0xFFFF  /* 255, 255, 255 */
#define NAVY         0x000F  /*   0,   0, 128 */
#define DARKGREEN    0x03E0  /*   0, 128,   0 */
#define DARKCYAN     0x03EF  /*   0, 128, 128 */
#define MAROON       0x7800  /* 128,   0,   0 */
#define PURPLE       0x780F  /* 128,   0, 128 */
#define OLIVE        0x7BE0  /* 128, 128,   0 */
#define LIGHTGREY    0xC618  /* 192, 192, 192 */
#define DARKGREY     0x7BEF  /* 128, 128, 128 */
#define ORANGE       0xFD20  /* 255, 165,   0 */
#define GREENYELLOW  0xAFE5  /* 173, 255,  47 */
#define PINK         0xF81F  /* 255,   0, 255 */
String symbol[4][3] = {
    {"1", "2", "3"},
    {"4", "5", "6"},
    {"7", "8", "9"},
    {"C", "0", "OK"}
};
long number = 0;
boolean enter = false;

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
    tft.fillScreen(BLACK);
    delay(500);
    drawUI();
}

void loop() {
    while (ts.touched()) {
        TS_Point p = ts.getPoint();
        delay(500);
        int y = p.x;
        p.y = map(p.y, 0, 320, 320, 0);
        int x = p.y;
        if (x>= 0 && x< 100 && y>= 60 && y < 105) {
            number = (number * 10) + 1;
        } else if (x>= 100 && x< 200 && y>= 60 && y < 105) {
            number = (number * 10) + 2;
        } else if (x>= 200 && x< 300 && y>= 60 && y < 105) {
            number = (number * 10) + 3;
        } else if (x>= 0 && x< 100 && y>= 105 && y < 150) {
            number = (number * 10) + 4;
        } else if (x>= 100 && x< 200 && y>= 105 && y < 150) {
            number = (number * 10) + 5;
        } else if (x>= 200 && x< 300 && y>= 105 && y < 150) {
            number = (number * 10) + 6;
        } else if (x>= 0 && x< 100 && y>= 150 && y < 195) {
            number = (number * 10) + 7;
        } else if (x>= 100 && x< 200 && y>= 150 && y < 195) {
            number = (number * 10) + 8;
        } else if (x>= 200 && x< 300 && y>= 150 && y < 195) {
            number = (number * 10) + 9;
        } else if (x>= 0 && x< 100 && y>= 195 && y < 240) {
            number = 0;
        } else if (x>= 100 && x< 200 && y>= 195 && y < 240) {
            number = (number * 10) + 0;
        } else if (x>= 200 && x< 300 && y>= 195 && y < 240) {
            enter = true;
        }
        tft.fillRect(0, 30, 300, 30, LIGHTGREY);
        tft.setCursor(0, 30);
        tft.setTextSize(3);
        tft.setTextColor(RED);
        tft.println(number);
    }
}

void drawUI() {
    tft.fillRect(0, 0, 300, 30, GREEN);
    tft.fillRect(0, 30, 300, 30, LIGHTGREY);
    tft.setCursor(0, 0);
    tft.setTextSize(3);
    tft.setTextColor(RED);
    tft.println("Setting para = ");
    tft.setCursor(0, 30);
    tft.println(number);
    tft.fillRect(0, 60, 300, 180, BLUE);

    tft.setTextSize(4);
    tft.setTextColor(WHITE);
    for (int j=0; j<3; j++) { // i is the first argument; j is the second argument
        for (int i=0; i<4; i++) {
        tft.setCursor(0 + 100 * j, 60 + 45 * i);
        tft.println(symbol[i][j]);
        }
    }
    for (int h=105; h<=240; h+=45) {
        tft.drawFastHLine(0, h, 300, WHITE);
    }
    for (int v=0; v<=300; v+=100) {
        tft.drawFastVLine(v, 60, 200, WHITE);
    }
}
