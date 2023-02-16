#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h> // DO NOT CHANGE INCLUDE ORDER!!!
#include <AccelStepper.h>
#include <HX711_ADC.h>

#define LCD_DC 9
#define LCD_CS 10
#define POWER_LED 40
#define TRIGGER_LED 41
#define DRIVER_ENABLE 42
#define DRIVER_DIRECTION 43
#define DRIVER_PULSE 44 // PWM
#define HX711_DOUT 45
#define HX711_SCK 46
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
long duration = 0;
int sub_page = 0;
float calibrationValue = 25695.96; // internal pressure tare value
unsigned long stabilizingtime = 2000;
boolean _tare = true;

Adafruit_FT6206 ts = Adafruit_FT6206();
Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_DC);
AccelStepper stepper = AccelStepper(1, DRIVER_PULSE, DRIVER_DIRECTION);
HX711_ADC LoadCell(HX711_DOUT, HX711_SCK);

void setup() {
    pinMode(POWER_LED, OUTPUT); // power LED
    pinMode(A10, INPUT); // pressure sensor intake
    pinMode(TRIGGER_LED, OUTPUT); // trigger LED
    pinMode(DRIVER_DIRECTION, OUTPUT);
    pinMode(DRIVER_PULSE, OUTPUT);
    pinMode(DRIVER_ENABLE, OUTPUT);
    stepper.setMaxSpeed(1500);
    Serial.begin(19200);
    tft.begin();
    LoadCell.begin();
    LoadCell.start(stabilizingtime, _tare);
    LoadCell.setCalFactor(calibrationValue);
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
        //Serial.print("CurrentPage = "); Serial.println(CurrentPage);
        if (CurrentPage == 0) {
            if (x>= 250 && x<= 300 && y>= 180 && y<= 220) { // button 4
                CurrentPage = 4;
                drawRunPage();
                delay(500);
            } else if (x>= 30 && x<= 230 && y>= 180 && y<= 220) { // button 3
                CurrentPage = 3;
                drawKeypad("Pressure = ", number_pressure);
                delay(500);
            } else if (x>= 30 && x<= 230 && y>= 130 && y<= 170) { // button 2.2
                CurrentPage = 2.2;
                drawKeypad_ratio("ON  : OFF   TIME", number_on, number_off, duration);
                delay(500);
            } else if (x>= 30 && x<= 230 && y>= 80 && y<= 120) { // button 1
                CurrentPage = 1;
                drawKeypad("Cycle = ", number_cycle);
                delay(500);
            }
        } else if (CurrentPage == 3) {
            number_pressure = changeNumber(number_pressure, x, y);
        } else if (CurrentPage == 2.2) {
            if (sub_page == 0) {
                if (x >= 0 && x<= 100 && y >= 30 && y <=60) {
                    tft.setTextSize(3);
                    tft.setTextColor(BLACK);
                    tft.fillRect(0, 30, 300, 30, LIGHTGREY);
                    tft.drawFastVLine(100, 30, 30, BLACK);
                    tft.drawFastVLine(200, 30, 30, BLACK);
                    tft.setCursor(0, 30);
                    tft.setTextColor(RED);
                    tft.println(number_on);
                    tft.setTextColor(BLACK);
                    tft.setCursor(100, 30);
                    tft.println(number_off);
                    tft.setCursor(200, 30);
                    tft.println(duration);
                    sub_page = 1;
                    //Serial.print("page0 -> "); Serial.println(page);
                } else if (x > 100 && x <= 200 && y >= 30 && y <=60) {
                    tft.setTextSize(3);
                    tft.setTextColor(BLACK);
                    tft.fillRect(0, 30, 300, 30, LIGHTGREY);
                    tft.drawFastVLine(100, 30, 30, BLACK);
                    tft.drawFastVLine(200, 30, 30, BLACK);
                    tft.setCursor(0, 30);
                    tft.println(number_on);
                    tft.setCursor(100, 30);
                    tft.setTextColor(RED);
                    tft.println(number_off);
                    tft.setTextColor(BLACK);
                    tft.setCursor(200, 30);
                    tft.println(duration);
                    sub_page = 2;
                    //Serial.print("page0 -> "); Serial.println(page);
                } else if (x > 200 && x <= 300 && y >= 30 && y <=60) {
                    tft.setTextSize(3);
                    tft.setTextColor(BLACK);
                    tft.fillRect(0, 30, 300, 30, LIGHTGREY);
                    tft.drawFastVLine(100, 30, 30, BLACK);
                    tft.drawFastVLine(200, 30, 30, BLACK);
                    tft.setCursor(0, 30);
                    tft.println(number_on);
                    tft.setCursor(100, 30);
                    tft.println(number_off);
                    tft.setCursor(200, 30);
                    tft.setTextColor(RED);
                    tft.println(duration);
                    tft.setTextColor(BLACK);
                    sub_page = 3;
                    //Serial.print("page0 -> "); Serial.println(page);
                } else if (x>= 200 && x< 300 && y>= 195 && y< 240) {
                    CurrentPage = 0;
                    drawHome();
                }
            } else if (sub_page == 1) {
            number_on = changeNumber_ratio(number_on, x, y, 0);
            //Serial.print("page1 -> "); Serial.println(page);
            } else if (sub_page == 2) {
            number_off = changeNumber_ratio(number_off, x, y, 100);
            //Serial.print("page0 -> "); Serial.println(page);
            } else if (sub_page == 3) {
            duration = changeNumber_ratio(duration, x, y, 200);
            //Serial.print("page0 -> "); Serial.println(page);
            }
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

    tft.fillRoundRect(30, 130, 200, 40, 8, RED);
    tft.drawRoundRect(30, 130, 200, 40, 8, WHITE);   // button 2    

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
    tft.print("Set ON/OFF ratio"); // button 2.2

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

void drawKeypad_ratio(String prompt, long number1, long number2, long number3) {
    tft.fillRect(0, 0, 300, 30, GREEN);
    tft.fillRect(0, 30, 300, 30, LIGHTGREY);
    tft.drawFastVLine(100, 30, 30, BLACK);
    tft.drawFastVLine(200, 30, 30, BLACK);
    tft.setCursor(0, 0);
    tft.setTextSize(3);
    tft.setTextColor(RED);
    tft.println(prompt);
    tft.setTextColor(BLACK);
    tft.setCursor(0, 30);
    tft.println(number1);
    tft.setCursor(100, 30);
    tft.println(number2);
    tft.setCursor(200, 30);
    tft.println(number3);
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

long changeNumber_ratio(long number, int x, int y, int x_cord) {
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
        tft.fillRect(0, 30, 300, 30, LIGHTGREY);
        tft.drawFastVLine(100, 30, 30, BLACK);
        tft.drawFastVLine(200, 30, 30, BLACK);
        if (x_cord == 0) {
          tft.setCursor(0, 0);
          tft.setTextSize(3);
          tft.setTextColor(RED);
          tft.setCursor(0, 30);
          tft.println(number);
          tft.setTextColor(BLACK);
          tft.setCursor(100, 30);
          tft.println(number_off);
          tft.setCursor(200, 30);
          tft.println(duration);
        } else if (x_cord == 100) {
          tft.setCursor(0, 0);
          tft.setTextSize(3);
          tft.setTextColor(BLACK);
          tft.setCursor(0, 30);
          tft.println(number_on);
          tft.setTextColor(RED);
          tft.setCursor(100, 30);
          tft.println(number);
          tft.setTextColor(BLACK);
          tft.setCursor(200, 30);
          tft.println(duration);
        } else if (x_cord == 200) {
          tft.setCursor(0, 0);
          tft.setTextSize(3);
          tft.setTextColor(BLACK);
          tft.setCursor(0, 30);
          tft.println(number_on);
          tft.setCursor(100, 30);
          tft.println(number_off);
          tft.setTextColor(RED);
          tft.setCursor(200, 30);
          tft.println(number);
        }
    } else if (x>= 100 && x< 200 && y>= 195 && y< 240) {
        number = (number * 10) + 0;
    } 
    //Serial.print("number = "); Serial.println(number);
    tft.fillRect(x_cord, 30, 100, 30, LIGHTGREY);
    tft.setCursor(x_cord, 30);
    tft.setTextSize(3);
    tft.setTextColor(RED);
    tft.println(number);
    if (x>= 200 && x< 300 && y>= 195 && y< 240) {
      sub_page = 0;
      tft.fillRect(0, 30, 300, 30, LIGHTGREY);
      tft.drawFastVLine(100, 30, 30, BLACK);
      tft.drawFastVLine(200, 30, 30, BLACK);
      tft.setCursor(0, 0);
      tft.setTextSize(3);
      tft.setTextColor(BLACK);
      tft.setCursor(0, 30);
      tft.println(number_on);
      tft.setCursor(100, 30);
      tft.println(number_off);
      tft.setCursor(200, 30);
      tft.println(duration);
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
    double onTime = calcOnTime(number_on, number_off, duration);
    double offTime = calcOffTime(number_on, number_off, duration);

    for (int i= 1; i<= number_cycle; i++) {
        tft.fillRect(0, 210, 320, 28, BLACK);; // clearing existing scale bar
        refreshSetPressure(number_pressure);
        refreshCurrentCycle(i);
        digitalWrite(POWER_LED, HIGH);
        refreshOnOFFText("ON");
        refreshTotalTime(onTime);
        refreshCurrentTime_char("Squz");
        refreshCurrentPressure_char("Squz");      
        digitalWrite(DRIVER_ENABLE, LOW);
        stepper.setSpeed(500);
        //Serial.print("currentpositionNew = "); Serial.println(stepper.currentPosition());
        while (getPressure() < number_pressure) {
            //Serial.print("pressure1 = "); Serial.println(getPressure());
            digitalWrite(TRIGGER_LED, HIGH);
            stepper.runSpeed();
            //Serial.print("currentpositionRunIni = "); Serial.println(stepper.currentPosition());
        }
        refreshCurrentPressure(getPressure());
        digitalWrite(TRIGGER_LED, LOW);
        long start_time = millis();
        //Serial.print("start_time = "); Serial.println(start_time);
        //Serial.print("millis() = "); Serial.println(millis());
        while ((millis() - start_time) < (onTime * 60 * 1000)) { // ON time
            refreshCurrentTime((millis() - start_time) / 1000.0 / 60.0);
            refreshCurrentPressure(getPressure());
            refreshScale(getPressure());
            delay(500);
            //Serial.println((millis() - start_time));
            //Serial.println((millis() - start_time) / 1000.0 / 60.0, 2);
            if (getPressure() < number_pressure) {
                //Serial.print("pressure2 = "); Serial.println(getPressure());
                refreshCurrentPressure_char("Squz");
                refreshCurrentTime_char("Squz"); // time paused for squeezing process
                tft.fillRect(0, 210, 320, 28, BLACK); // clearing existing scale bar
                long stop_time = millis();
                while (getPressure() < number_pressure) {
                    //Serial.print("pressure3 = "); Serial.println(getPressure());
                    digitalWrite(TRIGGER_LED, HIGH);
                    stepper.runSpeed();
                    //Serial.print("currentpositionRun = "); Serial.println(stepper.currentPosition());
                }
                digitalWrite(TRIGGER_LED, LOW);
                refreshCurrentPressure_char("Complt");
                delay(1000);
                start_time = start_time + (millis() - stop_time);
            } 
        }
        tft.fillRect(0, 210, 320, 28, BLACK); // clearing existing scale bar
        digitalWrite(POWER_LED, LOW);
        refreshOnOFFText("OFF");
        refreshTotalTime(offTime);
        digitalWrite(TRIGGER_LED, LOW);
        refreshSetPressure_char("<DIA:80");
        refreshCurrentPressure_char("Rev");
        refreshCurrentTime_char("Rev");
        stepper.setSpeed(-500);
        while (getPressure() > 80) {
            stepper.runSpeed();
        }
        digitalWrite(DRIVER_ENABLE, HIGH);
        start_time = millis(); // refreshing time mark for OFF
        while ((millis() - start_time) < (offTime * 60 * 1000)) { // OFF time
            refreshCurrentTime((millis() - start_time) / 1000.0 / 60.0);
            //Serial.println((millis() - start_time) / 1000.0 / 60.0, 2);
            refreshCurrentPressure(getPressure());
            refreshScale(getPressure());
            delay(500);
        }
    }
    CurrentPage = 0;
    digitalWrite(DRIVER_ENABLE, LOW);
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

void refreshCurrentTime_char(String value) {
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.fillRect(0, 120, 150, 30, YELLOW);
    tft.setCursor(0, 120);
    tft.print(value);
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

void refreshTotalTime(double value) {
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.fillRect(150, 120, 150, 30, RED);
    tft.setCursor(150, 120);
    tft.print(value, 1);
}

void refreshCurrentPressure(int value) {
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.fillRect(0, 180, 150, 30, YELLOW);
    tft.setCursor(0, 180);
    tft.print(value);
}

void refreshCurrentPressure_char(String value) {
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.fillRect(0, 180, 150, 30, YELLOW);
    tft.setCursor(0, 180);
    tft.print(value);
}

void refreshSetPressure(long value) {
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.fillRect(150, 180, 150, 30, RED);
    tft.setCursor(150, 180);
    tft.print(value);
}

void refreshSetPressure_char(String value) {
    tft.setTextSize(3);
    tft.setTextColor(BLACK);
    tft.fillRect(150, 180, 150, 30, RED);
    tft.setCursor(150, 180);
    tft.print(value);
}

 int getPressure() {
    LoadCell.update();
    float i = LoadCell.getData();
    if (i> 10 && i<= 20) {
        i = i - 4;
    } else if (i> 20 && i <= 30) {
        i = i - 6;
    } else if (i> 30 && i <= 40) {
        i = i - 6;
    } else if (i> 40 && i <= 50) {
        i = i - 7;
    } else if (i> 50 && i <= 60) {
        i = i - 7;
    } else if (i> 60 && i <= 70) {
        i = i - 7;
    } else if (i> 70 && i <= 80) {
        i = i - 7;
    } else if (i> 80 && i <= 90) {
        i = i - 6;
    } else if (i> 90 && i <= 100) {
        i = i - 6;
    } else if (i> 100 && i <= 110) {
        i = i - 5;
    } else if (i> 110 && i <= 120) {
        i = i - 3;
    } else if (i> 120 && i <= 130) {
        i = i - 3;
    } else if (i> 130 && i <= 140) {
        i = i - 4;
    } else if (i> 140 && i <= 150) {
        i = i - 3;
    } else if (i> 150 && i <= 160) {
        i = i - 2;
    } else if (i> 160 && i <= 170) {
        i = i - 2;
    } else if (i> 170 && i <= 180) {
        i = i - 3;
    } else if (i> 180 && i <= 190) {
        i = i - 2;
    } else if (i> 190 && i <= 200) {
        i = i - 2;
    }
    //Serial.print("pressure = "); Serial.println(i);
    return i;
 }

 void refreshScale(int number) {
    tft.fillRect(0, 218, 300, 20, BLACK); // remove exist
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.setCursor(0, 210);
    tft.print("0");
    tft.setCursor(75, 210);
    tft.print("50");
    tft.setCursor(150, 210);
    tft.print("100");
    tft.setCursor(225, 210);
    tft.print("150");
    tft.setCursor(300, 210);
    tft.print("200");
    tft.drawRect(0, 218, 300, 20, BLUE);
    tft.drawFastVLine(75, 218, 12, BLUE);
    tft.drawFastVLine(150, 218, 12, BLUE);
    tft.drawFastVLine(225, 218, 12, BLUE);
    //Serial.print("number = "); Serial.println(number);
    int draw = map(number, 0, 200, 0, 300);
    //Serial.print("draw = "); Serial.println(draw);
    tft.fillRect(0, 218, draw, 20, PINK);
    tft.drawFastVLine(75, 218, 12, BLUE);
    tft.drawFastVLine(150, 218, 12, BLUE);
    tft.drawFastVLine(225, 218, 12, BLUE);
}

 double calcOnTime(long number1, long number2, long number3) {
    double number11 = number1;
    double number22 = number2;
    //Serial.print("number11 = "); Serial.println(number11);
    //Serial.print("number22 = "); Serial.println(number22);
    double onTime = number11 / (number11 + number22) * number3;
    //Serial.print("onTime = "); Serial.println(onTime);
    return onTime;
 }

double calcOffTime(long number1, long number2, long number3) {
    double number11 = number1;
    double number22 = number2;
    double offTime = number22 / (number11 + number22) * number3;
    return offTime;
 }