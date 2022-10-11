#include <SPI.h>
#include <Adafruit_GFX.h>    // Adafruit's core graphics library
#include <Adafruit_ILI9341.h> // Adafruit's hardware-specific library: for drawing
#include <Adafruit_FT6206.h>     //Touchscreen library: for touchscreen sensing

#define LCD_DC 9
#define LCD_CS 10
#define ICSP_SCLK 13
#define ICSP_MISO 12
#define ICSP_MOSI 11

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_DC);

void setup() {
    Serial.begin(9600);
    Serial.println("ILI9341 Test!"); 
    tft.begin();
    uint8_t x = tft.readcommand8(ILI9341_RDMODE);
    Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDMADCTL);
    Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDPIXFMT);
    Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDIMGFMT);
    Serial.print("Image Format: 0x"); Serial.println(x, HEX);
    x = tft.readcommand8(ILI9341_RDSELFDIAG);
    Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
    tft.setRotation(1);
    tft.fillScreen(CYAN);
    delay(500);
    drawHome();
}

void loop() {

}

void drawHome() {
    tft.fillScreen(BLACK);
    tft.drawRoundRect(0, 0, 319, 240, 8, WHITE);    //Page border

    tft.fillRoundRect(60, 180, 200, 40, 8, RED);
    tft.drawRoundRect(60, 180, 200, 40, 8, WHITE);  //Game

    tft.fillRoundRect(60, 130, 200, 40, 8, RED);    //RGB led
    tft.drawRoundRect(60, 130, 200, 40, 8, WHITE);

    tft.fillRoundRect(60, 80, 200, 40, 8, RED);
    tft.drawRoundRect(60, 80, 200, 40, 8, WHITE);  //Oscilloscope

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