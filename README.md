# 🐱 N.E.K.O. (Network Embedded Kitty Object)

> **"A Desktop Companion that lives in the Cloud."**

**N.E.K.O.** adalah robot pendamping desktop berbasis ESP32 yang memiliki kepribadian digital. Proyek ini menggabungkan *Digital Logic* dan *Art* untuk menciptakan "peliharaan" yang interaktif, terhubung ke internet, dan berguna.

![Status](https://img.shields.io/badge/Status-Active-brightgreen)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![License](https://img.shields.io/badge/License-MIT-orange)

## ✨ Fitur Utama

* **👀 Animasi Ekspresif:** Mata yang berkedip, melirik (jitter), dan mengedip (wink) secara acak menggunakan algoritma prosedural.
* **👋 Touchless Control:** Ganti mode tampilan (Wajah/Jam) hanya dengan melambaikan tangan di depan sensor IR.
* **clock Jam Internet Akurat:** Sinkronisasi waktu otomatis via WiFi (NTP Server) dengan zona waktu WIB (GMT+7).
* **📶 WiFi Manager:** Tidak perlu hardcode SSID/Password. Jika WiFi tidak ditemukan, N.E.K.O. akan membuat Hotspot sendiri untuk konfigurasi.
* **🔋 Portable:** Didesain berjalan dengan baterai LiPo (menggunakan modul TP4056 & Boost Converter).

## 🛠️ Hardware yang Dibutuhkan

| Komponen | Deskripsi |
| :--- | :--- |
| **Microcontroller** | ESP32 DevKit V1 |
| **Display** | OLED SSD1306 (0.96 inch, I2C) |
| **Sensor** | IR Obstacle Sensor (Active LOW) |
| **Power** | LiPo Battery 3.7V + TP4056 + DC-DC Boost (to 5V) |
| **Enclosure** | Junction Box / Custom 3D Print |

## 🔌 Wiring Diagram (Pinout)

Pastikan koneksi kabel sesuai dengan tabel di bawah ini:

| Komponen | Pin Komponen | Pin ESP32 (GPIO) |
| :--- | :--- | :--- |
| **OLED Layar** | SDA | **GPIO 21** |
| | SCL | **GPIO 22** |
| | VCC | 3.3V |
| | GND | GND |
| **Sensor IR** | OUT / Signal | **GPIO 19** |
| | VCC | 3.3V / 5V |
| | GND | GND |
| **Tombol Reset** | Pin 1 | **GPIO 0 (BOOT)** |
| | Pin 2 | GND |

> **Catatan:** Tombol Reset (GPIO 0) digunakan untuk mereset pengaturan WiFi. Tahan tombol ini saat N.E.K.O. baru dinyalakan hingga muncul "Loading Bar".

## 📦 Cara Install

1.  **Persiapan Arduino IDE:**
    * Pastikan board **ESP32** sudah terinstall di Board Manager.
    * Install Library berikut melalui *Library Manager*:
        * `Adafruit GFX Library`
        * `Adafruit SSD1306`
        * `WiFiManager` (by tzapu)

2.  **Upload:**
    * Buka file `.ino`.
    * Pilih Board: "DOIT ESP32 DEVKIT V1".
    * Upload ke board.

3.  **Setup Pertama Kali:**
    * Nyalakan N.E.K.O.
    * Layar akan menampilkan **"WiFi Not Found"**.
    * Hubungkan HP/Laptop ke WiFi: `CatBot-AP`.
    * Buka browser (otomatis terbuka) dan masukkan SSID & Password WiFi rumah kamu.
    * Selesai! N.E.K.O. akan tersenyum padamu.

## 📷 Gallery
*(Tempat untuk menaruh foto/GIF N.E.K
