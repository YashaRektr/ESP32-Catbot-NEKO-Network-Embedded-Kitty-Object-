# 🐱 N.E.K.O. (Network Embedded Kitty Object)

> **"A Desktop Companion that lives in the Cloud."**

**N.E.K.O.** adalah robot pendamping desktop berbasis ESP32. Proyek ini menggabungkan *Digital Logic* dan *Pixel Art* untuk menciptakan "peliharaan" yang interaktif.

Versi **Ultimate Edition** ini dilengkapi dengan fitur **Anti-Stuck WiFi**, **Easy Admin Mode**, dan penyimpanan gambar permanen.

![Status](https://img.shields.io/badge/Status-Active-brightgreen)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![License](https://img.shields.io/badge/License-MIT-orange)

## ✨ Fitur Utama

* **👀 Animasi Ekspresif:** Mata yang berkedip, melirik, dan mengedip secara acak.
* **⏰ Smart Clock:** Jam otomatis sinkron dengan internet (WIB/GMT+7). Jika WiFi mati, jam tetap berjalan (estimasi).
* **👋 Touchless Control:** Ganti tampilan (Kucing ↔️ Jam ↔️ Custom) dengan melambaikan tangan di sensor IR.
* **📂 Web Upload (Admin Mode):** Upload gambar pixel art kustom (.bin) lewat WiFi langsung dari HP/Laptop.
* **🛡️ Anti-Stuck System:** Jika WiFi rumah mati, N.E.K.O. tidak akan macet/hang. Dia akan otomatis masuk ke **Offline Mode** setelah 10 detik.

## 🎮 Cara Penggunaan (Controls)

| Aksi | Cara Melakukan | Fungsi |
| :--- | :--- | :--- |
| **Ganti Tampilan** | Lambaikan tangan di depan Sensor IR | Berpindah antara Wajah Kucing, Jam Digital, dan Gambar Custom. |
| **Masuk Admin Mode** | Tahan tombol **BOOT** selama **3 Detik** (saat alat menyala) | Mengaktifkan Hotspot `Neko-Admin` untuk upload gambar. |
| **Reset WiFi** | Tahan tombol **BOOT** saat **Baru Dicolok Kabel** | Menghapus data WiFi lama (Factory Reset). |

## 🛠️ Hardware & Wiring

| Komponen | Pin | Pin ESP32 (GPIO) |
| :--- | :--- | :--- |
| **OLED (I2C)** | SDA | **GPIO 21** |
| | SCL | **GPIO 22** |
| **Sensor IR** | OUT | **GPIO 19** |
| **Tombol** | Pin | **GPIO 0 (BOOT)** |

---

## 🎨 Panduan Upload Gambar (PENTING!)

Agar gambar muncul dengan benar dan **tidak error di browser**, ikuti pengaturan ini dengan ketat.

### 1. Convert Gambar
1. Buka website **[image2cpp](https://javl.github.io/image2cpp/)**.
2. Upload gambar (JPG/PNG).
3. **Image Settings:**
   * Canvas Size: `128` x `64`
   * Background: `Black`
   * Scaling: `Scale to fit`

### 2. Output Settings (⚠️ JANGAN SALAH)
Di bagian bawah website image2cpp:
1. **Code output format:** Pilih **Binary** (Bukan Arduino Code).
2. **Draw mode:** Pilih **Horizontal - 1 bit per pixel**.
   * *Jangan pilih "Vertical" atau "2 bytes (565)", gambar akan rusak!*
3. Klik **Generate** -> **Download file** (Simpan sebagai `.bin`).

### 3. Upload ke N.E.K.O.
1. Saat N.E.K.O. menyala, tahan tombol **BOOT** selama 3 detik sampai layar berubah jadi **"ADMIN MODE"**.
2. Hubungkan Laptop/HP ke WiFi: `Neko-Admin` (Password: `12345678`).
3. Buka browser, ketik: `192.168.4.1`.
4. Upload file `.bin` tadi.
5. Klik tombol **Exit/Keluar** di web untuk restart alat.

---

## 📦 Cara Install (Coding)

1.  **Persiapan Arduino IDE:**
    * Install Library via Library Manager:
        * `Adafruit GFX`
        * `Adafruit SSD1306`
        * `WiFiManager` (by tzapu)
2.  **Upload:**
    * Buka file `.ino`.
    * Pilih Board "DOIT ESP32 DEVKIT V1".
    * Upload sketch.
3.  **Setup WiFi Pertama Kali:**
    * Nyalakan alat.
    * Layar akan menampilkan "Connecting WiFi...".
    * Jika gagal (karena belum disetting), dia akan membuat Hotspot `CatBot-Setup`.
    * Konek ke hotspot itu, buka browser, dan masukkan WiFi rumah kamu.

## 🤝 Troubleshooting

* **Layar Stuck di "Connecting WiFi...":**
    * Tunggu 10 detik, dia akan masuk mode Offline otomatis.
* **Browser Error "Corrupted Content":**
    * Pastikan kamu menggunakan kode terbaru dari repo ini (Logic upload sudah diperbaiki).
* **Gagal Upload (Connection Timed Out):**
    * Pastikan kamu connect ke WiFi `Neko-Admin` saat upload, bukan WiFi rumah.
    * Matikan Data Seluler jika upload dari HP.

---
*Created with ❤️ by Yasha*
