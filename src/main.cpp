#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <HX711.h>
#include <ESP32Servo.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// Konfigurasi WiFi
const char *ssid = "MEDIA MASJID";
const char *password = "31630726";
// Server REST API
const char *server_url = "https://yellowgreen-mule-372794.hostingersite.com/api";
const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n"
"MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB\n"
"iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n"
"cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n"
"BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw\n"
"MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV\n"
"BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU\n"
"aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy\n"
"dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK\n"
"AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B\n"
"3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY\n"
"tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/\n"
"Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2\n"
"VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT\n"
"79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6\n"
"c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT\n"
"Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l\n"
"c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee\n"
"UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE\n"
"Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd\n"
"BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G\n"
"A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF\n"
"Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO\n"
"VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3\n"
"ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs\n"
"8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR\n"
"iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze\n"
"Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ\n"
"XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/\n"
"qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB\n"
"VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB\n"
"L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG\n"
"jjxDah2nGN59PRbxYvnKkKj9\n"
"-----END CERTIFICATE-----\n";

WiFiClientSecure client;


// JSON Document
StaticJsonDocument<200> doc;
StaticJsonDocument<200> doc2;

// Konfigurasi Pin
#define SERVO_PIN 15
#define RST_PIN 4
#define SDA_PIN 5
#define I2C_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define SENSOR_PIN 34
#define DT_PIN 16
#define SCK_PIN 17

// Keypad konfigurasi
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
// byte rowPins[ROWS] = {32, 33, 25, 26};
// byte colPins[COLS] = {27, 14, 12, 13};

byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD I2C
LiquidCrystal_I2C lcd(I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// RFID
MFRC522 rfid(SDA_PIN, RST_PIN);
String kode_kartu = "";

// Loadcell HX711
HX711 scale;
// EEPROM address untuk menyimpan offset tare
#define EEPROM_TARE_ADDRESS 0
#define EEPROM_SCALE_ADDRESS 10
// Servo
Servo servo;

// Variabel Global
float berat_dibaca = 0;
String input_berat = "";

String getUsedEEPROMAddresses() {
  String usedAddress = "";
  int usedAddresses[EEPROM.length()];
  int count = 0;  // Variabel untuk menghitung jumlah alamat yang terisi

  // Membaca seluruh EEPROM dan memeriksa apakah ada data yang terisi
  for (int i = 0; i < EEPROM.length(); i++) {
    int content = (int) EEPROM.read(i);  // Baca nilai di EEPROM
    if (content != 255) {  // 255 berarti belum ada data yang ditulis
      usedAddresses[count] = i;  // Menyimpan alamat EEPROM yang terisi
      count++;  // Increment count untuk menyimpan alamat selanjutnya
    }
  }
  // Membuat string untuk menampilkan alamat-alamat yang terpakai
  usedAddress += "[";

  // Menggabungkan semua alamat yang terpakai dalam satu string
  for (int i = 0; i < count; i++) {
    usedAddress += String(usedAddresses[i]);
    if (i < count - 1) {
      usedAddress += ", ";  // Menambahkan koma hanya jika bukan elemen terakhir
    }
  }
  
  usedAddress += "]";

  return usedAddress;
}

void connectToWiFi()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  Serial.println("Menghubungak ke WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    lcd.setCursor(0, 1);
    lcd.print("Retrying...");
    Serial.println("Menghubungak ulang ke WiFi...");
  }
  Serial.println("WiFi terhubung");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP().toString());
  delay(2000);
}

