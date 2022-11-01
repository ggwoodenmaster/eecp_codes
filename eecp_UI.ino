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

long number_pressure = 0;
long number_on = 0;
long number_off = 0;
long number_cycle = 0;
double CurrentPage = 0;

Adafruit_FT6206 ts = Adafruit_FT6206();
Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_DC);

void setup() {
    pinMode(40, OUTPUT); // power LED
    pinMode(A10, INPUT); // pressure sensor intake
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
        drawHome();
}

void loop() {
    while (ts.touched()) {
        TS_Point p = ts.getPoint();
        delay(500);
        int y = p.x;
        p.y = map(p.y, 0, 320, 320, 0);
        int x = p.y;
        //Serial.print("Current x = "); Serial.println(x);
        //Serial.print("Current y = "); Serial.println(y);
        //Serial.print("Current z = "); Serial.println(p.z);
        Serial.print("CurrentPage = "); Serial.println(CurrentPage);
        if (CurrentPage == 0) {
            if (x>= 250 && x<= 300 && y>= 180 && y<= 220) { // button 4
                CurrentPage = 4;
                drawRunPage();
                delay(500);
            } else if (x>= 30 && x<= 230 && y>= 180 && y<= 220) { // button 3
                CurrentPage = 3;
                drawKeypad("Pressure = ", number_pressure);
                delay(500);
            } else if (x>= 30 && x<= 130 && y>= 130 && y<= 170) { // button 2.2
                CurrentPage = 2.2;
                drawKeypad("ON = ", number_on);
                delay(500);
            } else if (x>= 130 && x<= 230 && y>= 130 && y<= 170) { // button 2.1
                CurrentPage = 2.1;
                drawKeypad("OFF = ", number_off);
                delay(500);
            } else if (x>= 30 && x<= 230 && y>= 80 && y<= 120) { // button 1
                CurrentPage = 1;
                drawKeypad("Cycle = ", number_cycle);
                delay(500);
            }
        } else if (CurrentPage == 3) {
            number_pressure = changeNumber(number_pressure, x, y);
        } else if (CurrentPage == 2.2) {
            number_on = changeNumber(number_on, x, y);
        } else if (CurrentPage == 2.1) {
            number_off = changeNumber(number_off, x, y);
        } else if (CurrentPage == 1) {
            number_cycle = changeNumber(number_cycle, x, y);
        }
    }
}

void drawHome() {
    tft.fillScreen(BLACK);
    tft.drawRoundRect(0, 0, 319, 240, 8, WHITE);    // Page border

    tft.fillRoundRect(250, 180, 50, 40, 8, GREEN);  // button 4
    tft.drawRoundRect(250, 180, 50, 40, 8, WHITE);

    tft.fillRoundRect(30, 180, 200, 40, 8, RED);
    tft.drawRoundRect(30, 180, 200, 40, 8, WHITE);  // button 3

    tft.fillRoundRect(30, 130, 100, 40, 8, RED);    
    tft.drawRoundRect(30, 130, 100, 40, 8, WHITE);  // button 2.2

    tft.fillRoundRect(130, 130, 100, 40, 8, RED);    
    tft.drawRoundRect(130, 130, 100, 40, 8, WHITE);  // button 2.1

    tft.fillRoundRect(30, 80, 200, 40, 8, RED);
    tft.drawRoundRect(30, 80, 200, 40, 8, WHITE);   // button 1

    tft.setCursor(30, 20);
    tft.setTextSize(2);
    tft.setFont();
    tft.setTextColor(WHITE);
    tft.print("EECP Internal V1.0");
    tft.setCursor(40, 50);
    tft.setTextSize(2);
    tft.setTextColor(CYAN);
    tft.print("EG4301 Project");
    
    tft.setTextColor(BLACK);
    
    tft.setCursor(255, 195);
    tft.print("Run"); // button 4
    
    tft.setCursor(35, 195);
    tft.print("Set pressure"); // button 3

    tft.setCursor(35, 145);
    tft.print("Set ON"); // button 2.2

    tft.setCursor(135, 145);
    tft.print("Set OFF"); // button 2.1

    tft.setCursor(35, 95);
    tft.print("Set cycle"); // button 1
}

