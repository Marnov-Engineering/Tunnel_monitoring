#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <QuickEspNow.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Preferences.h>

Preferences preferences;

#define MAX485_DE_RE  5
#define RS485_RX      16
#define RS485_TX      17
#define indicatorLed 5

int sensorDalamID = 0;

static uint8_t receiver[] = {0x1C, 0x69, 0x20, 0x96, 0x31, 0xD4};
int8_t max_tx_power = 80; // 20 dBm
bool sent = true;
const unsigned int SEND_MSG_MSEC = 2000;
#define INTERVAL 1000 // in milliseconds
unsigned long lastReadTime = 0;

unsigned long DuaDetik = 2000;
unsigned long SatuDetik = 1000;
unsigned long waktu_kirim = 0;
unsigned long millisPengiriman = 0;
unsigned long lastAmbilSensor;
unsigned long nowKirim = 0;
static unsigned long lastSend = 0;
bool bWaktunyaKirim = 0;
bool bMulaiLoop = 0;


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
bool ledState;
//==========>AnemoMeter
float windSpeed1;
float windSpeed2;

uint8_t requestAddress1[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x01 };
uint8_t requestAddress2[] = { 0x02, 0x03, 0x00, 0x00, 0x00, 0x01 };

static uint8_t sensorDalam1[] = {0x10, 0x06, 0x1C, 0x68, 0x2A, 0x64}; 
static uint8_t sensorDalam2[] = {0x10, 0x06, 0x1C, 0x68, 0x20, 0xFC};
static uint8_t sensorDalam3[] = {0x10, 0x06, 0x1C, 0x68, 0x34, 0x0C};
static uint8_t sensorDalam4[] = {0x10, 0x06, 0x1C, 0x68, 0x2E, 0x40};
static uint8_t sensorDalam5[] = {0x10, 0x06, 0x1C, 0x68, 0x2F, 0xC0};
static uint8_t sensorDalam6[] = {0x10, 0x06, 0x1C, 0x68, 0x1D, 0x10};
static uint8_t sensorDalam7[] = {0x10, 0x06, 0x1C, 0x68, 0x2F, 0x08};

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

    ledState = !ledState;
    digitalWrite(indicatorLed, ledState ? HIGH : LOW);

    String msg = "";
    for (int i = 0; i < len; i++) {
      msg += (char)data[i];
    }

    Serial.printf("Dari MAC: " MACSTR " -> %s\n", MAC2STR(address), msg.c_str());
    Serial.println("Pesan diterima: " + msg);

    // === Jika pesan "MAC SensDlmX" diterima ===
    if (msg.startsWith("MAC SensDlm")) {
        sensorDalamID = msg.substring(11).toInt(); // ambil angka setelah "MAC SensDlm"
        preferences.putInt("sensorDalamID", sensorDalamID);
        // int idx = msg.substring(11).toInt(); // ambil angka setelah "MAC SensDlm"
        return;
    }

    if (msg.equals("KIRIM SEMUA DATA")) {
      // pakai jadwal waktu_kirim yang sudah ada

      lastSend = millis();
      Serial.printf("lastSend: %lu \n", lastSend);
      // kirimDataDenganJeda();
      // bWaktunyaKirim = 1;
      millisPengiriman = millis();
      bMulaiLoop = 1;
    }
    
    // windSpeed1 = getSensor(requestAddress1);
    // Serial.print("1: ");
    // Serial.println(windSpeed1);
    // vReadTempBME280();

    // String message = String(bme.readTemperature()) + "," + String(windSpeed1);
    // // sent = false;
    // if (!quickEspNow.send(receiver, (uint8_t *)message.c_str(), message.length()))
    // {
    //     Serial.printf(">>>>>>>>>> Message sent\n");
    // }
    // else
    // {
    //     Serial.printf(">>>>>>>>>> Message not sent\n");
    //     sent = true; // In case of error we need to set the flag to true to avoid blocking the loop
    // }

}

