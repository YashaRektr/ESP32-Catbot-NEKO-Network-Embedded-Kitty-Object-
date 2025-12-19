/*
 * 🐱 N.E.K.O. - EASY ADMIN VERSION
 * -------------------------------------------------------
 * Cara Pakai:
 * 1. Nyalakan alat seperti biasa (Mode Kucing/Jam).
 * 2. Ingin Upload Gambar? TAHAN tombol BOOT (di board) selama 3 detik.
 * 3. Alat akan berubah jadi Mode Admin (WiFi Hotspot).
 * 4. Connect Laptop ke WiFi "Neko-Admin", buka 192.168.4.1
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiManager.h> 
#include "time.h"
#include <LittleFS.h>
#include <WebServer.h>

// --- PIN ---
#define IR_SENSOR_PIN 19  
#define BUTTON_PIN 0      // Tombol BOOT

// --- LAYAR ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- SERVER & VARS ---
WebServer server(80);
bool hasCustomFace = false;
uint8_t customFaceBuffer[1024];

// Status System
bool isAdminMode = false;       // False = Mode Kucing, True = Mode Upload
unsigned long buttonPressStart = 0; // Untuk hitung durasi tekan tombol

// Vars Animasi
enum AppMode { MODE_CAT, MODE_CLOCK, MODE_CUSTOM };
AppMode currentMode = MODE_CAT; 
int pupilOffset = 0; unsigned long lastBlinkTime = 0; int blinkInterval = 3000; 
bool isBlinking = false; bool isWinking = false; int winkSide = 0;         
int lastIrState = HIGH; unsigned long lastInputTime = 0;

// Time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200; 
const int   daylightOffset_sec = 0;

// ==========================================
// 1. FUNGSI ADMIN & FILE
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

void enterAdminMode() {
  isAdminMode = true;
  
  // Matikan WiFi Station & Nyalakan AP
  WiFi.disconnect(); 
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Neko-Admin", "12345678");

  // --- HALAMAN UTAMA ---
  server.on("/", HTTP_GET, [](){
    String html = "<html><body style='background:#222;color:#0f0;text-align:center;font-family:monospace;'>";
    html += "<h1>N.E.K.O ADMIN MODE</h1>";
    html += "<div style='border:2px solid #0f0;padding:20px;'>";
    html += "<h3>UPLOAD IMAGE (.bin)</h3>";
    html += "<form method='POST' action='/upload' enctype='multipart/form-data'>";
    html += "<input type='file' name='upload' style='color:white;'><br><br>";
    html += "<input type='submit' value='UPLOAD' style='background:#0f0;padding:10px;font-weight:bold;'>";
    html += "</form></div>";
    html += "<br><a href='/exit'><button style='padding:15px;background:red;color:white;'>KELUAR ADMIN MODE</button></a>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  // --- BAGIAN YANG DIPERBAIKI (FIX CORRUPTED ERROR) ---
  server.on("/upload", HTTP_POST, 
    // 1. Handler RESPON (Dijalankan SETELAH upload selesai)
    [](){ 
      server.send(200, "text/html", "<h2>UPLOAD SUKSES!</h2><p>Gambar tersimpan.</p><a href='/'>Kembali</a>"); 
    }, 
    // 2. Handler PROSES UPLOAD (Dijalankan SAAT data dikirim)
    [](){
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        // Hapus file lama, buat baru
        LittleFS.remove("/face.bin"); 
        File file = LittleFS.open("/face.bin", "w");
        file.close();
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        // Tulis data chunk demi chunk
        File file = LittleFS.open("/face.bin", "a");
        if (file) file.write(upload.buf, upload.currentSize);
        file.close();
      } else if (upload.status == UPLOAD_FILE_END) {
        // Saat selesai, reload gambar ke RAM
        loadCustomFace();
        // PENTING: JANGAN ada server.send() di sini! Biarkan Handler 1 yang kerjakan.
      }
    }
  );

  server.on("/exit", HTTP_GET, [](){
    // 1. Kirim pesan terakhir ke Browser
    server.send(200, "text/html", "<h2 style='text-align:center;'>Restarting N.E.K.O...<br>Silakan tutup browser.</h2>");
    delay(500);

    // 2. Bersihkan Layar (Agar tidak terlihat stuck)
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("REBOOTING...");
    display.display();
    delay(1000);

    // 3. Matikan WiFi & Putus Koneksi Total (Penting!)
    WiFi.disconnect(true);  // Hapus kredensial sementara
    WiFi.mode(WIFI_OFF);    // Matikan radio WiFi
    delay(500);             // Beri jeda agar listrik stabil

    // 4. Restart System
    ESP.restart(); 
  });

  server.begin();
}

// ==========================================
// 2. SETUP
// ==========================================

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(IR_SENSOR_PIN, INPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Cek 0x3C / 0x3D
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) for(;;);
  }
  display.clearDisplay(); display.setTextColor(SSD1306_WHITE);

  if(!LittleFS.begin(true)){ Serial.println("FS Error"); }
  loadCustomFace();

  // STARTUP NORMAL
  display.setCursor(0,20);
  display.println("Connecting WiFi...");
  display.display();

  // Coba connect WiFi Rumah (Timeout 5 detik biar cepet)
  WiFiManager wifiManager;
  wifiManager.setConnectTimeout(5); 
  if(!wifiManager.autoConnect("CatBot-Setup")) {
    // Kalau gagal wifi, lanjut aja (Mode Offline)
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

// ==========================================
// 3. LOOP UTAMA
// ==========================================

void loop() {
  // --- LOGIKA TOMBOL BOOT (TEKAN 3 DETIK) ---
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (buttonPressStart == 0) buttonPressStart = millis(); // Mulai hitung
    
    // Tampilkan Loading Bar di Layar
    if (!isAdminMode) {
      display.clearDisplay();
      display.setCursor(0, 10); display.println("HOLD TO ADMIN...");
      long duration = millis() - buttonPressStart;
      int barWidth = map(duration, 0, 3000, 0, 128);
      display.fillRect(0, 30, barWidth, 10, SSD1306_WHITE);
      display.display();
    }

    // Jika sudah ditahan > 3 detik
    if (millis() - buttonPressStart > 3000 && !isAdminMode) {
      enterAdminMode();
    }
  } else {
    buttonPressStart = 0; // Reset jika tombol dilepas
  }

  // --- CABANG MODE ---
  if (isAdminMode) {
    // JIKA MODE ADMIN
    server.handleClient();
    
    // Tampilan Layar Admin
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("== ADMIN MODE ==");
    display.println("");
    display.println("WiFi: Neko-Admin");
    display.println("Pass: 12345678");
    display.println("IP:   192.168.4.1");
    display.display();
    delay(100); // Hemat CPU
    
  } else {
    // JIKA MODE NORMAL (Kucing/Jam)
    if (buttonPressStart == 0) { // Hanya update animasi jika tombol TIDAK ditekan
      runNormalMode();
    }
  }
}

// ==========================================
// 4. FUNGSI LOGIC NORMAL
// ==========================================

void runNormalMode() {
  handleIrInput(); // Cek Sensor IR

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
        if (hasCustomFace) currentMode = MODE_CUSTOM;
        else currentMode = MODE_CAT;
      } else currentMode = MODE_CAT;
      display.clearDisplay(); lastInputTime = millis();
  }
  lastIrState = reading;
}

void drawCustomImage() {
  display.clearDisplay();
  display.drawBitmap(0, 0, customFaceBuffer, 128, 64, SSD1306_WHITE);
  display.display();
}

void drawClock() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    display.clearDisplay(); display.setCursor(0,0);
    display.println("No Internet/Clock"); display.display(); return;
  }
  display.clearDisplay();
  display.setTextSize(3); display.setCursor(20, 15);
  display.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  display.setTextSize(1); display.setCursor(30, 50);
  display.printf("%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  display.display();
}

void drawAnimatedFace() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = currentMillis; blinkInterval = random(2000, 6000); 
    if (random(0, 100) < 20) { isWinking = true; isBlinking = false; winkSide = random(0, 2); } 
    else { isBlinking = true; isWinking = false; }
  }
  if ((isBlinking || isWinking) && (currentMillis - lastBlinkTime > 200)) { isBlinking = false; isWinking = false; }
  display.clearDisplay();
  if (random(0, 100) > 90) pupilOffset = random(-4, 5);
  if (isBlinking || (isWinking && winkSide == 0)) drawClosedEye(40, 32); else drawOpenEye(40, 32, pupilOffset); 
  if (isBlinking || (isWinking && winkSide == 1)) drawClosedEye(88, 32); else drawOpenEye(88, 32, pupilOffset);
  int mX = 64; int mY = 42;
  display.drawLine(mX - 12, mY, mX - 6, mY + 5, SSD1306_WHITE);
  display.drawLine(mX - 6, mY + 5, mX, mY, SSD1306_WHITE);
  display.drawLine(mX, mY, mX + 6, mY + 5, SSD1306_WHITE);
  display.drawLine(mX + 6, mY + 5, mX + 12, mY, SSD1306_WHITE);
  display.display();
}
void drawOpenEye(int x, int y, int offset) {
  display.fillCircle(x, y, 10, SSD1306_WHITE); display.fillCircle(x + offset, y, 4, SSD1306_BLACK); display.drawPixel(x + offset - 1, y - 1, SSD1306_WHITE); 
}
void drawClosedEye(int x, int y) {
  display.drawCircle(x, y, 10, SSD1306_WHITE); display.fillRect(x - 11, y - 12, 23, 13, SSD1306_BLACK);
}