String requestToServer(String endpoint, String payload)
{
  if (WiFi.status() == WL_CONNECTED)
  {

    client.setCACert(root_ca);
    // client.setInsecure(); // Abaikan validasi SSL
    HTTPClient http;
    http.begin(client, server_url + endpoint);
    
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Accept-Encoding", "identity");
    http.addHeader("User-Agent", "PostmanRuntime/7.43.0");
    http.addHeader("Authorization", "Bearer buatkode");

    http.setTimeout(15000);

    Serial.println("Menghubungkan ke Server...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Menghubungkan");
    lcd.setCursor(0, 1);
    lcd.print("ke Server...");
    Serial.println(server_url + endpoint);
    Serial.println(payload);

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0)
    {
      String remaining = http.header("x-ratelimit-remaining");
      Serial.println("Sisa kuota: " + remaining);
      String response = http.getString();
      Serial.println("Response: " + response);
      http.end();
     
      return response;
    }
    else
    {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
      http.end();
      return "";
    }
  }
  else
  {
    Serial.println("WiFi Disconnected");
    return "";
  }
}

void setup()
{
  Serial.println("MEMULAI SETUP...");
  Serial.begin(115200);

  // WiFi
  connectToWiFi();

  // Servo
  servo.attach(SERVO_PIN, 500, 2400);
  Serial.println("Servo siap");
  servo.write(90); // Tutup dispenser

  // RFID
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID siap");

  // LCD
  Wire.begin(21, 22);
  lcd.init();
  Serial.println("LCD Siap");
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("ATM BERAS READY");
  delay(2000);

  // Loadcell
  scale.begin(DT_PIN, SCK_PIN);
  // Tunggu beberapa saat agar HX711 stabil
  Serial.println("Tunggu HX711 stabilisasi...");
  delay(2000);
  // Kalibrasi awal
  if (scale.is_ready())
  {
    Serial.println("HX711 siap digunakan.");
  }
  else
  {
    Serial.println("HX711 tidak terhubung. Periksa koneksi.");
    while (true)
      ;
  }

  // Inisialisasi EEPROM
  EEPROM.begin(512);
  Serial.println("EEPROM inisialisasi selesai.");
    // Mencetak alamat EEPROM yang sudah terpakai
  String usedAddres =  getUsedEEPROMAddresses();
  Serial.println("Used EEPROM Address: "+usedAddres);

  // Muat tare dari EEPROM
  long saved_tare = 0;
  EEPROM.get(EEPROM_TARE_ADDRESS, saved_tare);
  Serial.println("Memuat EEPROM Saved Tare: "+String(saved_tare));

    // Memeriksa apakah sudah ada nilai kalibrasi di EEPROM
  float saved_scale;
  EEPROM.get(EEPROM_SCALE_ADDRESS, saved_scale);
  Serial.println("Memuat EEPROM Saved Scale: "+String(saved_scale));

  scale.set_offset(saved_tare);
  scale.set_scale(saved_scale);
  // Sensor IR
  pinMode(SENSOR_PIN, INPUT);
}

void bacaKartu()
{
  Serial.println("Mendeteksi Kartu...");
  while (true)
  {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
    {
      kode_kartu = "";
      for (byte i = 0; i < rfid.uid.size; i++)
      {
        kode_kartu += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
        kode_kartu += String(rfid.uid.uidByte[i], HEX);
      }
      rfid.PICC_HaltA();
      break;
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Silakan Tap");
    lcd.setCursor(0, 1);
    lcd.print("Kartu Anda     ");
    delay(500);
  }
}

void bukaServo()
{
  servo.write(0); // Buka dispenser
}

void tutupServo()
{
  servo.write(90); // Tutup dispenser
}

void cekSaldo()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cek Saldo Kartu");
  Serial.println("Cek Saldo Kartu...");

  // Baca kartu
  bacaKartu();

  // Kirim permintaan ke server
  String payload = "{\"card_code\":\"" + kode_kartu + "\"}";
  String response = requestToServer("/checkCard", payload);

  // Parsing JSON response
  DeserializationError error = deserializeJson(doc, response);
  if (error)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cek saldo gagal");
    lcd.setCursor(0, 1);
    lcd.print("Coba lagi nanti");
    Serial.println("Error parsing JSON: " + String(error.c_str()));
    delay(2000);
    return;
  }

  // Ambil saldo beras dari respons
  bool success = doc["success"];
  if (!success)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Belum Terdaftar");
    lcd.setCursor(0, 1);
    lcd.print(kode_kartu);
    delay(3000);
    return;
  }

  String name = doc["name"];
  float saldo_beras = doc["saldo_beras"];
  int saldo_kg = (int)saldo_beras/1000;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(name);
  lcd.setCursor(0, 1);
  lcd.print("Saldo: "+String(saldo_beras/1000) + " KG");
  Serial.println("Saldo Beras: " + String(saldo_beras));
  delay(3000);
}

