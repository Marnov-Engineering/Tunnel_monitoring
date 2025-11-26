#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <QuickEspNow.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define MAX485_DE_RE  5
#define RS485_RX      16
#define RS485_TX      17

static uint8_t receiver[] = {0x1C, 0x69, 0x20, 0x96, 0x31, 0xD4};
bool sent = true;
const unsigned int SEND_MSG_MSEC = 2000;
#define INTERVAL 1000 // in milliseconds
unsigned long lastReadTime = 0;

unsigned long millis1S = 1000;
unsigned long waktu_kirim = 0;
unsigned long now1S = 0;
unsigned long nowKirim = 0;
bool bWaktunyaKirim = 0;

//==========>BME280
float fTempC = -999;
float fHumidity = -999;
String Last_Time;
float Last_fTempC;
float Last_fHumidity;
Adafruit_BME280 bme;  // I2C
//==========>Pressure & Velocity
float mDifferentialPressure;
float mTemperature;
float mDifferentialPressure2;
float mTemperature2;
float fPressure = -999;
float fVelocity = -999;
float Last_fPressure;
float Last_fVelocity;
bool bRetryInitVelocity;
bool bRetryInitPressure;
//==========>AnemoMeter
float windSpeed1;
float windSpeed2;

uint8_t requestAddress1[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x01 };
uint8_t requestAddress2[] = { 0x02, 0x03, 0x00, 0x00, 0x00, 0x01 };

static uint8_t sensorUjung1[] = {0x10, 0x06, 0x1C, 0x68, 0x34, 0x64};
static uint8_t sensorUjung2[] = {0x10, 0x06, 0x1C, 0x68, 0x19, 0x58};

void dataSent(uint8_t *address, uint8_t status)
{
    sent = true;
    Serial.printf("Message sent to " MACSTR ", status: %d\n", MAC2STR(address), status);
}

void dataReceived(uint8_t *address, uint8_t *data, uint8_t len, signed int rssi, bool broadcast)
{
    Serial.print("Received: ");
    Serial.printf("%.*s\n", len, data);
    Serial.printf("RSSI: %d dBm\n", rssi);
    Serial.printf("From: " MACSTR "\n", MAC2STR(address));
    Serial.printf("%s\n", broadcast ? "Broadcast" : "Unicast");

    // ledState = !ledState;
    // digitalWrite(indicatorLed, ledState ? HIGH : LOW);
    
    windSpeed1 = getSensor(requestAddress1);
    Serial.print("1: ");
    Serial.println(windSpeed1);
    vReadTempBME280();
    vReadPressure();

    String message = String(bme.readTemperature()) + "," + String(windSpeed1) + "," + String(fPressure);
    // sent = false;
    if (!quickEspNow.send(receiver, (uint8_t *)message.c_str(), message.length()))
    {
        Serial.printf(">>>>>>>>>> Message sent\n");
    }
    else
    {
        Serial.printf(">>>>>>>>>> Message not sent\n");
        sent = true; // In case of error we need to set the flag to true to avoid blocking the loop
    }
}

void vKirimData(){
  windSpeed1 = getSensor(requestAddress1);
  Serial.print("1: ");
  Serial.println(windSpeed1);
  vReadTempBME280();
  vReadPressure();

  int rand1 = random(50, 59);
  int rand2 = random(60, 69);
  int rand3 = random(70, 79);

  String message = String(bme.readTemperature()) + "," + String(windSpeed1) + "," + String(fPressure);
  // sent = false;
  if (!quickEspNow.send(receiver, (uint8_t *)message.c_str(), message.length()))
  {
      Serial.printf(">>>>>>>>>> Message sent\n");
      Serial.println(message);
  }
  else
  {
      Serial.printf(">>>>>>>>>> Message not sent\n");
      sent = true; // In case of error we need to set the flag to true to avoid blocking the loop
  }
}