void vKirimData(){
  // windSpeed1 = getSensor(requestAddress1);
  // Serial.print("1: ");
  // Serial.println(windSpeed1);
  // vReadTempBME280();

  String message = String(bme.readTemperature()) + "," + String(windSpeed1) + "," + String(bme.readHumidity());
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

void kirimDataDenganJeda() {
  unsigned long now = millis();

  // cek apakah sudah melewati waktu_kirim
  if (bWaktunyaKirim){
    if (now - lastSend >= waktu_kirim) {
      // lastSend = now;

      String message = String(bme.readTemperature()) + "," + String(windSpeed1);
      // sent = false;
      if (!quickEspNow.send(receiver, (uint8_t *)message.c_str(), message.length()))
      {
          Serial.printf(">>>>>>>>>> Message sent\n");
          bWaktunyaKirim = 0;
      }
      else
      {
          Serial.printf(">>>>>>>>>> Message not sent\n");
          sent = true; // In case of error we need to set the flag to true to avoid blocking the loop
          bWaktunyaKirim = 0;
      }
    }
  }
}

void vPengechekanID(){
  int idx = sensorDalamID;
        if (idx >= 1 && idx <= 7) {
            // sensorDalamID = idx;  // simpan ID sensor dalam
            switch (idx) {
                case 1: waktu_kirim = 100; break;
                case 2: waktu_kirim = 200; break;
                case 3: waktu_kirim = 300; break;
                case 4: waktu_kirim = 400; break;
                case 5: waktu_kirim = 500; break;
                case 6: waktu_kirim = 600; break;
                case 7: waktu_kirim = 700; break;
            }
            Serial.printf("ESP ini sekarang = sensorDalam%d, waktu_kirim = %d\n", idx, waktu_kirim);
        } else {
            Serial.println("Nomor sensor dalam tidak valid!");
        }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(MAX485_DE_RE, OUTPUT);
  digitalWrite(MAX485_DE_RE, LOW);  // Receive mode
  pinMode(indicatorLed, OUTPUT);
  preferences.begin("my-app", false);
  sensorDalamID = preferences.getInt("sensorDalamID", 0);

  vPengechekanID();
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect(false, true);

  // uint8_t mac[6];
  // WiFi.macAddress(mac);

  // Serial.printf("MAC ESP ini: %02X:%02X:%02X:%02X:%02X:%02X\n",
  //               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Serial.printf("Connected to %s in channel %d\n", WiFi.SSID().c_str(), WiFi.channel());
  // Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
  // Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
  // esp_wifi_set_max_tx_power(max_tx_power);
  // // Pencocokan MAC
  // if (memcmp(mac, sensorDalam1, 6) == 0) {
  //   waktu_kirim = 100;
  //   Serial.println("ESP ini = sensorDalam1 → kirim ke target1");
  // }
  // else if (memcmp(mac, sensorDalam2, 6) == 0) {
  //   waktu_kirim = 200;
  //   Serial.println("ESP ini = sensorDalam2 → kirim ke target2");
  // }
  // else if (memcmp(mac, sensorDalam3, 6) == 0) {
  //   waktu_kirim = 300;
  //   Serial.println("ESP ini = sensorDalam3 → kirim ke target2");
  // }
  // else if (memcmp(mac, sensorDalam4, 6) == 0) {
  //   waktu_kirim = 400;
  //   Serial.println("ESP ini = sensorDalam4 → kirim ke target2");
  // }
  // else if (memcmp(mac, sensorDalam5, 6) == 0) {
  //   waktu_kirim = 500;
  //   Serial.println("ESP ini = sensorDalam5 → kirim ke target2");
  // }
  // else if (memcmp(mac, sensorDalam6, 6) == 0) {
  //   waktu_kirim = 600;
  //   Serial.println("ESP ini = sensorDalam6 → kirim ke target2");
  // }
  // else if (memcmp(mac, sensorDalam7, 6) == 0) {
  //   waktu_kirim = 700;
  //   Serial.println("ESP ini = sensorDalam7 → kirim ke target3");
  // } else {
  //   Serial.println("MAC tidak terdaftar → tidak kirim");
  // }



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
  if(bMulaiLoop){
    // windSpeed1 = getSensor(requestAddress1);
    // Serial.print("1: ");
    // Serial.println(windSpeed1);
    // vReadTempBME280();
    millisPengiriman = millis();
    nowKirim = millis();
    bWaktunyaKirim = 1;
    delay(waktu_kirim);
    vKirimData();
    bMulaiLoop = 0;
  }

  if(millis() - lastAmbilSensor > SatuDetik){
    lastAmbilSensor = millis();
    windSpeed1 = getSensor(requestAddress1);
    Serial.print("1: ");
    Serial.println(windSpeed1);
    vPengechekanID();
  }


  // kirimDataDenganJeda();
  // if(millis() - nowKirim > waktu_kirim && bWaktunyaKirim){
    
  //   Serial.println("vKirimData: ");

  //   vKirimData();
  //   nowKirim = millis();
  //   bWaktunyaKirim = 0;
  // }
}
