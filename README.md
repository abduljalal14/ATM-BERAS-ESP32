# ATM BERAS ESP32

Proyek firmware ESP32 untuk mesin penyalur beras berbasis kartu RFID, keypad, load cell (HX711), dan servo. Sistem terhubung ke REST API untuk cek kartu dan pemotongan saldo beras.

## Fitur Utama
- Input berat beras (gram) lewat keypad 4x4.
- Verifikasi kartu RFID ke server (`/checkCard`).
- Cek saldo kartu langsung dari keypad.
- Penyaluran beras otomatis menggunakan servo.
- Pengukuran berat real-time dengan load cell HX711.
- Simpan nilai tare dan faktor kalibrasi ke EEPROM.
- Koneksi HTTPS dengan CA certificate (`WiFiClientSecure`).

## Hardware yang Digunakan
- ESP32 DevKit v1 (`esp32doit-devkit-v1`)
- LCD I2C 16x2 (alamat `0x27`)
- Keypad 4x4
- RFID RC522
- HX711 + load cell
- Servo
- Sensor IR (opsional, saat ini logika cek wadah di kode masih dikomentari)

## Mapping Pin
- Servo: `GPIO 15`
- RFID RC522:
  - `RST = GPIO 4`
  - `SDA/SS = GPIO 5`
  - SPI mengikuti pin default ESP32
- LCD I2C:
  - `SDA = GPIO 21`
  - `SCL = GPIO 22`
- HX711:
  - `DT = GPIO 16`
  - `SCK = GPIO 17`
- Sensor IR: `GPIO 34`
- Keypad:
  - Row: `13, 12, 14, 27`
  - Col: `26, 25, 33, 32`

## Dependensi Library
Sudah didefinisikan di `platformio.ini`:
- `marcoschwartz/LiquidCrystal_I2C`
- `chris--a/Keypad`
- `miguelbalboa/MFRC522`
- `bogde/HX711`
- `madhephaestus/ESP32Servo`
- `bblanchon/ArduinoJson`

## Konfigurasi yang Perlu Diubah
Edit di `src/main.cpp`:
- WiFi:
  - `ssid`
  - `password`
- API:
  - `server_url`
  - Header Authorization bearer token pada fungsi `requestToServer()`

## Cara Build dan Upload (PlatformIO)
1. Buka folder proyek ini di VS Code + PlatformIO.
2. Sambungkan board ESP32 ke USB.
3. Build firmware:
   ```bash
   pio run
   ```
4. Upload ke board:
   ```bash
   pio run -t upload
   ```
5. Monitor serial:
   ```bash
   pio device monitor -b 115200
   ```

## Alur Penggunaan di Perangkat
1. Masukkan jumlah gram lewat keypad.
2. Tekan `#` untuk konfirmasi input.
3. Tap kartu RFID.
4. Sistem cek kartu & saldo ke server.
5. Jika saldo cukup, servo membuka dispenser.
6. Berat dibaca dari HX711 sampai target tercapai.
7. Servo menutup, saldo dipotong via API, dan sisa saldo ditampilkan.

## Shortcut Keypad
- `C` : Cek saldo kartu (tanpa dispensing)
- `*` : Hapus input berat
- `#` : Konfirmasi input
- `00001` lalu `#` : Reset tare load cell
- `00002` lalu `#` : Kalibrasi scale (beban referensi 220 gram)

## Catatan Penting
- Logika cek wadah dengan sensor IR saat ini masih dinonaktifkan (bagian kode dikomentari).
- Nilai kalibrasi dan tare disimpan di EEPROM (`512 byte`).
- Untuk keamanan, hindari menyimpan kredensial WiFi dan token API langsung di source code pada deployment produksi.

## Struktur Proyek
- `src/main.cpp` : firmware utama
- `platformio.ini` : konfigurasi board dan library
- `include/`, `lib/`, `test/` : folder standar PlatformIO