void vPinModeSetup() {
  // pinMode(PIN_IN_OLED_BT, INPUT);
  // pinMode(PIN_MISO_TRIG_STEPUP, OUTPUT);
  // pinMode(PIN_SCK_TRIG_LORA, OUTPUT);
  // pinMode(PIN_MOSI_TRIG_BME_MAX, OUTPUT);
  // // pinMode(PIN_TRIG_OLED,OUTPUT);
  // pinMode(PIN_LED_INDICATOR, OUTPUT);
  // pinMode(PIN_TRIG_SDCARD, OUTPUT);
  // pinMode(PIN_TRIG_OLED,OUTPUT);
  // pinMode(PIN_CS_LORA, OUTPUT);
  // pinMode(PIN_CS_THERMO, OUTPUT);
  // pinMode(PIN_CS_SDCARD, OUTPUT);
  // digitalWrite(PIN_CS_THERMO, HIGH);
  // digitalWrite(PIN_CS_SDCARD, HIGH);
  // digitalWrite(PIN_CS_LORA, HIGH);
  // digitalWrite(PIN_TRIG_OLED, LOW);
}

void vInitBME280() {
  Serial.println(F("BME280 test"));
  bool status;
  // Coba deteksi sensor hingga 3 kali
  for (int attempt = 0; attempt < 3; attempt++) {
    status = bme.begin(0x76);  // Alamat I2C dari BME280
    if (status) {
      Serial.println("BME280 sensor detected!");
      return;  // Keluar dari fungsi jika sensor terdeteksi
    }
    Serial.println("Could not find a valid BME280 sensor, retrying...");
    delay(100);  // Tunggu 1 detik sebelum mencoba lagi
  }
  // Jika sensor tidak terdeteksi setelah 3 kali percobaan
  Serial.println("Could not find a valid BME280 sensor, setting values to NAN.");
  fTempC = NAN;
  fHumidity = NAN;
}

void vReadTempBME280() {
  // fTempC = bme.readTemperature();
  float TempfTempC = bme.readTemperature();
  // fTempC = fCalTempM*TempfTempC + fCalTempB;
  Serial.print("TempBMEC = ");
  Serial.print(TempfTempC);
  Serial.print(" *C");
  // fHumidity = bme.readHumidity();
  float TempfHumidity = bme.readHumidity();
  // fHumidity = fCalHumM*TempfHumidity + fCalHumB;
  Serial.print(", Humidity = ");
  Serial.print(TempfHumidity);
  Serial.println(" %");
}

void vCheckAndReadTemp() {
  //ambil data suhu
  vInitBME280();
  int retry = 0;
  while (fTempC <= -999 && fHumidity <= -999 && retry <= 3) {
    vReadTempBME280();
    retry++;
    Serial.println(retry);
  }
}

void vInitVelocity() {
  Wire.begin();
  int ret = iInitSensorSPD810(1);
  if (ret == 0) {
    Serial.print("iInitSensorSPD810(1): success\n");
    bRetryInitVelocity = 0;
  } else {
    Serial.print("iInitSensorSPD810(1): failed, ret = ");
    Serial.println(ret);
    bRetryInitVelocity = 1;
    // return; // Keluar dari fungsi jika inisialisasi gagal
  }
}

void vReadPressure() {
  int ret = readSample(1);
  float RawPressure = 0;
  if (ret == 0) {
    RawPressure = mDifferentialPressure2;
    Serial.print("RawPressure:");
    Serial.print(RawPressure);
    Serial.println(" Pa");
    if (RawPressure <= 0) {
      RawPressure = 0;
      fPressure = RawPressure;
    } else {
      // fPressure = RawPressure * fCalPresM + fCalPresB;
      // Serial.print("RawPressure:");
      // Serial.print(RawPressure);
      // Serial.println(" Pa");
    }
    Serial.print("fPressure:");
    Serial.print(fPressure);
    Serial.println(" Pa");
    // Serial.print("Temp: ");
    // Serial.print(sdp2.getTemperature());
    // Serial.print("C\n");
  } else {
    // Serial.print("Error in readSample(), ret = ");
    // Serial.println(ret2);
  }
}

void vCheckAndReadVelocity() {
  Serial.println("vCheckAndReadVelocity");
  vInitVelocity();
  int retry = 0;
  while (fVelocity <= -999 && retry <= 3) {
    if (bRetryInitVelocity) {
      vInitVelocity();
    }
    vReadPressure();
    retry++;
    Serial.println("vReadVelocity retry =");
    Serial.println(retry);
  }
  if (fVelocity < 0) {
    fVelocity = 0;
  } else {
    fVelocity = NAN;
  }
}

