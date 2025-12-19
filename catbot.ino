/*
 * 🐱 N.E.K.O. (Network Embedded Kitty Object) - FINAL STABLE
 * ----------------------------------------------------------
 * Hardware:
 * - ESP32 DevKit V1
 * - OLED SSD1306 (SDA:21, SCL:22)
 * - IR Sensor (GPIO 19)
 * - Tombol Menu (GPIO 15) -> Sambung ke GND saat ditekan
 * - Tombol BOOT (GPIO 0) -> Cadangan
 * * Fitur:
 * - Normal: Animasi Kucing, Jam (WIB), Custom Image.
 * - Admin: Tekan tombol -> Hotspot "Neko-Admin" -> Buka 192.168.4.1
 * - Web Menu: Upload Gambar & Reset WiFi.
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiManager.h> 
#include "time.h"
#include <LittleFS.h>
#include <WebServer.h>

// --- KONFIGURASI PIN ---
#define IR_SENSOR_PIN   19  
#define EXT_BUTTON_PIN  15  // Tombol Menu Eksternal
#define BOOT_BUTTON_PIN 0   // Tombol Bawaan

// --- LAYAR ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- SERVER & VARIABLE ---
WebServer server(80);
bool hasCustomFace = false;
uint8_t customFaceBuffer[1024];
bool isAdminMode = false;       

// --- ANIMASI & LOGIC ---
enum AppMode { MODE_CAT, MODE_CLOCK, MODE_CUSTOM };
AppMode currentMode = MODE_CAT; 

// Variabel Mata
int pupilOffset = 0; 
unsigned long lastBlinkTime = 0; 
int blinkInterval = 3000; 
bool isBlinking = false; 
bool isWinking = false; 
int winkSide = 0;         

// Variabel IR Sensor
int lastIrState = HIGH; 
unsigned long lastInputTime = 0;

// Waktu (WIB / GMT+7)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200; 
const int   daylightOffset_sec = 0;

// ==========================================
// 1. FUNGSI FILE SYSTEM
// ==========================================

void loadCustomFace() {
  if (LittleFS.exists("/face.bin")) {
    File file = LittleFS.open("/face.bin", "r");
    if (file.size() == 1024) { 
      file.read(customFaceBuffer, 1024);
      hasCustomFace = true;
    }
    file.close();
  } else { hasCustomFace = false; }
}

// ==========================================
// 2. MODE ADMIN (WEB SERVER)
// ==========================================

void enterAdminMode() {
  isAdminMode = true; // Kunci Mode Admin
  
  // Tampilan Layar Info
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("== ADMIN MENU ==");
  display.println("");
  display.println("WiFi: Neko-Admin");
  display.println("Pass: 12345678");
  display.println("");
  display.println("IP:   192.168.4.1");
  display.display();

  // Matikan Station, Nyalakan AP
  WiFi.disconnect(); 
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Neko-Admin", "12345678");

  // --- HALAMAN WEB ---
  server.on("/", HTTP_GET, [](){
    String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>body{background:#222;color:#0f0;font-family:monospace;text-align:center;padding:20px;}";
    html += ".box{border:2px solid #0f0;padding:15px;margin-bottom:20px;border-radius:10px;}";
    html += "button{width:100%;padding:12px;font-weight:bold;margin:5px 0;border:none;border-radius:5px;cursor:pointer;}";
    html += ".btn-up{background:#0f0;color:#000;} .btn-reset{background:#f90;color:#000;} .btn-exit{background:#f00;color:#fff;}";
    html += "</style></head><body>";
    
    html += "<h1>🐱 N.E.K.O. PANEL</h1>";
    
    // Upload Box
    html += "<div class='box'><h3>1. GANTI WAJAH</h3>";
    html += "<p>Upload file .bin (128x64)</p>";
    html += "<form method='POST' action='/upload' enctype='multipart/form-data'>";
    html += "<input type='file' name='upload' style='color:white;margin-bottom:10px;'><br>";
    html += "<input type='submit' value='UPLOAD GAMBAR' class='btn-up'>";
    html += "</form></div>";
    
    // Reset WiFi Box
    html += "<div class='box'><h3>2. KONEKSI</h3>";
    html += "<p>Ganti WiFi Rumah?</p>";
    html += "<a href='/resetwifi' onclick=\"return confirm('Reset WiFi sekarang?')\">";
    html += "<button class='btn-reset'>RESET WIFI SETTINGS</button></a></div>";

    // Exit Button
    html += "<br><a href='/exit'><button class='btn-exit'>KELUAR / RESTART</button></a>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  // Handler Upload (Fixed for Firefox)
  server.on("/upload", HTTP_POST, 
    [](){ server.send(200, "text/html", "<h2>UPLOAD SUKSES!</h2><a href='/'>Kembali</a>"); }, 
    [](){
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        LittleFS.remove("/face.bin"); File file = LittleFS.open("/face.bin", "w"); file.close();
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        File file = LittleFS.open("/face.bin", "a"); if (file) file.write(upload.buf, upload.currentSize); file.close();
      } else if (upload.status == UPLOAD_FILE_END) { loadCustomFace(); }
    }
  );

  // Handler Reset WiFi
  server.on("/resetwifi", HTTP_GET, [](){
    server.send(200, "text/html", "<h2>WiFi Direset!</h2><p>Alat sedang restart...</p>");
    display.clearDisplay(); display.setCursor(0,20); display.println("RESETTING WIFI..."); display.display();
    WiFiManager wm; wm.resetSettings();
    delay(2000); ESP.restart();
  });

  // Handler Exit (Clean Restart)
  server.on("/exit", HTTP_GET, [](){
    server.send(200, "text/html", "<h2>Bye Bye!</h2>");
    delay(500);
    display.clearDisplay(); display.setCursor(0,20); display.println("REBOOTING..."); display.display();
    WiFi.disconnect(true); WiFi.mode(WIFI_OFF);
    delay(1000); ESP.restart(); 
  });

  server.begin();
}

// ==========================================
// 3. SETUP
// ==========================================

// ==========================================
// 3. SETUP (PERBAIKAN WIFI RESET)
// ==========================================

void setup() {
  Serial.begin(115200);
  pinMode(EXT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(IR_SENSOR_PIN, INPUT);

  // Init Layar
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) for(;;);
  }
  display.clearDisplay(); display.setTextColor(SSD1306_WHITE);

  // Init File System
  if(!LittleFS.begin(true)){ Serial.println("FS Error"); }
  loadCustomFace();

  // STARTUP
  display.setCursor(0,20);
  display.println("Starting...");
  display.display();

  // --- LOGIKA WIFI MANAGER BARU ---
  WiFiManager wifiManager;
  
  // 1. Timeout Koneksi ke Router (Coba connect 10 detik)
  wifiManager.setConnectTimeout(10); 
  
  // 2. Timeout Hotspot Setup (PENTING!)
  // Ganti dari 1 menjadi 180 (3 Menit).
  // Ini memberi kamu waktu 3 menit untuk connect ke "CatBot-Setup" setelah reset.
  wifiManager.setConfigPortalTimeout(180); 

  // Coba connect otomatis
  // Jika gagal (atau habis direset), dia bikin Hotspot "CatBot-Setup"
  if(!wifiManager.autoConnect("CatBot-Setup")) {
    
    // Jika setelah 3 menit (180s) kamu tidak connect juga, baru dia nyerah
    display.clearDisplay(); display.setCursor(0,0);
    display.println("WiFi Setup Timeout!"); 
    display.println("Offline Mode"); 
    display.display();
    delay(2000);
    
  } else {
    // Jika berhasil connect
    display.clearDisplay(); display.setCursor(0,20);
    display.println("WiFi Connected!"); 
    display.display();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }
}
// ==========================================
// 4. LOOP UTAMA (ANTI-HANG & ONE-SHOT)
// ==========================================

void loop() {
  // 1. JIKA MODE ADMIN AKTIF -> Stop semua logika lain
  if (isAdminMode) {
    server.handleClient();
    delay(10); 
    return; // PENTING: Jangan lanjut ke bawah
  }

  // 2. CEK TOMBOL (Hanya jika belum Admin)
  bool btnExt = (digitalRead(EXT_BUTTON_PIN) == LOW);
  bool btnBoot = (digitalRead(BOOT_BUTTON_PIN) == LOW);

  if (btnExt || btnBoot) {
    delay(50); // Debounce
    // Cek lagi untuk memastikan
    if (digitalRead(EXT_BUTTON_PIN) == LOW || digitalRead(BOOT_BUTTON_PIN) == LOW) {
      
      // Visual Feedback
      display.clearDisplay();
      display.setCursor(0, 20);
      display.println("SWITCHING MODE...");
      display.println("PLEASE WAIT");
      display.display();
      
      // JALANKAN PERINTAH (Hanya Sekali!)
      enterAdminMode(); 
      
      // Tunggu tombol dilepas (Biar gak restart looping)
      while(digitalRead(EXT_BUTTON_PIN) == LOW || digitalRead(BOOT_BUTTON_PIN) == LOW) {
        delay(100);
      }
    }
  }

  // 3. LOGIKA NORMAL (Hanya jalan jika tombol tidak ditekan)
  runNormalMode();
}

// ==========================================
// 5. ANIMASI & FUNGSI GAMBAR
// ==========================================

void runNormalMode() {
  handleIrInput(); 
  if (currentMode == MODE_CAT) drawAnimatedFace();
  else if (currentMode == MODE_CLOCK) drawClock();
  else if (currentMode == MODE_CUSTOM) drawCustomImage();
  delay(20);
}

void handleIrInput() {
  int reading = digitalRead(IR_SENSOR_PIN);
  if (reading == LOW && lastIrState == HIGH && (millis() - lastInputTime > 500)) {
      if (currentMode == MODE_CAT) currentMode = MODE_CLOCK;
      else if (currentMode == MODE_CLOCK) {
        if (hasCustomFace) currentMode = MODE_CUSTOM; else currentMode = MODE_CAT;
      } else currentMode = MODE_CAT;
      display.clearDisplay(); lastInputTime = millis();
  }
  lastIrState = reading;
}

void drawCustomImage() {
  display.clearDisplay(); display.drawBitmap(0, 0, customFaceBuffer, 128, 64, SSD1306_WHITE); display.display();
}

void drawClock() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    display.clearDisplay(); display.setCursor(0,0);
    display.println("No Internet"); display.println("Check WiFi"); display.display(); return;
  }
  display.clearDisplay();
  display.setTextSize(3); display.setCursor(20, 15);
  display.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  display.setTextSize(1); display.setCursor(30, 50);
  display.printf("%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  display.display();
}

// --- ANIMASI WAJAH (ANTI-STUCK / FAIL SAFE) ---
void drawAnimatedFace() {
  unsigned long currentMillis = millis();

  // Trigger Kedip
  if (currentMillis - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = currentMillis; blinkInterval = random(2000, 6000); 
    if (random(0, 100) < 20) { isWinking = true; isBlinking = false; winkSide = random(0, 2); } 
    else { isBlinking = true; isWinking = false; }
  }

  // Reset Kedip (Fail Safe: Paksa melek jika > 500ms)
  if ((isBlinking || isWinking) && (currentMillis - lastBlinkTime > 200)) { 
    isBlinking = false; isWinking = false; 
  }
  if (currentMillis - lastBlinkTime > 500) { // Safety Extra
     isBlinking = false; isWinking = false; 
  }

  display.clearDisplay();
  if (random(0, 100) > 90) pupilOffset = random(-4, 5);

  // Gambar Mata
  if (isBlinking || (isWinking && winkSide == 0)) drawClosedEye(40, 32); 
  else drawOpenEye(40, 32, pupilOffset); 

  if (isBlinking || (isWinking && winkSide == 1)) drawClosedEye(88, 32); 
  else drawOpenEye(88, 32, pupilOffset);

  // Gambar Mulut
  int mX = 64; int mY = 42;
  display.drawLine(mX - 12, mY, mX - 6, mY + 5, SSD1306_WHITE);
  display.drawLine(mX - 6, mY + 5, mX, mY, SSD1306_WHITE);
  display.drawLine(mX, mY, mX + 6, mY + 5, SSD1306_WHITE);
  display.drawLine(mX + 6, mY + 5, mX + 12, mY, SSD1306_WHITE);
  display.display();
}

void drawOpenEye(int x, int y, int offset) {
  display.fillCircle(x, y, 10, SSD1306_WHITE); 
  display.fillCircle(x + offset, y, 4, SSD1306_BLACK); 
  display.drawPixel(x + offset - 1, y - 1, SSD1306_WHITE); 
}

void drawClosedEye(int x, int y) {
  display.drawCircle(x, y, 10, SSD1306_WHITE); 
  display.fillRect(x - 11, y - 12, 23, 13, SSD1306_BLACK);
}