void drawKeypad(String prompt, long number) {
    tft.fillRect(0, 0, 300, 30, GREEN);
    tft.fillRect(0, 30, 300, 30, LIGHTGREY);
    tft.setCursor(0, 0);
    tft.setTextSize(3);
    tft.setTextColor(RED);
    tft.println(prompt);
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

long changeNumber(long number, int x, int y) {
    if (x>= 0 && x< 100 && y>= 60 && y < 105) {
        number = (number * 10) + 1;
    } else if (x>= 100 && x< 200 && y>= 60 && y< 105) {
        number = (number * 10) + 2;
    } else if (x>= 200 && x< 300 && y>= 60 && y< 105) {
        number = (number * 10) + 3;
    } else if (x>= 0 && x< 100 && y>= 105 && y< 150) {
        number = (number * 10) + 4;
    } else if (x>= 100 && x< 200 && y>= 105 && y< 150) {
        number = (number * 10) + 5;
    } else if (x>= 200 && x< 300 && y>= 105 && y< 150) {
        number = (number * 10) + 6;
    } else if (x>= 0 && x< 100 && y>= 150 && y< 195) {
        number = (number * 10) + 7;
    } else if (x>= 100 && x< 200 && y>= 150 && y< 195) {
        number = (number * 10) + 8;
    } else if (x>= 200 && x< 300 && y>= 150 && y< 195) {
        number = (number * 10) + 9;
    } else if (x>= 0 && x< 100 && y>= 195 && y< 240) {
        number = 0;
    } else if (x>= 100 && x< 200 && y>= 195 && y< 240) {
        number = (number * 10) + 0;
    } 
    tft.fillRect(0, 30, 300, 30, LIGHTGREY);
    tft.setCursor(0, 30);
    tft.setTextSize(3);
    tft.setTextColor(RED);
    tft.println(number);
    if (x>= 200 && x< 300 && y>= 195 && y< 240) {
        CurrentPage = 0;
        drawHome();
    }
    return number;
}

void drawRunPage() {
    tft.fillScreen(BLACK);
    tft.fillRect(0, 0, 300, 30, GREEN); // operating bar
    tft.fillRect(0, 30, 300, 30, LIGHTGREY); // Cycle text
    tft.fillRect(0, 60, 150, 30, YELLOW); // current cycle
    tft.fillRect(150, 60, 150, 30, RED); // total cycle
    tft.fillRect(0, 90, 300, 30, LIGHTGREY); // ON/OFF text
    tft.fillRect(0, 120, 150, 30, YELLOW); // current time
    tft.fillRect(150, 120, 150, 30, RED); // total time
    tft.fillRect(0, 150, 300, 30, LIGHTGREY); // Pressure text
    tft.fillRect(0, 180, 150, 30, YELLOW); // current pressure
    tft.fillRect(150, 180, 150, 30, RED); // aim pressure
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.setCursor(0, 0);
    tft.print("Operating...");
    tft.setCursor(0, 30);
    tft.print("Cycle");
    tft.setCursor(0, 60);
    tft.print("CC"); // current cycle
    tft.setCursor(150, 60);
    tft.print(number_cycle); // total cycle
    tft.setCursor(0, 90);
    tft.print("Status - ");
    tft.setCursor(0, 150);
    tft.print("Pressure");
    tft.setCursor(150, 180);
    tft.print(number_pressure);

    long start_time = millis();
    //Serial.print("start_time = "); Serial.println(start_time);
    //Serial.print("millis() = "); Serial.println(millis());
    for (int i= 1; i<= number_cycle; i++) {
        refreshCurrentCycle(i);
        digitalWrite(40, HIGH);
        refreshOnOFFText("ON");
        refreshTotalTime(number_on);
        while ((millis() - start_time) < (number_on * 60 * 1000)) { // ON time
             refreshCurrentTime((millis() - start_time) / 1000.0 / 60.0);
             //Serial.println((millis() - start_time));
             //Serial.println((millis() - start_time) / 1000.0 / 60.0, 2);
             refreshCurrentPressure(getPressure());
             delay(50);
        }
        start_time = millis(); // refreshing time mark for OFF
        digitalWrite(40, LOW);
        refreshOnOFFText("OFF");
        refreshTotalTime(number_off);
        while ((millis() - start_time) < (number_off * 60 * 1000)) { // OFF time
            refreshCurrentTime((millis() - start_time) / 1000.0 / 60.0);
            //Serial.println((millis() - start_time) / 1000.0 / 60.0, 2);
            delay(50);
        }
        start_time = millis(); // refreshing time mark for ON
    }
    CurrentPage = 0;
    drawHome();
}

void refreshCurrentCycle(long value) {
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.fillRect(0, 60, 150, 30, YELLOW);
    tft.setCursor(0, 60);
    tft.print(value);
}

void refreshCurrentTime(float value) {
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.fillRect(0, 120, 150, 30, YELLOW);
    tft.setCursor(0, 120);
    tft.print(value, 1);
}

void refreshOnOFFText(String text) {
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.fillRect(0, 90, 300, 30, LIGHTGREY);
    tft.setCursor(0, 90);
    tft.print("Status - ");
    tft.setCursor(160, 90);
    tft.print(text);
}

void refreshTotalTime(long value) {
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.fillRect(150, 120, 150, 30, RED);
    tft.setCursor(150, 120);
    tft.print(value);
}

void refreshCurrentPressure(float value) {
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.fillRect(0, 180, 150, 30, YELLOW);
    tft.setCursor(0, 180);
    tft.print(value);
}

double getPressure() {
    int anR = analogRead(A10);
    int invanR = map(anR, 0, 1023, 1023, 0);
    double voltage = invanR / 1023.0 * 5.0;
    //Serial.print("voltage = "); Serial.println(voltage);
    if (voltage == 5.00) {
        return 0.00;
    } else {
        double resistance = (12000.0 / (5.0 - voltage)) * voltage / 1000.0;
        //Serial.print("resistance = "); Serial.println(resistance);
        double base = resistance / 336.04;
        //Serial.print("base = "); Serial.println(base);
        double exponent = -1 / 0.712;
        double force = pow(base, exponent); // in N
        if (force > 0 && force < 1500.00) {
            //Serial.print("force = "); Serial.println(force);
            double area = 3.14 * pow(2.8, 2); // in mm^2
            double pressure = force / area; // in N/mm^2
            //Serial.print("pressure = "); Serial.println(pressure); 
            return pressure;
        } else {
            //Serial.println("Error -1");
            return -1.00;
        }
        delay(500);
    }
}