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

#define YELLOW  0xFFE0  // Yellow
#define MAGENTA 0xF81F  // Magenta
#define ORANGE  0xFC00  // Orange
#define PURPLE  0x780F  // Purple
#define BROWN   0xA145  // Brown
#define GRAY    0x8410  // Gray
#define LIGHT_GRAY  0xD69A // Light Gray
#define LIGHT_BLUE  0x5DFF // Light Blue
#define PINK    0xF81F  // Pink
#define LIGHT_GREEN  0xB7E0 // Light Green
#define TEAL    0x4C1F  // Teal
#define LIME    0x07E0  // Lime (similar to green)
#define GOLD    0xFEA0  // Gold
#define NAVY    0x000F  // Navy Blue
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define WHITE   0xFFFF 

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define MINPRESSURE 10
#define MAXPRESSURE 1000

bool alarmOn = false;
bool wifiConnected = true;
bool monitoringOn = false; // New variable for monitoring status
int ivFlowRate = 90; // Example IV flow rate in mL/hr

void setup() {
    Serial.begin(9600);
    Serial.println(F("Home Page"));

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

        // Check if settings button is pressed
        if (screenX >= 5 && screenX <= 35 && screenY >= 5 && screenY <= 25) {
            Serial.println("Settings button pressed");
        }

        // Check if alarm symbol is pressed
        if (screenX >= 120 && screenX <= 140 && screenY >= 5 && screenY <= 25) {
            alarmOn = !alarmOn;
            drawHomePage();
        }

        // Check if WiFi symbol is pressed
        if (screenX >= 160 && screenX <= 180 && screenY >= 5 && screenY <= 25) {
            wifiConnected = !wifiConnected;
            drawHomePage();
        }

        // Check if monitoring symbol is pressed
        if (screenX >= 200 && screenX <= 220 && screenY >= 5 && screenY <= 25) {
            monitoringOn = !monitoringOn;
            drawHomePage();
        }
    }
}

void drawHomePage() {
    tft.fillScreen(BLACK);

    // Draw settings button
    drawSettingsButton();

    // Draw IV flow circular indicator
    drawIVFlowIndicator();

    // Draw Wi-Fi status text box
    drawWifiStatus();

    // Draw Monitoring status text box
    drawMonitoringStatus();

    // Draw alarm status text box
    drawAlarmStatus();

    // Draw Drop Factor at the bottom
    drawDropFactor();
}

void drawSettingsButton() {
    tft.fillRect(5, 5, 50, 20, YELLOW); // Rectangular button
    tft.setTextColor(BLACK);
    tft.setTextSize(1);
    tft.setCursor(8, 10);
    tft.print("Setting");
}

void drawIVFlowIndicator() {
    int xCenter = tft.width() / 2;
    int yCenter = tft.height() / 2;
    int radius = 70;  // Increased the radius to 100

    tft.drawCircle(xCenter, yCenter, radius, WHITE);
    tft.setTextColor(WHITE);
    tft.setTextSize(4);
    tft.setCursor(xCenter - 25, yCenter - 25);  // Adjusted positioning for larger radius
    tft.print(ivFlowRate);
    tft.setTextSize(2);
    tft.setCursor(xCenter - 28, yCenter + 30);  // Centered the text for "mL/hr"
    tft.print("mL/hr");
}

void drawWifiStatus() {
    int x = tft.width() - 100;  // Position for Wi-Fi status box on the right
    int y = 5;  // Below Monitoring

    // Draw a rectangle for the Wi-Fi status
    tft.fillRect(x, y, 100, 20, WHITE); // Rectangular background
    tft.setTextSize(1);
    tft.setCursor(x + 5, y + 5);
    tft.setTextColor(BLACK);

    // Draw "Wi-Fi Connected" or "Wi-Fi Disconnected" text
    if (wifiConnected) {
        tft.fillRect(x, y, 100, 20, GREEN);
        tft.print("Wi-Fi Connected");
    } else {
        tft.fillRect(x, y, 100, 20, RED);  // Use RED for disconnected status
        tft.print("Wi-Fi Disconnected");
    }
}

void drawMonitoringStatus() {
    int x = tft.width() - 90;  // Position for Monitoring status box
    int y = 30;  // Push to the top

    // Draw a rectangle for the Monitoring status
    tft.fillRect(x, y, 100, 20, WHITE); // Rectangular background
    tft.setTextSize(1);
    tft.setCursor(x + 5, y + 5);
    tft.setTextColor(BLACK);

    // Draw "Monitoring On" or "Monitoring Off" text
    if (monitoringOn) {
        tft.fillRect(x, y, 100, 20, GREEN);
        tft.print("Monitoring On");
    } else {
        tft.fillRect(x, y, 100, 20, RED);
        tft.print("Monitoring Off");
    }
}

void drawAlarmStatus() {
    int x = tft.width() - 70;  // Position for Alarm status box below Wi-Fi
    int y = 55;  // Directly below Wi-Fi

    // Draw a rectangle for the alarm status
    tft.setTextColor(BLACK);
    tft.setTextSize(1);
    tft.setCursor(x + 5, y + 5);

    // Draw "Alarm On" or "Alarm Off" text
    if (alarmOn) {
        tft.fillRect(x, y, 70, 20, GREEN);  // Use GREEN for alarm on
        tft.print("Alarm On");
    } else {
        tft.fillRect(x, y, 70, 20, RED);  // Use RED for alarm off
        tft.print("Alarm Off");
    }
}

void drawDropFactor() {
    int x = 0;  // X position of the rectangle (full width)
    int y = tft.height() - 30; // Y position for the bottom of the screen
    int width = tft.width();  // Full screen width
    int height = 30;  // Height of the rectangle

    // Draw rectangle at the bottom
    tft.fillRect(x, y, width, height, LIGHT_BLUE);  // Light blue rectangle at the bottom

    // Set the text color and size
    tft.setTextColor(BLACK);

    // Draw "Drop Factor" with text size 2
    tft.setTextSize(2);
    tft.setCursor(x + 10, y + 5);  // Add some padding
    tft.print("Drop Factor = 15");

    // Draw "15 GTT/ML" with text size 1
    tft.setTextSize(1);
    tft.setCursor(x + 200, y+20 );  // Add some padding
    tft.print("gtt/ml");
}
