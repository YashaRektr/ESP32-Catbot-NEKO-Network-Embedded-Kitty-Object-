# 🐱 N.E.K.O. (Network Embedded Kitty Object)

> **"A Desktop Companion that lives in the Cloud."**

**N.E.K.O.** adalah robot pendamping desktop berbasis ESP32 yang memiliki kepribadian digital. Versi ini dilengkapi dengan **Web Dashboard**, memungkinkan kamu mengganti wajah kucing dengan gambar kustom (.bin) secara nirkabel!

![Status](https://img.shields.io/badge/Status-Active-brightgreen)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)

## ✨ Fitur Utama
* **👀 Animasi Ekspresif:** Mata yang berkedip, melirik, dan mengedip secara prosedural.
* **🌐 Web Upload:** Ganti wajah kucing dengan Logo/Foto Pacar lewat browser HP/Laptop.
* **👋 Touchless Control:** Ganti mode (Kucing ↔️ Jam ↔️ Custom) dengan melambaikan tangan di sensor IR.
* **clock Jam Internet:** Sinkronisasi waktu otomatis (WIB).
* **💾 Permanen:** Gambar tersimpan di memori ESP32 meskipun alat dimatikan.

## 🛠️ Hardware & Wiring

| Komponen | Pin | Pin ESP32 (GPIO) |
| :--- | :--- | :--- |
| **OLED (I2C)** | SDA | **GPIO 21** |
| | SCL | **GPIO 22** |
| **Sensor IR** | OUT | **GPIO 19** |
| **Tombol** | Pin | **GPIO 0 (BOOT)** |

---

## 🎨 Cara Upload Gambar Custom (PENTING!)

Agar gambar muncul dengan benar di layar OLED, ikuti pengaturan ini dengan teliti saat mengubah gambar menjadi file `.bin`.

### Langkah 1: Convert Gambar
1. Buka website **[image2cpp](https://javl.github.io/image2cpp/)**.
2. **Select Image:** Upload gambar kamu (JPG/PNG).
3. **Image Settings:**
   * **Canvas Size:** Ubah ke `128` x `64`.
   * **Background:** `Black`.
   * **Scaling:** Pilih `Scale to fit` (agar gambar pas).
4. **Preview:** Pastikan gambar terlihat bagus di kotak preview.

### Langkah 2: Setting Output (JANGAN SALAH PILIH)
Di bagian bawah (Output), atur seperti ini:

* **Code output format:** Ubah menjadi **Binary** (Penting! Jangan pilih Arduino Code/Plain Bytes).
* **Draw mode:** Pilih **Horizontal - 1 bit per pixel** (Pilihan paling atas).
  > ⚠️ *Jangan pilih "2 bytes per pixel" atau "Vertical", nanti gambar rusak!*
* **Tombol Download:**
  * Klik tombol **Generate**.
  * Akan muncul tombol **Download file**. Klik untuk menyimpan file (biasanya bernama `namafile.bin`).

### Langkah 3: Upload ke N.E.K.O.
1. Nyalakan N.E.K.O.
2. Lihat layar OLED, catat **IP Address** yang muncul (contoh: `192.168.1.8`).
3. Buka browser di HP/Laptop (pastikan satu WiFi dengan alat).
4. Ketik IP Address tersebut.
5. Klik **Choose File** -> Pilih file `.bin` tadi -> Klik **Upload**.
6. Selesai! Lambaikan tangan ke sensor IR untuk melihat hasilnya.

---

## 📦 Cara Install (Coding)
1. Install Library di Arduino IDE: `Adafruit GFX`, `Adafruit SSD1306`, `WiFiManager`.
2. Upload file `.ino` ke board ESP32.
3. Saat pertama nyala, connect ke WiFi `CatBot-AP` untuk setting WiFi rumah.

## 🤝 Kontribusi
Project ini Open Source. Silakan fork!

---
*Created with ❤️ by [Nama Kamu]*
