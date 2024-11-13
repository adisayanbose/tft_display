#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>

#define YP A2  
#define XM A1  
#define YM 6   
#define XP 7   

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

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

bool alarmOn = true;       
bool monitoringOn = true;  
bool wifiConfigMode = false; 
bool dripFactorScreen = false;  // To track if the Drip Factor screen is active

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("IV Drip Assist System!"));

  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);

  drawHomeScreen();  // Draw home screen with buttons directly visible
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop() {
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    int16_t screenX = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    int16_t screenY = map(p.y, TS_MAXY, TS_MINY, tft.height(), 0);

    // Handle touch events directly for the buttons on home screen
    if (dripFactorScreen) {
      // Drip Factor Screen buttons
      if (screenY > 60 && screenY < 100) {  // 10 button
        Serial.println("Drip Factor 10 selected");
        dripFactorScreen = false;
        drawHomeScreen();
      } else if (screenY > 100 && screenY < 140) {  // 15 button
        Serial.println("Drip Factor 15 selected");
        dripFactorScreen = false;
        drawHomeScreen();
      } else if (screenY > 140 && screenY < 180) {  // 20 button
        Serial.println("Drip Factor 20 selected");
        dripFactorScreen = false;
        drawHomeScreen();
      }
    } else {
      // Home screen buttons
      if (screenY > 60 && screenY < 100) {  // Alarm button area
        alarmOn = !alarmOn;
        drawHomeScreen();  // Redraw the home screen to update alarm button state
        Serial.println(alarmOn ? "Alarm ON" : "Alarm OFF");
      } else if (screenY > 100 && screenY < 140) {  // WiFi Config button area
        wifiConfigMode = true;
        displayWiFiConfigMenu();
      } else if (screenY > 180 && screenY < 220) {  // Monitoring button area
        monitoringOn = !monitoringOn;
        drawHomeScreen();  // Redraw the home screen to update monitoring button state
        Serial.println(monitoringOn ? "Monitoring ON" : "Monitoring OFF");
      } else if (screenY > 140 && screenY < 180) {  // SOS Config button area
        Serial.println("SOS Config selected");
      } else if (screenY > 220 && screenY < 260) {  // Drip Factor button area
        dripFactorScreen = true;
        displayDripFactorScreen();
      } else if (screenY > tft.height() - 50 && screenX < 60) {  // Back button area
        Serial.println("Back button pressed");
        drawHomeScreen();
      } else if (screenY > tft.height() - 50 && screenX > tft.width() - 60) {  // Forward button area
        Serial.println("Forward button pressed");
      } else if (screenY > tft.height() - 50 && screenX > tft.width() / 2 - 50 && screenX < tft.width() / 2 + 50) {  // Save button area
        Serial.println("Settings saved");
      } else if (screenY < 40 && screenX < 60) {  // Home icon area
        Serial.println("Home icon pressed");
        drawHomeScreen();
      }
    }
  }
}

void drawHomeScreen() {
  tft.fillScreen(BLACK);

  // Home Icon Button
  tft.fillRect(10, 10, 40, 40, BLUE);  // Home button
  tft.setCursor(15, 15);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("H");

  // Alarm Button
  tft.fillRect(10, 60, tft.width() - 20, 30, alarmOn ? RED : BLUE);
  tft.setCursor(15,  65);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(alarmOn ? "Alarm ON" : "Alarm OFF");

  // WiFi Config Button
  tft.fillRect(10, 100, tft.width() - 20, 30, GREEN);
  tft.setCursor(15, 105);
  tft.setTextColor(WHITE);
  tft.print("WiFi Config");

  // SOS Config Button
  tft.fillRect(10, 140, tft.width() - 20, 30, RED);
  tft.setCursor(15, 145);
  tft.setTextColor(WHITE);
  tft.print("SOS Config");

  // Monitoring Button
  tft.fillRect(10, 180, tft.width() - 20, 30, monitoringOn ? MAGENTA : BLUE);
  tft.setCursor(15, 185);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(monitoringOn ? "Monitoring ON" : "Monitoring OFF");

  // Drip Factor Button
  tft.fillRect(10, 220, tft.width() - 20, 30, CYAN);
  tft.setCursor(15, 225);
  tft.setTextColor(WHITE);
  tft.print("Drip Factor");

  // Back Button
  tft.fillRect(10, tft.height() - 50, 40, 40, BLUE);
  tft.setCursor(15, tft.height() - 40);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("<");

  // Save Button
  tft.fillRect(tft.width() / 2 - 50, tft.height() - 50, 100, 40, GREEN);
  tft.setCursor(tft.width() / 2 - 30, tft.height() - 40);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Save");

  // Forward Button
  tft.fillRect(tft.width() - 50, tft.height() - 50, 40, 40, RED);
  tft.setCursor(tft.width() - 40, tft.height() - 40);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print(">");
}

void displayWiFiConfigMenu() {
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("WiFi Configuration");

  // WiFi Name button
  tft.fillRect(10, 60, tft.width() - 20, 30, GREEN);
  tft.setCursor(15, 65);
  tft.setTextColor(WHITE);
  tft.print("Enter WiFi Name");

  // Password button
  tft.fillRect(10, 100, tft.width() - 20, 30, GREEN);
  tft.setCursor(15, 105);
  tft.setTextColor(WHITE);
  tft.print("Enter Password");

  // Back button
  tft.fillRect(10, tft.height() - 50, 40, 40, BLUE);
  tft.setCursor(15, tft.height() - 40);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("<");

  // Handle touch events on the buttons
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    int16_t screenX = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    int16_t screenY = map(p.y, TS_MAXY, TS_MINY, tft.height(), 0);

    // Back button action
    if (screenY > tft.height() - 50 && screenX < 60) {
      drawHomeScreen();
    }
  }
}

void displayDripFactorScreen() {
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Drip Factor Selection");

  // Drip factor options (10, 15, 20)
  tft.fillRect(10, 60, tft.width() - 20, 30, CYAN);
  tft.setCursor(15, 65);
  tft.setTextColor(WHITE);
  tft.print("Drip Factor 10");

  tft.fillRect(10, 100, tft.width() - 20, 30, CYAN);
  tft.setCursor(15, 105);
  tft.setTextColor(WHITE);
  tft.print("Drip Factor 15");

  tft.fillRect(10, 140, tft.width() - 20, 30, CYAN);
  tft.setCursor(15, 145);
  tft.setTextColor(WHITE);
  tft.print("Drip Factor 20");

  // Back button action
  tft.fillRect(10, tft.height() - 50, 40, 40, BLUE);
  tft.setCursor(15, tft.height() - 40);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("<");
}
