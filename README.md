# 🐱 N.E.K.O. (Network Embedded Kitty Object)

> **"A Desktop Companion that lives in the Cloud."**

**N.E.K.O.** adalah robot pendamping desktop berbasis ESP32. Versi **Ultimate Edition** ini menggabungkan animasi prosedural, jam internet akurat, dan kontrol nirkabel yang stabil.

Dilengkapi dengan **Unified Admin System**, di mana satu tombol memberikan akses penuh ke menu Upload Gambar dan Reset WiFi tanpa ribet.

![Status](https://img.shields.io/badge/Status-Stable-brightgreen)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![License](https://img.shields.io/badge/License-MIT-orange)

## ✨ Fitur Utama

* **👀 Animasi Hidup:** Mata kucing yang berkedip, melirik, dan mengedip secara acak. Dilengkapi sistem *Anti-Stuck* agar mata tidak pernah macet tertutup.
* **⏰ Smart Clock:** Jam otomatis sinkron dengan internet (WIB/GMT+7).
* **👋 Touchless Control:** Ganti tampilan (Kucing ↔️ Jam ↔️ Custom) dengan melambaikan tangan di sensor IR.
* **🔧 Unified Admin Menu:** Tekan satu tombol untuk membuka dashboard admin:
    * **Upload Gambar:** Ganti wajah kucing dengan pixel art kustom (.bin).
    * **Reset WiFi:** Hapus pengaturan WiFi lama jika kamu pindah rumah/router.
* **🛡️ Fail-Safe System:**
    * **Anti-Hang:** Logika tombol "One-Shot" mencegah ESP32 macet saat tombol ditahan.
    * **Setup Timeout:** Memberi waktu 3 menit untuk setting WiFi sebelum masuk mode Offline.

## 🎮 Cara Penggunaan (Controls)

| Aksi | Cara Melakukan | Fungsi |
| :--- | :--- | :--- |
| **Ganti Tampilan** | Lambaikan tangan di depan Sensor IR | Berpindah mode: **Wajah Kucing** ➡ **Jam** ➡ **Gambar Custom**. |
| **Masuk Menu Admin** | Tekan Tombol Menu (GPIO 15) atau BOOT | Mengaktifkan WiFi `Neko-Admin` untuk upload gambar atau reset WiFi. |

## 🛠️ Wiring Diagram

Pastikan pemasangan kabel sesuai tabel di bawah ini. Untuk tombol menu, gunakan skema *Active Low* (konek ke GND).

| Komponen | Pin Komponen | Pin ESP32 (GPIO) | Catatan |
| :--- | :--- | :--- | :--- |
| **OLED (I2C)** | SDA | **GPIO 21** | |
| | SCL | **GPIO 22** | |
| | VCC | 3.3V | |
| | GND | GND | |
| **Sensor IR** | OUT | **GPIO 19** | Active LOW |
| **Tombol Menu** | Kaki 1 | **GPIO 15** | Tombol Eksternal |
| | Kaki 2 | **GND** | |
| **Tombol BOOT** | - | **GPIO 0** | Tombol Bawaan (Cadangan) |

> **Info:** Tombol Menu (GPIO 15) dan Tombol BOOT (GPIO 0) memiliki fungsi yang sama. Kamu bisa menggunakan salah satunya.

---

## 🎨 Panduan Upload Gambar

Agar gambar muncul sempurna di layar OLED, ikuti langkah konversi ini:

1.  Buka website **[image2cpp](https://javl.github.io/image2cpp/)**.
2.  Upload gambar kamu.
3.  **Image Settings:**
    * Canvas Size: `128` x `64`
    * Background: `Black`
    * Scaling: `Scale to fit`
4.  **Output Settings (PENTING!):**
    * Code output format: **Binary** (Pilih ini agar muncul tombol download).
    * Draw mode: **Horizontal - 1 bit per pixel**.
5.  Klik **Generate** -> **Download file** (Simpan sebagai `.bin`).

### Cara Upload ke N.E.K.O.:
1.  Tekan **Tombol Menu** (GPIO 15) di alat. Layar akan menampilkan "ADMIN MENU".
2.  Connect HP/Laptop ke WiFi: `Neko-Admin` (Pass: `12345678`).
3.  Buka browser: `192.168.4.1`.
4.  Pilih menu **"1. GANTI WAJAH"** -> Upload file `.bin` tadi.
5.  Klik **KELUAR / RESTART**.

---

## 📶 Reset WiFi & Setup Awal

Jika N.E.K.O. tidak bisa terhubung ke WiFi rumah (atau kamu baru pindah lokasi):

1.  **Cara Reset:**
    * Tekan Tombol Menu -> Masuk ke `Neko-Admin` (192.168.4.1).
    * Pilih menu **"2. RESET WIFI SETTINGS"**.
    * Alat akan restart dan melupakan WiFi lama.
2.  **Cara Setup Baru:**
    * Setelah reset, layar akan menampilkan "Connecting..." lalu membuat WiFi baru bernama **`CatBot-Setup`**.
    * Connect ke WiFi tersebut.
    * Halaman konfigurasi akan otomatis muncul (Sign in to network).
    * Pilih WiFi rumah kamu, masukkan password, klik Save.

---

## 📦 Instalasi Kode

1.  Install **Arduino IDE**.
2.  Install Library yang dibutuhkan:
    * `Adafruit GFX`
    * `Adafruit SSD1306`
    * `WiFiManager` (by tzapu)
3.  Buka file `.ino`, pilih Board **DOIT ESP32 DEVKIT V1**.
4.  Upload!

---
*Created with ❤️ by Yasha*