uint16_t modbusCRC(uint8_t *data, uint8_t len) {
  uint16_t crc = 0xFFFF;
  for (uint8_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x0001)
        crc = (crc >> 1) ^ 0xA001;
      else
        crc = crc >> 1;
    }
  }
  return crc;
}

float getSensor(uint8_t *request){
    // Flush serial buffer
  while (Serial2.available()) Serial2.read();

  // Modbus RTU request: Read Holding Register 0x0000 from device ID 1
  uint16_t crc = modbusCRC(request, 6);
  uint8_t fullRequest[8];
  memcpy(fullRequest, request, 6);
  fullRequest[6] = crc & 0xFF;
  fullRequest[7] = (crc >> 8) & 0xFF;

  // Send request
  digitalWrite(MAX485_DE_RE, HIGH);
  // delayMicroseconds(100); // Let driver stabilize
  Serial2.flush();
  Serial2.write(fullRequest, 8);
  Serial2.flush();
  // delayMicroseconds(800); // Let line settle
  digitalWrite(MAX485_DE_RE, LOW); // Back to receive

  // Wait for sensor to respond (datasheet says > 200 ms)
  delay(250);

  // Read response (expected 7 bytes)
  uint8_t response[7];
  uint8_t i = 0;
  unsigned long start = millis();
  while (i < 7 && (millis() - start) < 500) {
    if (Serial2.available()) {
      response[i] = Serial2.read();
      if(i == 1 && response[i] != 0x03){
        i=0;
      }
      i++;
      if(response[0] != request[0]){
        i = 0;
      }


    }
  }

  // Serial.println();
  // Serial.print("Response: ");
  // for (int j = 0; j < 7; j++) {
  //   Serial.print("0x");
  //   if (response[j] < 0x10) Serial.print("0");
  //   Serial.print(response[j], HEX);
  //   Serial.print(" ");
  // }
  // Serial.println();

  if (i == 7) {
    uint16_t crcResp = modbusCRC(response, 5);
    uint16_t recvCRC = (response[6] << 8) | response[5];

    if (crcResp == recvCRC) {
      uint16_t raw = (response[3] << 8) | response[4];
      float windSpeed = raw * 0.1;
      // Serial.print("Wind Speed: ");
      // Serial.print(windSpeed);
      // Serial.println(" m/s");
      return windSpeed;
    } else {
      Serial.println("❌ CRC Error");
      return -1;
    }
  } else {
    Serial.println("❌ Timeout or incomplete frame");
    return -1;
  }

  // Wait before next poll (must be >200 ms)
  delay(250);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(MAX485_DE_RE, OUTPUT);
  digitalWrite(MAX485_DE_RE, LOW);  // Receive mode

  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect(false, true);

  uint8_t mac[6];
  WiFi.macAddress(mac);

  Serial.printf("Connected to %s in channel %d\n", WiFi.SSID().c_str(), WiFi.channel());
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());

  if (memcmp(mac, sensorUjung1, 6) == 0) {
    waktu_kirim = 800;
    Serial.println("ESP ini = sensorUjung1 → kirim ke target1");
  }
  else if (memcmp(mac, sensorUjung2, 6) == 0) {
    waktu_kirim = 900;
    Serial.println("ESP ini = sensorUjung2 → kirim ke target2");
  }

  quickEspNow.begin(10, 0, false);
  quickEspNow.onDataSent(dataSent);
  quickEspNow.onDataRcvd(dataReceived);

  Serial2.begin(4800, SERIAL_8N1, RS485_RX, RS485_TX);
  Serial.println(" ");
  Serial.println("Alat ON");
  

  vCheckAndReadTemp();
  

}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() - now1S > millis1S){
    windSpeed1 = getSensor(requestAddress1);
    Serial.print("1: ");
    Serial.println(windSpeed1);
    vReadTempBME280();
    now1S = millis();
    nowKirim = millis();
    bWaktunyaKirim = 1;
  }
  
  
  

  if(millis() - nowKirim > waktu_kirim && bWaktunyaKirim){
    
    Serial.println("vKirimData: ");

    vKirimData();
    nowKirim = millis();
    bWaktunyaKirim = 0;
  }
  
}
