/*
 * N.E.K.O. (Network Embedded Kitty Object)
 * Hardware: ESP32 + SSD1306 OLED + IR Sensor
 * Features: 
 * - IR Sensor (GPIO 19): Wave hand to switch Mode (Face/Clock)
 * - Safe WiFi Reset: Hold BOOT button during startup
 * - Animated Eyes & Internet Clock
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiManager.h> // Library: "WiFiManager" by tzapu
#include "time.h"

// --- PINS ---
#define IR_SENSOR_PIN 19  // The new IR Sensor Pin
#define BUTTON_PIN 0      // Built-in BOOT button (Used only for WiFi Reset now)

// --- CONFIGURATION ---
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200; // GMT+7 (WIB)
const int   daylightOffset_sec = 0;

// --- SCREEN ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- LOGIC GLOBALS ---
enum AppMode { MODE_CAT, MODE_CLOCK };
AppMode currentMode = MODE_CAT; 

// Animation Vars
int pupilOffset = 0;      
unsigned long lastBlinkTime = 0;
int blinkInterval = 3000; 
bool isBlinking = false;
bool isWinking = false;
int winkSide = 0;         

// Input Handling Vars
int lastIrState = HIGH;        // Assume standard IR sensor (High = Clear, Low = Object)
unsigned long lastInputTime = 0;

// --- WIFI CALLBACK ---
void configModeCallback (WiFiManager *myWiFiManager) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  display.println("WiFi Not Found!");
  display.println("----------------");
  display.println("Starting Setup AP...");
  display.println("");
  display.print("Name: "); display.println("CatBot-AP"); 
  display.println("(Use private pass)"); 
  display.display();
  
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
}

void setup() {
  Serial.begin(115200);
  
  // 1. Init Pins
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Still needed for Reset
  pinMode(IR_SENSOR_PIN, INPUT);     // IR Sensor Input

  // 2. Init Screen
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("Screen allocation failed"));
    for(;;); 
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // 3. SAFE RESET LOGIC (Hold BOOT button to reset WiFi)
  // We keep this on the physical button because it's safer for maintenance
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("To Reset WiFi:");
  display.println("Hold BOOT Button...");
  display.drawRect(0, 40, 128, 10, SSD1306_WHITE);
  display.display();

  bool forceSetup = false;
  for (int i = 0; i < 128; i++) {
    display.fillRect(0, 40, i, 10, SSD1306_WHITE);
    display.display();
    
    if (digitalRead(BUTTON_PIN) == LOW) {
      forceSetup = true;
      break; 
    }
    delay(20); 
  }

  // 4. WiFiManager Logic
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);

  if (forceSetup) {
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Button Detected!");
    display.println("Forcing Setup...");
    display.display();
    delay(1000); 

    if (!wifiManager.startConfigPortal("CatBot-AP", "catbotyasha")) {
      ESP.restart();
    }
  } else {
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Connecting...");
    display.display();
    
    if(!wifiManager.autoConnect("CatBot-AP", "catbotyasha")) {
      Serial.println("Failed to connect");
      ESP.restart(); 
    }
  }

  // 5. Success
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("WiFi Connected!");
  display.println("Syncing Clock...");
  display.display();
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
     display.println("Time OK!");
  } else {
     display.println("Time Error");
  }
  display.display();
  delay(1000);
}

void loop() {
  handleInput(); // Check IR Sensor

  if (currentMode == MODE_CAT) {
    drawAnimatedFace();
  } else {
    drawClock();
  }
  
  delay(20); 
}

// --- NEW INPUT LOGIC (IR SENSOR) ---
void handleInput() {
  // Read the IR Sensor on Pin 19
  int reading = digitalRead(IR_SENSOR_PIN);
  
  // Logic: Most IR sensors go LOW when they see an object (Hand)
  // We check if it IS Low (Object seen) AND it WAS High (Empty before)
  // We add a longer delay (500ms) to prevent double-triggering while waving
  if (reading == LOW && lastIrState == HIGH && (millis() - lastInputTime > 500)) {
      
      // Toggle Mode
      if (currentMode == MODE_CAT) {
        currentMode = MODE_CLOCK;
      } else {
        currentMode = MODE_CAT;
      }
      
      display.clearDisplay(); 
      lastInputTime = millis();
  }
  lastIrState = reading;
}

// --- ANIMATION LOGIC ---
void drawAnimatedFace() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = currentMillis;
    blinkInterval = random(2000, 6000); 

    if (random(0, 100) < 20) { 
      isWinking = true;
      isBlinking = false;
      winkSide = random(0, 2); 
    } else {
      isBlinking = true;
      isWinking = false;
    }
  }

  if ((isBlinking || isWinking) && (currentMillis - lastBlinkTime > 200)) {
    isBlinking = false;
    isWinking = false;
  }

  display.clearDisplay();
  
  if (random(0, 100) > 90) pupilOffset = random(-4, 5);

  if (isBlinking || (isWinking && winkSide == 0)) {
    drawClosedEye(40, 32); 
  } else {
    drawOpenEye(40, 32, pupilOffset); 
  }

  if (isBlinking || (isWinking && winkSide == 1)) {
    drawClosedEye(88, 32);
  } else {
    drawOpenEye(88, 32, pupilOffset);
  }

  int mX = 64; 
  int mY = 42;
  display.drawLine(mX - 12, mY, mX - 6, mY + 5, SSD1306_WHITE);
  display.drawLine(mX - 6, mY + 5, mX, mY, SSD1306_WHITE);
  display.drawLine(mX, mY, mX + 6, mY + 5, SSD1306_WHITE);
  display.drawLine(mX + 6, mY + 5, mX + 12, mY, SSD1306_WHITE);

  display.display();
}

// --- DRAWING HELPERS ---
void drawOpenEye(int x, int y, int offset) {
  display.fillCircle(x, y, 10, SSD1306_WHITE);
  display.fillCircle(x + offset, y, 4, SSD1306_BLACK); 
  display.drawPixel(x + offset - 1, y - 1, SSD1306_WHITE); 
}

void drawClosedEye(int x, int y) {
  display.drawCircle(x, y, 10, SSD1306_WHITE);
  display.fillRect(x - 11, y - 12, 23, 13, SSD1306_BLACK);
}

// --- CLOCK DISPLAY ---
void drawClock() {
  struct tm timeinfo;
  
  if(!getLocalTime(&timeinfo)){
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Syncing Time...");
    display.display();
    return;
  }

  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 15);
  display.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  
  display.setTextSize(1);
  display.setCursor(30, 50);
  display.printf("%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  
  display.display();
}