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

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40
#define LETTER_WIDTH 30
#define LETTER_HEIGHT 30
#define MAX_PASSWORD_LENGTH 20

String password = "";  // To store the entered password
bool lastTouchState = false; // Track the last touch state
bool letterAdded = false; // Track if a letter has been added

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("Password Entry!"));

  tft.reset();
  
  uint16_t identifier = tft.readID();
  tft.begin(identifier);

  tft.fillScreen(BLACK);

  // Draw alphabet letters with boundaries
  drawAlphabet();

  // Draw the submit and backspace buttons
  drawButtons();

  pinMode(13, OUTPUT);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop() {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  bool currentTouchState = (p.z > MINPRESSURE && p.z < MAXPRESSURE);

  // Check if the touch state has changed
  if (currentTouchState) { // Touch is currently pressed
    if (!lastTouchState) { // Touch has just started
      // Apply coordinate inversion here
      int16_t screenX = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
      int16_t screenY = map(p.y, TS_MAXY, TS_MINY, tft.height(), 0);

      // Check if an alphabet letter was pressed
      if (screenY > BOXSIZE && screenY < tft.height() - 50) { // Exclude button area
        char selectedLetter = getSelectedLetter(screenX, screenY);
        if (selectedLetter != '\0' && !letterAdded) {
          addToPassword(selectedLetter); // Add letter to password
          letterAdded = true; // Set the flag to indicate a letter has been added
        }
      }

      // Check if backspace was pressed
      if (screenY >= tft.height() - 50 && screenY < tft.height() - 20 && screenX < tft.width() / 2) {
        removeLastCharacter();
      }

      // Check if submit was pressed
      if (screenY >= tft.height() - 50 && screenY < tft.height() - 20 && screenX >= tft.width() / 2) {
        submitPassword();
      }
    }
  } else { // Touch is released
    letterAdded = false; // Reset the flag when the touch is released
  }

  // Update last touch state
  lastTouchState = currentTouchState;
}


void drawAlphabet() {
  int x = 0;
  int y = BOXSIZE + 10; // Start below the color boxes
  for (char letter = 'A'; letter <= 'Z'; letter++) {
    // Draw a rectangle around each letter
    tft.drawRect(x, y, LETTER_WIDTH, LETTER_HEIGHT, WHITE);
    
    // Display the letter
    tft.setCursor(x + 5, y +  5); // Center the letter in the rectangle
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print(letter);

    x += LETTER_WIDTH; // Move to the right for the next letter
    if (x > tft.width() - LETTER_WIDTH) { // Move to the next line if the end of the screen is reached
      x = 0; 
      y += LETTER_HEIGHT;
    }
  }
}

void drawButtons() {
  // Draw Backspace button
  tft.fillRect(0, tft.height() - 50, tft.width() / 2, 30, RED);
  tft.setCursor(10, tft.height() - 45);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Backspace");

  // Draw Submit button
  tft.fillRect(tft.width() / 2, tft.height() - 50, tft.width() / 2, 30, GREEN);
  tft.setCursor(tft.width() / 2 + 10, tft.height() - 45);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Submit");
}

char getSelectedLetter(int x, int y) {
  // Calculate the letter index based on the position
  int letterIndex = (y - (BOXSIZE + 10)) / LETTER_HEIGHT * (tft.width() / LETTER_WIDTH) + x / LETTER_WIDTH;
  if (letterIndex >= 0 && letterIndex < 26) {
    return 'A' + letterIndex; // Return the corresponding letter
  }
  return '\0'; // Return null character if out of bounds
}

void addToPassword(char letter) {
  if (password.length() < MAX_PASSWORD_LENGTH) {
    password += letter; // Append the letter to the password
    displayPassword();
  }
}

void removeLastCharacter() {
  if (password.length() > 0) {
    password.remove(password.length() - 1); // Remove the last character
    displayPassword();
  }
}

void displayPassword() {
  tft.fillRect(0, tft.height() - 30, tft.width(), 30, BLACK); // Clear the area
  tft.setCursor(10, tft.height() - 30);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Password: ");
  tft.print(password); // Display the current password
}

void submitPassword() {
  // Here you can handle the password submission logic
  Serial.print("Submitted Password: ");
  Serial.println(password);
  // Optionally, clear the password after submission
  password = "";
  displayPassword();
}