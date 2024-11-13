#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A1  // must be an analog pin, use "An" notation!
#define YM 6   // can be a digital pin
#define XP 7   // can be a digital pin

#define TS_MINX 140
#define TS_MINY 170
#define TS_MAXX 920
#define TS_MAXY 948

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define YELLOW  0xFFE0
#define RED     0xF800
#define GREEN   0x07E0
#define BLACK   0x0000
#define WHITE   0xFFFF 

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define MINPRESSURE 10
#define MAXPRESSURE 1000

bool alarmOn = false;
bool wifiConnected = true;
bool monitoringOn = false;
bool inSettingsMenu = false; // Flag to check if in settings menu

void setup() {
    Serial.begin(9600);

    tft.reset();
    uint16_t identifier = tft.readID();
    tft.begin(identifier);
    tft.fillScreen(BLACK);

    drawHomePage();
}

void loop() {
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
        int16_t screenX = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
        int16_t screenY = map(p.y, TS_MAXY, TS_MINY, tft.height(), 0);

        if (!inSettingsMenu) {
            // Check if settings button is pressed on the home page
            if (screenX >= 5 && screenX <= 55 && screenY >= 5 && screenY <= 25) {
                Serial.println("Settings button pressed");
                inSettingsMenu = true;  // Enter settings menu
                drawSettingsMenu();
            }
        } else {
            // We are in settings menu, check for button presses here
            // Toggle alarm in settings
            if (screenX >= 5 && screenX <= 100 && screenY >= 50 && screenY <= 70) {
                alarmOn = !alarmOn;
                drawSettingsMenu();
            }
            // Toggle monitoring in settings
            else if (screenX >= 5 && screenX <= 100 && screenY >= 90 && screenY <= 110) {
                monitoringOn = !monitoringOn;
                drawSettingsMenu();
            }
            // Toggle WiFi in settings
            else if (screenX >= 5 && screenX <= 100 && screenY >= 130 && screenY <= 150) {
                wifiConnected = !wifiConnected;
                drawSettingsMenu();
            }
            // Check if back button is pressed
            else if (screenX >= 5 && screenX <= 55 && screenY >= 5 && screenY <= 25) {
                Serial.println("Back button pressed");
                inSettingsMenu = false;  // Exit settings menu
                drawHomePage(); // Return to home page
            }
        }
    }
}

void drawHomePage() {
    tft.fillScreen(BLACK);

    // Draw settings button
    tft.fillRect(5, 5, 50, 20, YELLOW); // Rectangular button
    tft.setTextColor(BLACK);
    tft.setTextSize(1);
    tft.setCursor(8, 10);
    tft.print("Settings");

    // Display home screen information like WiFi, Alarm, and Monitoring status
    tft.setTextColor(WHITE);
    tft.setCursor(10, 50);
    tft.print("Home Page");

    // WiFi status
    tft.setCursor(10, 70);
    tft.print(wifiConnected ? "WiFi: Connected" : "WiFi: Disconnected");

    // Monitoring status
    tft.setCursor(10, 90);
    tft.print(monitoringOn ? "Monitoring: On" : "Monitoring: Off");

    // Alarm status
    tft.setCursor(10, 110);
    tft.print(alarmOn ? "Alarm: On" : "Alarm: Off");
}

void drawSettingsMenu() {
    tft.fillScreen(BLACK);

    // Draw back button
    tft.fillRect(5, 5, 50, 20, YELLOW); // Rectangular button
    tft.setTextColor(BLACK);
    tft.setTextSize(1);
    tft.setCursor(8, 10);
    tft.print("Back");

    // Alarm toggle button
    tft.fillRect(5, 50, 100, 20, alarmOn ? GREEN : RED);
    tft.setCursor(8, 55);
    tft.setTextColor(WHITE);
    tft.print("Toggle Alarm");

    // Monitoring toggle button
    tft.fillRect(5, 90, 100, 20, monitoringOn ? GREEN : RED);
    tft.setCursor(8, 95);
    tft.print("Toggle Monitor");

    // Wi-Fi toggle button
    tft.fillRect(5, 130, 100, 20, wifiConnected ? GREEN : RED);
    tft.setCursor(8, 135);
    tft.print("Toggle Wi-Fi");
}
