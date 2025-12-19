# 🐱 N.E.K.O. (Network Embedded Kitty Object)

> **"A Desktop Companion that lives in the Cloud."**

**N.E.K.O.** adalah robot pendamping desktop berbasis ESP32 yang memiliki kepribadian digital. Proyek ini menggabungkan *Digital Logic* dan *Art* untuk menciptakan "peliharaan" yang interaktif.

Versi terbaru ini dilengkapi dengan **Web Dashboard**, memungkinkan kamu mengunggah gambar wajah atau logo kustom langsung dari browser HP/Laptop tanpa perlu coding ulang!

![Status](https://img.shields.io/badge/Status-Active-brightgreen)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![License](https://img.shields.io/badge/License-MIT-orange)

## ✨ Fitur Utama

* **👀 Animasi Ekspresif:** Mata yang berkedip, melirik (jitter), dan mengedip (wink) secara prosedural.
* **🌐 Web Dashboard & Upload:** Upload gambar kustom (.bin) lewat browser untuk mengganti wajah kucing.
* **👋 Touchless Control:** Ganti mode (Wajah Kucing ↔️ Jam ↔️ Gambar Custom) dengan melambaikan tangan di depan sensor IR.
* **clock Jam Internet:** Sinkronisasi waktu otomatis via WiFi (NTP Server) zona waktu WIB.
* **💾 LittleFS Storage:** Gambar yang diupload tersimpan permanen di memori ESP32.
* **📶 WiFi Manager:** Membuat Hotspot otomatis jika WiFi rumah tidak ditemukan.

## 🛠️ Hardware

| Komponen | Deskripsi |
| :--- | :--- |
| **Microcontroller** | ESP32 DevKit V1 |
| **Display** | OLED SSD1306 (0.96 inch, I2C) |
| **Sensor** | IR Obstacle Sensor (Active LOW) |
| **Power** | LiPo Battery 3.7V + TP4056 + Boost Converter (5V) |

## 🔌 Wiring Diagram

| Komponen | Pin | Pin ESP32 (GPIO) |
| :--- | :--- | :--- |
| **OLED Layar** | SDA | **GPIO 21** |
| | SCL | **GPIO 22** |
| | VCC | 3.3V |
| | GND | GND |
| **Sensor IR** | OUT | **GPIO 19** |
| | VCC | 3.3V / 5V |
| | GND | GND |
| **Tombol Reset** | Pin 1 | **GPIO 0 (BOOT)** |
| | Pin 2 | GND |

> **Catatan:** Tombol BOOT digunakan untuk mereset WiFi. Tahan tombol saat menyalakan alat.

## 🎨 Cara Upload Gambar Custom (Mudah)

Kamu bisa mengganti wajah kucing dengan Logo, Pixel Art, atau Foto Pacar (versi 1-bit)!

### 1. Siapkan File Gambar (.bin)
Kita perlu mengubah gambar kamu menjadi format binary agar bisa dibaca ESP32.
1. Buka website **[image2cpp](https://javl.github.io/image2cpp/)**.
2. Upload gambar kamu (JPG/PNG).
3. Atur setting berikut:
   * **Canvas Size:** `128` x `64`
   * **Background:** `Black`
   * **Scaling:** `Scale to fit` atau `Stretch`
4. Di bagian **Output** (Penting!):
   * Code format: Pilih **Binary** (Bukan Arduino Code).
   * Klik tombol **Generate** / **Download**.
   * Simpan file dengan nama `face.bin` (opsional, nama apa saja bisa).

### 2. Upload ke N.E.K.O.
1. Nyalakan N.E.K.O.
2. Lihat layar OLED, catat **IP Address** yang muncul (misal: `192.168.1.5`).
3. Buka browser di HP/Laptop (harus satu WiFi dengan N.E.K.O.).
4. Ketik IP Address tersebut.
5. Pilih file `.bin` yang tadi didownload, lalu klik **Upload**.
6. Selesai! Lambaikan tangan ke sensor IR untuk melihat hasilnya.

## 📦 Cara Install (Coding)

1.  **Persiapan:**
    * Install **Arduino IDE**.
    * Install Library via Library Manager:
        * `Adafruit GFX`
        * `Adafruit SSD1306`
        * `WiFiManager` (by tzapu)
2.  **Upload:**
    * Buka file `.ino`.
    * Pilih Board "DOIT ESP32 DEVKIT V1".
    * Upload sketch.
3.  **Setup WiFi:**
    * Saat pertama nyala, connect ke WiFi `CatBot-AP`.
    * Buka browser, masukkan SSID & Password WiFi rumahmu.

## 🤝 Kontribusi
Project ini Open Source. Silakan fork dan kreasikan casing 3D print atau fitur barumu!

---
*Created with ❤️ by [Nama Kamu]*