void resetTare()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tare Loadcell...");
  Serial.println("Melakukan tare loadcell...");
  scale.tare(); // Tare loadcell
  // Simpan tare ke EEPROM
  long tare_offset = scale.get_offset();
  EEPROM.put(EEPROM_TARE_ADDRESS, tare_offset);
  EEPROM.commit();
  // float coba = 0;
  // EEPROM.get(EEPROM_TARE_ADDRESS,coba);
  // Serial.println("Skala dari EEPROM: "+ String(coba));

  Serial.print("Tare disimpan: ");
  Serial.println(tare_offset);

  lcd.setCursor(0, 1);
  lcd.print("Tare Selesai");
  Serial.println("Tare selesai, offset: " + String(tare_offset));
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Masukan Jumlah:");
}
void kalibrasiScale()
{
  float calibration_factor = 220; // Sesuaikan dengan kebutuhan
  scale.set_scale(calibration_factor);
  Serial.println("Kalibrasi Loadcell");
  Serial.println("Silakan taruh beban di atas loadcell untuk kalibrasi");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kalibrasi Loadcell...");
  lcd.setCursor(0, 1);
  lcd.print("Letakan Beban 220");

  // Tunggu beberapa detik untuk stabilisasi
  delay(5000);


  // Hitung rata-rata nilai pembacaan beban untuk kalibrasi
  float sum = 0;
  int count = 50;
  Serial.print("hitung Rata-rata mulai");
  for (int i = 0; i < count; i++) {
    sum += scale.get_units(10); // Ambil rata-rata pembacaan
    delay(50);
  }
  Serial.print("hitung Rata-rata selesai");
  
  float averageReading = sum / count;
  Serial.print("Rata-rata pembacaan: ");
  Serial.println(averageReading);
  
  // Menyimpan factor kalibrasi ke EEPROM (alamat 10)
  EEPROM.begin(512); // Ukuran EEPROM (512 byte)
  EEPROM.put(10, averageReading);
  EEPROM.commit();
  Serial.print("Faktor kalibrasi disimpan di EEPROM alamat 10: ");
  Serial.println(averageReading);
  
  lcd.setCursor(0, 1);
  lcd.print("Kalib. Selesai");

  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Masukan Jumlah:");
}


