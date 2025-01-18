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

// Konfigurasi WiFi
const char *ssid = "MEDIA MASJID";
const char *password = "31630726";
// Server REST API
const char *server_url = "https://yellowgreen-mule-372794.hostingersite.com/api";
// JSON Document
StaticJsonDocument<200> doc;

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
byte rowPins[ROWS] = {32, 33, 25, 26};
byte colPins[COLS] = {27, 14, 12, 13};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD I2C
LiquidCrystal_I2C lcd(I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// RFID
MFRC522 rfid(SDA_PIN, RST_PIN);
String kode_kartu = "";

// Loadcell HX711
HX711 scale;

// Servo
Servo servo;

// Variabel Global
long saved_tare = 0;
float berat_dibaca = 0;
String input_berat = "";

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
    HTTPClient http;
    http.begin(server_url + endpoint);
    http.addHeader("Content-Type", "application/json");

    Serial.println("Menghubungkan ke Server...");
    Serial.println(server_url + endpoint);
    Serial.println(payload);

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0)
    {
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
  Serial.begin(115200);

  // WiFi
  connectToWiFi();

  // Servo
  servo.attach(SERVO_PIN, 500, 2400);
  Serial.println("Servo siap");
  servo.write(0); // Tutup dispenser

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
  Serial.print("mengambil tare tersimpan: ");
  EEPROM.get(0, saved_tare);
  Serial.println(saved_tare);
  scale.set_offset(saved_tare);
  scale.set_scale(380.f);
  Serial.println("Loadcell Siap");

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
    lcd.setCursor(0, 0);
    lcd.print("Silakan Tap");
    lcd.setCursor(0, 1);
    lcd.print("Kartu Anda     ");
    delay(500);
  }
}

void bukaServo()
{
  servo.write(90); // Buka dispenser
}

void tutupServo()
{
  servo.write(0); // Tutup dispenser
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
      if (key == '*')
      {
        input_berat = ""; // Reset input jika * ditekan
        lcd.setCursor(0, 1);
        lcd.print(input_berat + " gram     ");
      }
      else if (key == '#')
      {
        Serial.println("Input selesai.");
        break; // Keluar dari loop jika # ditekan
      }
      else
      {
        input_berat += key; // Tambahkan angka ke input
        lcd.setCursor(0, 1);
        lcd.print(input_berat + " gram     ");
      }
    }
  }

  // Step 2: Baca kartu RFID
  bacaKartu();
  Serial.println("Kartu: " + kode_kartu);

  // Step 3: Request ke server
  String payload = "{\"id_kartu\":\"" + kode_kartu + "\"}";
  String response = requestToServer("/checkCard", payload);

  Serial.print("Response: ");
  Serial.println(response);

  // Parsing JSON response ke array doc
  DeserializationError error = deserializeJson(doc, response);

  // Periksa apakah parsing berhasil
  if (error)
  {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Ambil data dari doc
  bool success = doc["success"];
  // const char* message = doc["message"];

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

  float saldo_beras = doc["saldo_beras"];

  if (saldo_beras < input_berat.toFloat())
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Saldo Kurang");
    lcd.setCursor(0, 1);
    lcd.print("Sil. Isi Ulang");
    delay(3000);
    return;
  }

  // Step 4: Deteksi wadah
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cek Wadah...");
  if (digitalRead(SENSOR_PIN) == LOW)
  {
    lcd.setCursor(0, 1);
    lcd.print("Wadah belum ada");
    delay(3000);
    return;
  }

  // Step 5: Dispenser beras
  bukaServo();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Beras Keluar...");
  while (true)
  {
    berat_dibaca = scale.get_units();
    lcd.setCursor(0, 1);
    lcd.print(String(berat_dibaca) + " gram   ");
    if (berat_dibaca >= input_berat.toFloat())
    {
      tutupServo();
      break;
    }
  }

  // Step 6: Konfirmasi ke server
  payload = "{\"id_kartu\":\"" + kode_kartu + "\",\"berat\":\"" + input_berat + "\"}";
  requestToServer("/deductBalance", payload);

  // Step 7: Konfirmasi
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Beras diambil:");
  lcd.setCursor(0, 1);
  lcd.print(input_berat + " gram     ");
  delay(3000);
}