void loop()
{
  // Step 1: Masukan jumlah berat
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Masukan Jumlah:");
  lcd.setCursor(0, 1);
  lcd.print(input_berat + " gram     ");

  Serial.println("Proses input gram beras...");
  char key = keypad.getKey();

  while (true)
  {
    key = keypad.getKey();
    if (key)
    {
      // Cek saldo jika tombol "C" ditekan
      if (key == 'C')
      {
        cekSaldo();
        return;
      }

      // else if (key == 'D')
      // {
      //   resetTare();
      // }

      else if (key == '*')
      {
        input_berat = ""; // Reset input berat
        return;           // Kembali ke awal loop
      }
      else if (key == '#')
      {
        if (input_berat== "00001")
        {
          resetTare();
          input_berat = "";
          lcd.setCursor(0, 1);
          lcd.print(input_berat + " gram     ");
        }
        else if (input_berat== "00002")
        {
          kalibrasiScale();
          input_berat = "";
          lcd.setCursor(0, 1);
          lcd.print(input_berat + " gram     ");
        }
        else{
        Serial.println("Input selesai.");
        break; // Keluar dari loop jika # ditekan
        }
      }
      else
      {
        if (key != 'A' && key != 'B' && key != 'D')
        {
          input_berat += key; // Tambahkan angka ke input
        lcd.setCursor(0, 1);
        lcd.print(input_berat + " gram     ");
        Serial.print("Input Beras: ");
        Serial.println(input_berat);
        }
        
        
      }
    }
  }

  // Step 2: Baca kartu RFID
  bacaKartu();
  Serial.println("Kartu: " + kode_kartu);

  // Step 3: Request ke server
  String payload = "{\"card_code\":\"" + kode_kartu + "\"}";
  String response = requestToServer("/checkCard", payload);

  // Parsing JSON response ke array doc
  Serial.print("Parsing JSON to Array...");
  DeserializationError error = deserializeJson(doc, response);

  // Periksa apakah parsing berhasil
  if (error)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Gagal Koneksi");
    lcd.setCursor(0, 1);
    lcd.print("Sil. Coba Lagi");
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
    delay(3000);
    return;
  }
  // Ambil data dari doc
  bool success = doc["success"];
  // const char* message = doc["message"];

  if (!success)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    Serial.println("Kartu belum terdaftar");
    lcd.print("Belum Terdaftar");
    lcd.setCursor(0, 1);
    lcd.print(kode_kartu);
    delay(3000);
    return;
  }

  float saldo_beras = doc["saldo_beras"];
  Serial.print("Saldo Beras: ");
  Serial.println(saldo_beras);
  Serial.print("Input Beras: ");
  Serial.println(input_berat);

  if (saldo_beras < input_berat.toFloat())
  {
    Serial.println("Saldo Kurang");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Saldo Kurang");
    lcd.setCursor(0, 1);
    lcd.print("Sil. Isi Ulang");
    delay(3000);
    return;
  }

  // Step 4: Deteksi wadah
  // lcd.clear();
  // lcd.setCursor(0, 0);
  // Serial.println("Cek Wadah...");
  // lcd.print("Cek Wadah...");
  // if (digitalRead(SENSOR_PIN) == HIGH)
  // {
  //   lcd.setCursor(0, 1);
  //   Serial.println("Wadah belum ada");
  //   lcd.print("Wadah belum ada");
  //   delay(3000);
  //   return;
  // }

  // Step 6: Konfirmasi ke server
  payload = "{\"card_code\":\"" + kode_kartu + "\",\"berat\":\"" + input_berat + "\"}";
  String response2 = requestToServer("/deductBalance", payload);

  // Step 5: Dispenser beras
  Serial.println("Membuka Servo");
  bukaServo();
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println("Beras Keluar....");
  lcd.print("Beras Keluar...");
  while (true)
  {
    berat_dibaca = scale.get_units();
    Serial.println(berat_dibaca);
    lcd.setCursor(0, 1);
    lcd.print(String((int)berat_dibaca) + " gram   ");
    if (berat_dibaca >= input_berat.toFloat())
    {
      tutupServo();
      break;
    }
  }

  

  // Parsing JSON response

  // Step 7: Konfirmasi
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Beras diambil:");
  lcd.setCursor(0, 1);
  lcd.print(input_berat + " gram     ");
  delay(3000);
  // Step 8: Cek Sisa Saldo
  DeserializationError error2 = deserializeJson(doc2, response2);
  if (error)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cek saldo gagal");
    lcd.setCursor(0, 1);
    lcd.print("Coba lagi nanti");
    Serial.println("Error parsing JSON: " + String(error.c_str()));
    delay(2000);
    return;
  }

  String name = doc["name"];
  float sisa_saldo = doc2["saldo"];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(name);
  lcd.setCursor(0, 1);
  lcd.print("Sisa: "+String(sisa_saldo/1000) + " KG");
  Serial.println("Sisa: " + String(sisa_saldo));
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Semoga Manfaat");
  delay(3000);
  
  input_berat = "";
}