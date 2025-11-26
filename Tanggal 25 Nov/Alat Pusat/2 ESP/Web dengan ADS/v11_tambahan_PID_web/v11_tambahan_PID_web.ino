#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <QuickEspNow.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
// #include "MainWebServer.h"
// #include "ControlingWebserver.h"
// #include "SDcardFunctions.h"
#include <Adafruit_ADS1X15.h>
#include <ESP32Servo.h>
#include <Preferences.h>
#include <nvs_flash.h>
#include <Update.h>
// #include <HTTPClient.h>
#include <CRC32.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <ArduinoHttpClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define VERSION 1

Preferences preferences;

// const char* ssid = "Marnov";
// const char* password = "jujurdanamanah";

const char* ssid = "Web Tunnel Monitoring";
const char* password = "12345678";

// waktu
WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 0;
unsigned long waktuUpdate = 0;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


// var baru

const char *firmwareURL = "https://firebasestorage.googleapis.com/v0/b/otastorage-503b9.appspot.com/o/Sigapp_sensor.bin?alt=media";

int setPointPower = 0;
// float Kecepatan = 0;
// float Arus = 0.00000;
// float calM = 1.00000;
// float calB = 0.00000;
// int Safety = true;
bool Update_OTA = 0;
bool isSdCardLogging = 0;
int currentYear;
int currentMonth;
int currentDay;
char MergedMacAddress[13];
#define CHUNCK_SIZE 256

/************** Char to porgmem ***********************/
// static const char tail[] PROGMEM = "\r\n--SNoveLab--\r\n";
// static const char headData[] PROGMEM = "\r\n--SNoveLab\r\nContent-Disposition: form-data; name=\"CRC32\"\r\n\r\n";
// const char server_tambang[] = "asia-east1-tambang-2b501.cloudfunctions.net";
// const int port = 443;
bool bSedangKirimData = 0;
const int serverPort = 443; //443 for HTTPS
uint32_t fileLen;
char getResponse[2048];   // sesuaikan ukuran dengan response JSON yang kamu dapat
int getResponseIndex = 0; // panjang data valid dalam getResponse
bool bOTAStart = 0;
int timeToReport;
bool bstopLoop = 0;
// WiFiClientSecure client;  

// HttpClient http(client, server_tambang, port);

char serverName[256] = "asia-east1-tambang-2b501.cloudfunctions.net";   // REPLACE WITH YOUR Server IP ADDRESS

char serverPath[256] = "/Tambang/periodic-csv/1C692094D05C?feedback=all";      // ServerPath /upload or /file-upload

struct SensorData {
  String id;
  String name;
  float calTempM;
  float calTempB;
  float calHumM;
  float calHumB;
  float calPresM;
  float calPresB;
  float calVelM;
  float calVelB;
  float calBat;
};

WiFiClientSecure client;

const int MAX_SENSOR = 9;   // sesuaikan dengan jumlah sensor maksimum
SensorData sensorsData[MAX_SENSOR];
int sensorCount = 0;         // jumlah sensor terisi


String suhu1;
String kec1;
String suhu2;
String kec2;
String suhu3;
String kec3;
String suhu4;
String kec4;
String suhu5;
String kec5;
String suhu6;
String kec6;
String suhu7;
String kec7;

String Usuhu1;
String Ukec1;
String USDP1;
String Usuhu2;
String Ukec2;
String USDP2;

String SuhuData;
String WindData;
String TekananData;
String TunnelData;
String SuhuPipaData;
String SuhuDindingData;
String OutPID;
String GabunganSuhu;
//

#define RX_PIN 16
#define TX_PIN 17

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
// Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

AsyncWebServer server(80);
Servo servo1, servo2, servo3, servo4;

static const int servoPin1 = 13;
static const int servoPin2 = 26;
static const int servoPin3 = 27;
static const int servoPin4 = 25;

int Power_Tunnel_1 ;
int Heater_st_1 ;
int Fan_Power_1 ;
int Suhu_Tunnel_1 = 0;

int Power_Tunnel_2 ;
int Heater_st_2 ;
int Fan_Power_2 ;
int Suhu_Tunnel_2 = 0;

int Power_Tunnel_3 ;
int Heater_st_3 ;
int Fan_Power_3 ;
int Suhu_Tunnel_3 = 0;

int Safety = 0 ;
int Heater_st = 0;
int Suhu_Tunnel = 0;
int reset_arr = 0;
int Power_Tunnel;
int Fan_Power;
int safety;
int CounterWIFI;

bool WCSstate1;
bool WCSstate2;
bool WCSstate3;

float  BME1;
float  BME2;
float  BME3;

float CalM_DinD1 = 1 ;
float CalB_DinD1 = 0 ;

float CalM_DinD2 = 1 ;
float CalB_DinD2 = 0;

float CalM_DinD3 = 1 ;
float CalB_DinD3 = 0 ;

float SuhuDS1;
float SuhuDS2;
float SuhuDS3;

int Watt1 = 0 ;
int Watt2 = 0 ;
int Watt3 = 0 ;
int Watt4 = 0 ;

int Arus1 ;
int Arus2 ;
int Arus3 ;
int Arus4 ;

int kecepatan1 = 21;
int kecepatan2 = 22;
int kecepatan3 = 23;
int kecepatan4 = 24;

float calM_Wcs1 =  0.004351 ;
float calB_Wcs1 =  - 85.004 ;

float calM_Wcs2 = 0.004351 ;
float calB_Wcs2 = - 85.004 ;

float calM_Wcs3 =  0.004351 ;
float calB_Wcs3 =  - 85.004 ;

float calM_Wcs4 =  0.004351 ;
float calB_Wcs4 =  - 85.004 ;

float DindSuhu1 = 81;
float DindSuhu2 = 82;
float DindSuhu3 = 83;

float setpoint1, setpoint2, setpoint3;
float kp1, kp2, kp3;
float ki1, ki2, ki3;
float kd1, kd2, kd3;
float out_pid1, out_pid2, out_pid3;

String SData;
String SDataHeat;

unsigned long ADSMillis;
unsigned long IntervalADSMillis = 1000;

unsigned long CopyFileMillis;
unsigned long IntervalCopyFileMillis = 60000;

unsigned long KirimServerMillis;
unsigned long IntervalKirimServerMillis = 130000;

static const String msg = "send cmd";
const unsigned int SEND_MSG_MSEC = 50;

// MAC addresses
static uint8_t sensorDalam1[] = {0x10, 0x06, 0x1C, 0x68, 0x2A, 0x64}; 
static uint8_t sensorDalam2[] = {0x10, 0x06, 0x1C, 0x68, 0x20, 0xFC};
static uint8_t sensorDalam3[] = {0x10, 0x06, 0x1C, 0x68, 0x34, 0x0C};
static uint8_t sensorDalam4[] = {0x10, 0x06, 0x1C, 0x68, 0x2E, 0x40};
static uint8_t sensorDalam5[] = {0x10, 0x06, 0x1C, 0x68, 0x2F, 0xC0};
static uint8_t sensorDalam6[] = {0x10, 0x06, 0x1C, 0x68, 0x1D, 0x10};
static uint8_t sensorDalam7[] = {0x10, 0x06, 0x1C, 0x68, 0x2F, 0x08};

static uint8_t sensorUjung1[] = {0x10, 0x06, 0x1C, 0x68, 0x34, 0x64};
static uint8_t sensorUjung2[] = {0x10, 0x06, 0x1C, 0x68, 0x19, 0x58};

static uint8_t sensorDinding1[] = {0x68, 0xC6, 0x3A, 0xF6, 0xA7, 0x2B};
static uint8_t sensorDinding2[] = {0x84, 0xCC, 0xA8, 0x98, 0xA6, 0xC8};
static uint8_t sensorDinding3[] = {0x8C, 0xAA, 0xB5, 0x50, 0xBE, 0xCE};

// static uint8_t dust13[] = {0xFC, 0xF5, 0xC4, 0xA6, 0xEA, 0x75};
// static uint8_t dust14[] = {0xE8, 0x68, 0xE7, 0xC7, 0xFB, 0x56};
// static uint8_t wind15[] = {0x10, 0x06, 0x1C, 0x68, 0x33, 0xA4};
// static uint8_t wind16[] = {0x3C, 0x8A, 0x1F, 0xA3, 0xEF, 0x8C};
// static uint8_t wind17[] = {0x10, 0x06, 0x1C, 0x68, 0x29, 0xC8};
// static uint8_t wind18[] = {0x10, 0x06, 0x1C, 0x68, 0x1B, 0xDC}; 
// static uint8_t valve20[] = {0x1C, 0x69, 0x20, 0x96, 0xD6, 0x60};

const uint8_t RECEIVER_COUNT = 12;

static uint8_t* receivers[RECEIVER_COUNT] = {
  sensorDalam1, sensorDalam2, sensorDalam3, sensorDalam4, sensorDalam5,
  sensorDalam6, sensorDalam7,
  sensorUjung1, sensorUjung2,
  sensorDinding1,sensorDinding2,sensorDinding3
};

// Jumlah data yang dikirim setiap node
const uint8_t dataPerNode[RECEIVER_COUNT] = {
  2, 2, 2, 2, 2, 2, 2,   // 7 sensorDalam → 2 data per node
  3, 3, 1, 1, 1                   // 2 sensorUjung → 3 data per node
};


String data[62];  // Stores data1 to data20

bool sent = true;

void vSetupEEPROM(){
  preferences.begin("info-umum", false);
  calM_Wcs1  = preferences.getFloat("calM_Wcs1", 0);
  calB_Wcs1  = preferences.getFloat("calB_Wcs1", 0);
  calM_Wcs2  = preferences.getFloat("calM_Wcs2", 0);
  calB_Wcs2  = preferences.getFloat("calB_Wcs2", 0);
  calM_Wcs3  = preferences.getFloat("calM_Wcs3", 0);
  calB_Wcs3  = preferences.getFloat("calB_Wcs3", 0);
  calM_Wcs4  = preferences.getFloat("calM_Wcs4", 0);
  calB_Wcs4  = preferences.getFloat("calB_Wcs4", 0);
  // CalM_DinD1 =  preferences.getFloat("CalM_DinD1", 0);
  // CalB_DinD1 =  preferences.getFloat("CalB_DinD1", 0);
  // CalM_DinD2 =  preferences.getFloat("CalM_DinD2", 0);
  // CalB_DinD2 =  preferences.getFloat("CalB_DinD2", 0);
  // CalM_DinD3 =  preferences.getFloat("CalM_DinD3", 0);
  // CalB_DinD3 =  preferences.getFloat("CalB_DinD3", 0);
  Suhu_Tunnel = preferences.getInt("Suhu_Tunnel", 0);
  // timeToReport = preferences.getInt("timeReportPeriodic", 0);
  

  // loadSensorData(0, sensorsData[0]);
  // loadSensorData(1, sensorsData[1]);
  // loadSensorData(2, sensorsData[2]);
  // loadSensorData(3, sensorsData[3]);
  // loadSensorData(4, sensorsData[4]);
  // loadSensorData(5, sensorsData[5]);
  // loadSensorData(6, sensorsData[6]);
  // loadSensorData(7, sensorsData[7]);
  // loadSensorData(8, sensorsData[8]);
  
  float suhuReal1 = (sensorsData[0].calTempM * data[0].toFloat()) + sensorsData[0].calTempB;
  float suhuReal2 = (sensorsData[1].calTempM * data[2].toFloat()) + sensorsData[1].calTempB;
  float suhuReal3 = (sensorsData[2].calTempM * data[4].toFloat()) + sensorsData[2].calTempB;
  float suhuReal4 = (sensorsData[3].calTempM * data[6].toFloat()) + sensorsData[3].calTempB;
  float suhuReal5 = (sensorsData[4].calTempM * data[8].toFloat()) + sensorsData[4].calTempB;
  float suhuReal6 = (sensorsData[5].calTempM * data[10].toFloat()) + sensorsData[5].calTempB;
  float suhuReal7 = (sensorsData[6].calTempM * data[12].toFloat()) + sensorsData[6].calTempB;
  float suhuReal8 = (sensorsData[7].calTempM * data[14].toFloat()) + sensorsData[7].calTempB;
  float suhuReal9 = (sensorsData[8].calTempM * data[17].toFloat()) + sensorsData[8].calTempB;

  Serial.println("suhuReal1  =");
  Serial.println(suhuReal1);
  Serial.println("suhuReal2 =");
  Serial.println(suhuReal2);
  Serial.println("suhuReal3  =");
  Serial.println(suhuReal3);
  Serial.println("suhuReal4 =");
  Serial.println(suhuReal4);
  Serial.println("suhuReal5  =");
  Serial.println(suhuReal5);
  Serial.println("suhuReal6 =");
  Serial.println(suhuReal6);
  Serial.println("suhuReal7  =");
  Serial.println(suhuReal7);
  Serial.println("suhuReal8 =");
  Serial.println(suhuReal8);
  Serial.println("suhuReal9  =");
  Serial.println(suhuReal9);
}

void ensureNvsReady() {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    nvs_flash_init();
  }
}

void saveSensorData(int index, const SensorData &sensorDataTemp) {
  Preferences perfs;
  perfs.begin("info-umum", false);  // namespace "info-umum"

  String keyBase = "s" + String(index) + "_"; // contoh: s0_calTempM

  perfs.putFloat((keyBase + "calTempM").c_str(), sensorDataTemp.calTempM);
  perfs.putFloat((keyBase + "calTempB").c_str(), sensorDataTemp.calTempB);
  perfs.putFloat((keyBase + "calHumM").c_str(),  sensorDataTemp.calHumM);
  perfs.putFloat((keyBase + "calHumB").c_str(),  sensorDataTemp.calHumB);
  perfs.putFloat((keyBase + "calPresM").c_str(), sensorDataTemp.calPresM);
  perfs.putFloat((keyBase + "calPresB").c_str(), sensorDataTemp.calPresB);
  perfs.putFloat((keyBase + "calVelM").c_str(),  sensorDataTemp.calVelM);
  perfs.putFloat((keyBase + "calVelB").c_str(),  sensorDataTemp.calVelB);
  perfs.putFloat((keyBase + "calBat").c_str(),   sensorDataTemp.calBat);

  perfs.putString((keyBase + "id").c_str(),   sensorDataTemp.id);
  perfs.putString((keyBase + "name").c_str(), sensorDataTemp.name);
  Serial.println("data di save");
  perfs.end();
}

void loadSensorData(int index, SensorData &sensorDataTemp) {
  Preferences perfs;
  perfs.begin("info-umum", true); // read-only

  String keyBase = "s" + String(index) + "_";

  sensorDataTemp.calTempM = perfs.getFloat((keyBase + "calTempM").c_str(), 1.0f);
  sensorDataTemp.calTempB = perfs.getFloat((keyBase + "calTempB").c_str(), 0.0f);
  sensorDataTemp.calHumM  = perfs.getFloat((keyBase + "calHumM").c_str(),  1.0f);
  sensorDataTemp.calHumB  = perfs.getFloat((keyBase + "calHumB").c_str(),  0.0f);
  sensorDataTemp.calPresM = perfs.getFloat((keyBase + "calPresM").c_str(), 1.0f);
  sensorDataTemp.calPresB = perfs.getFloat((keyBase + "calPresB").c_str(), 0.0f);
  sensorDataTemp.calVelM  = perfs.getFloat((keyBase + "calVelM").c_str(),  1.0f);
  sensorDataTemp.calVelB  = perfs.getFloat((keyBase + "calVelB").c_str(),  0.0f);
  sensorDataTemp.calBat   = perfs.getFloat((keyBase + "calBat").c_str(),   0.0f);

  sensorDataTemp.id   = perfs.getString((keyBase + "id").c_str(),   "");
  sensorDataTemp.name = perfs.getString((keyBase + "name").c_str(), "");

  perfs.end();
}

void saveInfoUmum(const char* lastVersion, int totalNode, int totalRepeater, 
                  const char* calBatPusat, int timeReportPeriodic, 
                  const char* timePeriodicSensor, int jumlahSensor, 
                  const char* crc32web) {
  preferences.begin("info-umum", false);  // namespace "info-umum"

  preferences.putString("lastVersion", lastVersion);
  preferences.putInt("totalNode", totalNode);
  preferences.putInt("totalRepeater", totalRepeater);
  preferences.putString("calBatPusat", calBatPusat);
  preferences.putInt("timeReportPeriodic", timeReportPeriodic);
  preferences.putString("timePeriodicSensor", timePeriodicSensor);
  preferences.putInt("jumlahSensor", jumlahSensor);
  preferences.putString("crc32web", crc32web);

  preferences.end();
  Serial.println("✅ Info Umum tersimpan di EEPROM");
}

void loadInfoUmum() {
  preferences.begin("info-umum", true); // read only

  String lastVersion        = preferences.getString("lastVersion", "");
  int totalNode             = preferences.getInt("totalNode", 0);
  int totalRepeater         = preferences.getInt("totalRepeater", 0);
  String calBatPusat        = preferences.getString("calBatPusat", "");
  int timeReportPeriodic    = preferences.getInt("timeReportPeriodic", 0);
  String timePeriodicSensor = preferences.getString("timePeriodicSensor", "");
  int jumlahSensor          = preferences.getInt("jumlahSensor", 0);
  String crc32web           = preferences.getString("crc32web", "");

  preferences.end();

  Serial.println("=== Info Umum (EEPROM) ===");
  Serial.printf("lastVersion: %s\n", lastVersion.c_str());
  Serial.printf("totalNode: %d\n", totalNode);
  Serial.printf("totalRepeater: %d\n", totalRepeater);
  Serial.printf("calBatPusat: %s\n", calBatPusat.c_str());
  Serial.printf("timeReportPeriodic: %d\n", timeReportPeriodic);
  Serial.printf("timePeriodicSensor: %s\n", timePeriodicSensor.c_str());
  Serial.printf("jumlahSensor: %d\n", jumlahSensor);
  Serial.printf("crc32web: %s\n\n", crc32web.c_str());
  timeToReport = timeReportPeriodic;

}



void dataSent(uint8_t* address, uint8_t status) {
  sent = true;
  Serial.printf("Message sent to " MACSTR ", status: %d\n", MAC2STR(address), status);
}

bool macMatch(uint8_t* a, uint8_t* b) {
  for (int i = 0; i < 6; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

void dataReceived(uint8_t* address, uint8_t* dataRaw, uint8_t len, signed int rssi, bool broadcast) {
  
  String incoming = "";
  for (int i = 0; i < len; i++) incoming += (char)dataRaw[i];

  for (int node = 0, dataIndex = 0; node < RECEIVER_COUNT; node++) {
    if (macMatch(address, receivers[node])) {
      int count = dataPerNode[node];
      int idx = 0;
      for (int j = 0; j < count; j++) {
        int nextComma = incoming.indexOf(',', idx);
        String val;
        if (nextComma != -1) {
          val = incoming.substring(idx, nextComma);
          idx = nextComma + 1;
        } else {
          val = incoming.substring(idx);
          idx = incoming.length();  // force exit
        }

        data[dataIndex + j] = val;
      }
      break;
    }
    // Menambahkan offset jumlah data yang dilewati
    dataIndex += dataPerNode[node];
  }
}

void vSetupSDcard(){
  if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
}

void vSetupLittlefs(){
  // Inisialisasi LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Gagal mount LittleFS!");
    return;
  }
}

unsigned long getCurrentTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

void vSetupWifi(){
  // Serial2.begin(115200);
  // WiFi.mode(WIFI_MODE_STA);
  // WiFi.disconnect(false, true);
  // WiFi.mode(WIFI_AP_STA);
  // WiFi.softAP("Test Fan web 2", "12345678");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
  Serial.println(WiFi.softAPIP());
  client.setInsecure();

  //   WiFi.begin("Marnov", "jujurdanamanah");
  // Serial.print("Connecting to WiFi ..");
  //   while (WiFi.status() != WL_CONNECTED) {
  //     Serial.print('.');
  //     delay(1000);
  //   }
  // Serial.println(WiFi.localIP());
  // Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
  // Serial.println(WiFi.softAPIP());

}

void vReconnectWifi(){
  if (!WiFi.isConnected()){
      CounterWIFI++;
      Serial.println(CounterWIFI);
      if (CounterWIFI >= 20){
        CounterWIFI = 20;
        WiFi.disconnect();
        WiFi.begin(ssid, password);
        Serial.println("Belum terhubung Wifi");
      }
    }
    else {
      CounterWIFI=0;
    }
}

void vSetupEspNow(){
  quickEspNow.begin(10, 0, false);
  quickEspNow.onDataSent(dataSent);
  quickEspNow.onDataRcvd(dataReceived);
}

void vSetupADS(){
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
}

void vSetupServo(){
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);

  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
}

void vPembacaanADS(){
  int16_t adc0, adc1, adc2, adc3;
  float volts0, volts1, volts2, volts3;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  volts0 = ads.computeVolts(adc0);
  volts1 = ads.computeVolts(adc1);
  volts2 = ads.computeVolts(adc2);
  volts3 = ads.computeVolts(adc3);

  Serial.println("-----------------------------------------------------------");
  Serial.print("AIN0: "); Serial.print(adc0); Serial.print("  "); Serial.print(volts0); Serial.println("V");
  Serial.print("AIN1: "); Serial.print(adc1); Serial.print("  "); Serial.print(volts1); Serial.println("V");
  Serial.print("AIN2: "); Serial.print(adc2); Serial.print("  "); Serial.print(volts2); Serial.println("V");
  Serial.print("AIN3: "); Serial.print(adc3); Serial.print("  "); Serial.print(volts3); Serial.println("V");
  Arus1 = adc0*calM_Wcs1+calB_Wcs1;
  Arus2 = adc1*calM_Wcs2+calB_Wcs2;
  Arus3 = adc2*calM_Wcs3+calB_Wcs3;
  Arus4 = adc3*calM_Wcs4+calB_Wcs4;
}

String safeValue(String val) {
  val.trim();
  if (val.length() == 0 || val == "nan" || val == "-1.00") return "0";
  return val;
}

// float safeValueFloat(float val){
//   if(val < 0){
//     val = 0;
//     // Serial.println("val = 0");
//   }
// }

void parseSerialData(String input) {
  int index = 0;
  int start = 0;
  for (int i = 0; i < input.length(); i++) {
    if (input.charAt(i) == ',') {
      data[index++] = input.substring(start, i);
      start = i + 1;
    }
  }
  data[index] = input.substring(start); // elemen terakhir

  

  // SuhuDS1 = (CalM_DinD1*data[27].toFloat())+CalB_DinD1;
  // SuhuDS2 = (CalM_DinD2*data[38].toFloat())+CalB_DinD2;
  // SuhuDS3 = (CalM_DinD3*data[44].toFloat())+CalB_DinD3;
  
  SuhuDS1 = data[27].toFloat();
  SuhuDS2 = data[40].toFloat();
  SuhuDS3 = data[53].toFloat();

  if (SuhuDS1 < 0) SuhuDS1 = 0;
  if (SuhuDS2 < 0) SuhuDS2 = 0;
  if (SuhuDS3 < 0) SuhuDS3 = 0;

  WCSstate1 = !data[26].toInt();
  WCSstate2 = !data[39].toInt();
  WCSstate3 = !data[52].toInt();

  Power_Tunnel_1 = data[25].toInt();
  Heater_st_1 =  data[23].toInt();
  Fan_Power_1 = data[24].toInt();

  Power_Tunnel_2 = data[38].toInt();
  Heater_st_2 =  data[36].toInt();
  Fan_Power_2 = data[37].toInt();

  Power_Tunnel_3 = data[51].toInt();
  Heater_st_3 =  data[49].toInt();
  Fan_Power_3 = data[50].toInt();

  setpoint1 = data[29].toFloat();
  kp1 = data[30].toFloat();
  ki1 = data[31].toFloat();
  kd1 = data[32].toFloat();
  out_pid1 = data[33].toFloat();

  setpoint2 = data[42].toFloat();
  kp2 = data[43].toFloat();
  ki2 = data[44].toFloat();
  kd2 = data[45].toFloat();
  out_pid2 = data[46].toFloat();

  setpoint3 = data[55].toFloat();
  kp3 = data[56].toFloat();
  ki3 = data[57].toFloat();
  kd3 = data[58].toFloat();
  out_pid3 = data[59].toFloat();


  BME1 = data[28].toFloat();
  BME2 = data[41].toFloat();
  BME3 = data[54].toFloat();

  CalM_DinD1 = data[34].toFloat();
  CalB_DinD1 = data[35].toFloat();
 
  CalM_DinD2 = data[47].toFloat();
  CalB_DinD2 = data[48].toFloat();

  CalM_DinD3 = data[60].toFloat();
  CalB_DinD3 = data[61].toFloat();

  // Cetak semua nilai yang diterima
  for (int i = 0; i <= index; i++) {
    Serial.printf("data[%d] = %s\n", i, data[i].c_str());

  }
  
  Serial.println("selesai mendapatkan data");
  // reset_arr bisa diatur di sini juga kalau diperlukan
  reset_arr = 0;
}

void vPembacaanSerial(){
  static String serialBuffer = "";

  while (Serial2.available()) {
    char c = Serial2.read();

    // Deteksi awal dan akhir
    if (c == '<') {
      serialBuffer = "";
    } else if (c == '>') {
      // Data lengkap diterima
      parseSerialData(serialBuffer);
      serialBuffer = "";
      isSdCardLogging = true;
    } else {
      serialBuffer += c;
    }
  }
}

void vAwalSdCard(){
  deleteFile(SD, "/SuhuData.js");
  deleteFile(SD, "/WindData.js");
  deleteFile(SD, "/TekananData.js");
  deleteFile(SD, "/TunnelData.csv");

  deleteFile(SD, "/SuhuPipaData.js");
  deleteFile(SD, "/SuhuDindingData.js");
  deleteFile(SD, "/OutPID.js");
  deleteFile(SD, "/GabunganSuhu.js");

  vTulisDataAwalSuhu();
  vTulisDataAwalAngin();
  vTulisDataAwalTekanan();
  vTulisDataAwalTunnel();
  vTulisDataAwalSuhuPipa();
  vTulisDataAwalSuhuDinding();
  vTulisDataAwalOutPID();
  vTulisDataAwalGabunganSuhu();
}

void vAwalSetupSdCard(){
  vTulisDataAwalSuhu();
  vTulisDataAwalAngin();
  vTulisDataAwalTekanan();
  vTulisDataAwalTunnel();
  vTulisDataAwalSuhuPipa();
  vTulisDataAwalSuhuDinding();
  vTulisDataAwalOutPID();
  vTulisDataAwalGabunganSuhu();
}

void vTulisKeSDcard(){
  if(isSdCardLogging){
      waktuUpdate = getCurrentTime();

      writeBufferSuhu();
      writeBufferWind();
      writeBufferTekanan();
      writeBufferTunnel();
      writeBufferSuhuPipa();
      writeBufferOutPID();
      writeBufferSuhuDinding();
      writeBufferGabunganSuhu();

      
      writeToSdCard("SuhuData");
      writeToSdCard("WindData");
      writeToSdCard("TekananData");
      writeToSdCard("SuhuPipaData");
      writeToSdCard("SuhuDindingData");
      writeToSdCard("OutPID");
      writeToSdCard("GabunganSuhu");

      writeAllToSdCard("TunnelData");

      Serial.println("isSdCardLogging");

      isSdCardLogging = false; // Reset the flag after sending
    }
}

void vKirimFirebase(){

  unsigned long epochTime = getCurrentTime();

  struct tm timeinfo;
  gmtime_r((time_t*)&epochTime, &timeinfo);  // pecah epoch ke tm

  currentYear  = timeinfo.tm_year + 1900;
  currentMonth = timeinfo.tm_mon + 1;
  currentDay   = timeinfo.tm_mday;

  Serial.printf("Tanggal: %04d-%02d-%02d\n", currentYear, currentMonth, currentDay);


  char fileName[80] = "";
  char fileLoc[80] = "";
  snprintf(fileName, sizeof(fileName), "Tunnel_%04d-%02d-%02d.csv", currentYear,currentMonth,currentDay);
  snprintf(fileLoc, sizeof(fileLoc), "/Tunnel_%04d-%02d-%02d.csv", currentYear,currentMonth,currentDay);
  char fileNameUpload[80] = "";
  char fileLocUpload[80] = "";
  snprintf(fileNameUpload, sizeof(fileNameUpload), "Up_Tunnel_%04d-%02d-%02d.csv", currentYear,currentMonth,currentDay);
  snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d.csv", currentYear,currentMonth,currentDay);

  Serial.println("performPost");
  Serial.print("fileName=");
  Serial.println(fileName);
  Serial.print("fileLoc=");
  Serial.println(fileLoc);
  Serial.print("fileNameUpload=");
  Serial.println(fileNameUpload);
  Serial.print("fileLocUpload=");
  Serial.println(fileLocUpload);

  makeCopyDataFirebase(SD, "/TunnelData.csv");
  verifyCopyCRC(SD, "/TunnelData.csv");

  
}

// void vPrepareID() {
//   unsigned char mac[6];
//   WiFi.macAddress(mac);
//   snprintf(MergedMacAddress, sizeof(MergedMacAddress), "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
// }

// String vCheckCRCfile(const char *FileLoc, unsigned long timeoutMillis) {
//   CRC32 crc;
//   File dataFile = LittleFS.open(FileLoc, "r");
//   if (!dataFile) {
//     Serial.println(F("Error opening file"));
//     return "00000000";  // CRC default jika gagal membuka file
//   }
//   Serial.println(F("File available"));
//   unsigned long startTime = millis();  // Catat waktu mulai
//   // char HeaderID[100];
//   // snprintf(HeaderID, sizeof(HeaderID), "id,timeStamp,battery,temperature,humidity,velocity,pressure,power,signal\n");
//   // for (int i=0; HeaderID[i] != '\n'; i++){
//   //   crc.update(HeaderID[i]);
//   // }
//   // crc.update('\n');
//   while (dataFile.available()) {
//     // Periksa timeout
//     if (millis() - startTime > timeoutMillis) {
//       Serial.println(F("Timeout, exiting from vCheckCRCfile"));
//       dataFile.close();
//       return "00000000";  // CRC default jika timeout
//     }
//     uint8_t byteRead = dataFile.read();
//     crc.update(byteRead);
//   }
//   uint32_t CRCResult = crc.finalize();
//   char CRCResultChar[9];  // Menggunakan 9 karakter untuk nilai CRC32
//   snprintf(CRCResultChar, sizeof(CRCResultChar), "%08x", CRCResult);  // Format hasil CRC menjadi string hexa 8 karakter
//   Serial.print(F("Calculated CRC32: 0x"));
//   Serial.println(CRCResultChar);
//   dataFile.close();
//   return String(CRCResultChar);
// }

// int vPostDataPeriodicCSV(const char *FileLoc, const char *FileName) {
//   Serial.println("vPostDataPeriodicCSV");
//   Serial.print("FileLoc=");
//   Serial.println(FileLoc);
//   Serial.print("FileName=");
//   Serial.println(FileName);
//   vPrepareID();
//   int codePost = -1;
//   int httpCode = -1;
//   String CRC32File = vCheckCRCfile(FileLoc,10000);
//   char resourceID[60];
//   sprintf(resourceID, "/Tambang/periodic-csv/%s?feedback=all", MergedMacAddress);
//   // https://asia-east1-tambang-2b501.cloudfunctions.net/Tambang/periodic-csv/:id?feedback=1-2

//   // if (!modem.isGprsConnected()) {
//   //   setupModule();
//   // }// ganti wifi

//   Serial.print("LINK URL : ");
//   Serial.println(resourceID);
//   Serial.print("2.FileLoc=");
//   Serial.println(FileLoc);
//   File file = LittleFS.open(FileLoc, "r");
//   if (!file) {
//     Serial.println(F("Failed to open file for reading"));
//     return -1;
//   }
//   // if (bWebSerialON) {
//   //   WebSerial.println("run POST");
//   //   WebSerial.print("CRC32File = ");
//   //   WebSerial.println(CRC32File);
//   // }
  
//   unsigned char tempBuffer[CHUNCK_SIZE];
//   int sent_size;
//   bool bFailSendData = 0;
//   int x = 0;
//   char tempHead[150];
//   Serial.print(F("FileName:"));
//   Serial.println(FileName);
//   snprintf(tempHead, sizeof(tempHead), "--SNoveLab\r\nContent-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\nContent-Type: text/csv\r\n\r\n", FileName);
//   String head = String(tempHead);

//   // char CSVHeader[150];
//   // snprintf(CSVHeader, sizeof(CSVHeader), "id,timeStamp,battery,temperature,humidity,velocity,pressure,power,signal\n");
//   // String sCSVHeader = String(CSVHeader);
 

//   // uint32_t textLen = sCSVHeader.length() + file.size() + head.length() + strlen_P(tail) + CRC32File.length() + strlen_P(headData);
//   uint32_t textLen = file.size() + head.length() + strlen_P(tail) + CRC32File.length() + strlen_P(headData);
//   Serial.print(F("textLen:"));
//   Serial.println(textLen);
//   Serial.println(F("Sending HTTP POST request"));
//   http.beginRequest();
//   http.post(resourceID);
//   http.sendHeader("Content-Type: multipart/form-data; boundary=SNoveLab");
//   http.sendHeader("Content-Length", textLen);
//   http.beginBody();
//   http.print(head);
//   // http.print(sCSVHeader);
//   // http.write(tempBuffer, bytesRead);
//   // http.flush();
//   for (long i = 0; i < textLen && !bFailSendData; i += CHUNCK_SIZE) {
//     size_t bytesToRead = std::min(static_cast<size_t>(CHUNCK_SIZE), static_cast<size_t>(textLen - i));
//     Serial.print(F("bytesToRead: "));
//     Serial.println(bytesToRead);
//     size_t bytesRead = file.read(tempBuffer, bytesToRead);
//     Serial.print(F("Bytes read from SD card: "));
//     Serial.println(bytesRead);
//     sent_size = http.write(tempBuffer, bytesRead);
//     if (sent_size != bytesRead) {
//       bFailSendData = true;
//       Serial.println(F("Failed to send all data"));
//       // if (bWebSerialON) {
//       //   WebSerial.println(F("Failed to send all data"));
//       // }
//     }
//   }
//   http.flush();
//   http.print(headData);
//   http.print(CRC32File);
//   http.print(tail);
//   http.endRequest();

//   httpCode = http.responseStatusCode();
//   codePost = httpCode;

//   Serial.print("codePost: ");
//   Serial.println(codePost);
//   String response = http.responseBody();
//   Serial.print("response: ");
//   Serial.println(response);
//   // size_t jsonSize = measureJson(response.c_str());
//   // Serial.print("jsonSize =");
//   // Serial.println(jsonSize);
//   // DynamicJsonDocument doc(jsonSize * 1.1); // Berikan sedikit buffer lebih
//   // vParsingResponseWebV2(response.c_str());///////////////////////////////////////////////harus pakai ini nanti
//   // if (bWebSerialON) {
//   //   WebSerial.print("codePost: ");
//   //   WebSerial.println(codePost);
//   //   // WebSerial.print("response: ");
//   //   // WebSerial.println(response);
//   // }
//   file.close();
//   http.stop();
//   // SerialAT.println(F("AT+HTTPTERM"));
//   // modem.gprsDisconnect();
//   // modem.restart();
//   return codePost;
// }

String vCheckCRCfile(fs::FS &fs, const char *FileLoc, unsigned long timeoutMillis) {
  CRC32 crc;
  File dataFile = fs.open(FileLoc);
  if (!dataFile) {
    Serial.println(F("Error opening file"));
    return "00000000";  // CRC default jika gagal membuka file
  }
  Serial.println(F("File available"));
  unsigned long startTime = millis();  // Catat waktu mulai
  // char HeaderID[100];
  // snprintf(HeaderID, sizeof(HeaderID), "id,timeStamp,battery,temperature,humidity,velocity,pressure,power,signal\n");
  // for (int i=0; HeaderID[i] != '\n'; i++){
  //   crc.update(HeaderID[i]);
  // }
  // crc.update('\n');
  while (dataFile.available()) {
    // Periksa timeout
    if (millis() - startTime > timeoutMillis) {
      Serial.println(F("Timeout, exiting from vCheckCRCfile"));
      dataFile.close();
      return "00000000";  // CRC default jika timeout
    }
    uint8_t byteRead = dataFile.read();
    crc.update(byteRead);
  }
  uint32_t CRCResult = crc.finalize();
  char CRCResultChar[9];  // Menggunakan 9 karakter untuk nilai CRC32
  snprintf(CRCResultChar, sizeof(CRCResultChar), "%08x", CRCResult);  // Format hasil CRC menjadi string hexa 8 karakter
  Serial.print(F("Calculated CRC32: 0x"));
  Serial.println(CRCResultChar);
  dataFile.close();
  return String(CRCResultChar);
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  vSetupLittlefs();
  vSetupSDcard();
  // vAwalSdCard();
  vAwalSetupSdCard();

  vSetupWifi();
  vSetupServo();
  vAsyncWebServer();
  // vSetupEspNow();
  ensureNvsReady();
  vSetupEEPROM();
  // loadInfoUmum();
  vSetupADS();
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  xTaskCreatePinnedToCore(
    TaskSendData,   /* Task function. */
    "TaskSendData",     /* name of task. */
    20480,       /* Stack size of task */
    NULL,        /* parameter of the task */
    0,           /* priority of the task */
    NULL,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */                  
}

void TaskSendData( void * pvParameters ){
  (void) pvParameters;
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 1000/ portTICK_PERIOD_MS;
  xLastWakeTime = xTaskGetTickCount ();
  for(;;){
    if (!bOTAStart) {
      if (bSedangKirimData) {
        v_SendFile();
        bSedangKirimData = 0;
      }
    }
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!bstopLoop){
    if(millis() - ADSMillis >= IntervalADSMillis){
      vPembacaanADS();
      waktuUpdate = getCurrentTime();
      vReconnectWifi();

      Serial.println(waktuUpdate);
      Serial.print("timeToReport = ");
      Serial.println(timeToReport);
      // vKirimFirebase();
      Serial.println("LOOP SELESAI");
      ADSMillis = millis();
    }

    if(millis() - CopyFileMillis >= IntervalCopyFileMillis && !bSedangKirimData){

      vKirimFirebase();
      Serial.println("SELESAI MEMBUAT FILE");
      CopyFileMillis = millis();
    }

    if(millis() - KirimServerMillis >= IntervalKirimServerMillis){

      // vKirimFirebase();
      // bSedangKirimData = 1;
      // v_SendFile();
      // bSedangKirimData = 0;
      Serial.println("SELESAI Mengirim File");
      KirimServerMillis = millis();
    }
    vPembacaanSerial();
    vTulisKeSDcard();
  }
  

}

void v_SendFile() {
  // Use this only for HTTPS
  // client.setInsecure();//skip verification because our server changes rootCA every month or so. Skip this as long as we get certificate we accept
  if (client.connect(serverName, serverPort)) {
    Serial.println("Connection successful!");
    deleteFile(SD, "/TunnelData.csv");
    vTulisDataAwalTunnel();
    // char fileNameUpload[80] = "";
    // char fileLocUpload[80] = "";
    // snprintf(fileNameUpload, sizeof(fileNameUpload), "Up_Tunnel_%04d-%02d-%02d.csv", currentYear,currentMonth,currentDay);
    // snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d.csv", currentYear,currentMonth,currentDay);

    // File file = fs.open(fileLocUpload, "r");

    // char head[100];
    // snprintf(head, sizeof(head), "--SnoveLab\r\nContent-Disposition: form-data; name=\"File\"; filename=\"%s\"\r\nContent-Type: text/csv\r\n\r\n", fileLocUpload);

    // String CRC32File = vCheckCRCfile(SD,FileLoc,10000);
    // char headCRC[100];
    // snprintf(headCRC, sizeof(headCRC), "--SnoveLab\r\nContent-Disposition: form-data; name=\"CRC32\"\r\n\r\n%s\r\n", CRC32File);

    

    // String head = "--SnoveLab\r\nContent-Disposition: form-data; name=\"File\"; filename=\"DummyData20250826.csv\"\r\nContent-Type: text/csv\r\n\r\n";
    // String headCRC = "--SnoveLab\r\nContent-Disposition: form-data; name=\"CRC32\"\r\n\r\nB773AB77\r\n";
    // String tail = "\r\n--SnoveLab--\r\n";



    char fileNameUpload[80] = "";
    char fileLocUpload[80] = "";
    snprintf(fileNameUpload, sizeof(fileNameUpload), "Up_Tunnel_%04d-%02d-%02d.csv",
            currentYear, currentMonth, currentDay);
    snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d.csv",
            currentYear, currentMonth, currentDay);

    File file = SD.open(fileLocUpload, FILE_READ);

    // CRC32 dari file
    String CRC32File = vCheckCRCfile(SD, fileLocUpload, 10000);

    // buffer untuk header
    char headTemp[200];
    char headCRCTemp[200];
    char tailTemp[50];

    snprintf(headTemp, sizeof(headTemp),
            "--SnoveLab\r\n"
            "Content-Disposition: form-data; name=\"File\"; filename=\"%s\"\r\n"
            "Content-Type: text/csv\r\n\r\n",
            fileNameUpload);

    snprintf(headCRCTemp, sizeof(headCRCTemp),
            "--SnoveLab\r\n"
            "Content-Disposition: form-data; name=\"CRC32\"\r\n\r\n%s\r\n",
            CRC32File.c_str());

    snprintf(tailTemp, sizeof(tailTemp),
            "\r\n--SnoveLab--\r\n");


    String head = String(headTemp);
    String headCRC = String(headCRCTemp);
    String tail = String(tailTemp);

    Serial.println("head");
    Serial.println(head);
    Serial.println("headCRC");
    Serial.println(headCRC);
    Serial.println("tail");
    Serial.println(tail);
    

    // readFile(SD,fileLocUpload); 
    fileLen = file.size();
    uint32_t extraLen = head.length() + tail.length()+headCRC.length();
    uint32_t totalLen = fileLen + extraLen;


  
    client.print("POST ");
    client.print(serverPath);
    client.println(" HTTP/1.1");
    Serial.print("POST ");
    Serial.print(serverPath);
    Serial.println(" HTTP/1.1");
    
    client.print("Host: ");
    client.println(serverName);
    Serial.print("Host: ");
    Serial.println(serverName);
    

    client.println("Content-Length: " + String(totalLen));
    Serial.println("Content-Length: " + String(totalLen));
    
    client.println("Content-Type: multipart/form-data; boundary=SnoveLab");
    Serial.println("Content-Type: multipart/form-data; boundary=SnoveLab");
    
    client.println();
    Serial.println();
    
    client.print(headCRC);
    client.print(head);
    Serial.print(headCRC);
    Serial.print(head);

    //Use this for HTTPS because WiFiClientSecure does not provide Stream Type writing
    while (file.available()) {
      char c= file.read();
      client.write(c);
      Serial.write(c);
    }
    
    //Use this for HTTP
    // client.write(file);


    client.flush();
    Serial.flush();
    client.print(tail);
    Serial.println(tail);
    file.close();

    //Uncomment this to view server response
    
    int timoutTimer = 10000;
    unsigned long startTimer = millis();
    boolean state = false;

    // char getResponse[2048];
    // int getResponseIndex = 0;

    char headerLine[128];
    char httpStatus[4];
    int headerIndex = 0;
    bool statusCaptured = false;



    while ((startTimer + timoutTimer) > millis()) {
      Serial.print(".");
      vTaskDelay(100);      
      while (client.available()) {
        char c = client.read();
        Serial.print(c);


         if (c == '\n') {
            headerLine[headerIndex] = '\0';  // end the line

            if (!statusCaptured) {
              // Example: "HTTP/1.1 200 OK"
              // Skip "HTTP/1.1 " (9 chars) → status starts at index 9
              strncpy(httpStatus, headerLine + 9, 3);
              httpStatus[3] = '\0';  // null terminate
              statusCaptured = true;


            }

            headerIndex = 0;  // reset for next header
          }
          else if (c != '\r') {
            if (headerIndex < 128 - 1) {
              headerLine[headerIndex++] = c;
            }
          }






        if(c == '{'){
          state = true;
        }
        if(state){
          getResponse[getResponseIndex] = c;
          getResponseIndex++;
        }
      }
      if(state == true && !client.available()){
        break;
      }

    }

    Serial.println();

    bstopLoop = 1;
    Serial.print("HTTP Status: ");
    Serial.println(httpStatus);  // should print "200"

    Serial.print("getResponse>> ");
    for (int i = 0; i < getResponseIndex; i++){
      Serial.print(getResponse[i]);
    }

    parseResponse();

    Serial.println();
    
    client.stop();
    
    // deleteFile(SD, "/TunnelData.csv");
    // vTulisDataAwalTunnel();
    deleteFile(SD, fileLocUpload);
    bstopLoop = 0;
    Serial.println("jalankan loop");

  } else {
    Serial.println("Not Connected");
  }
}

// void parseResponse() {
//   // Buat StaticJsonDocument (jangan terlalu kecil)
//   StaticJsonDocument<4096> doc;

//   // Deserialize JSON dari buffer
//   DeserializationError error = deserializeJson(doc, getResponse, getResponseIndex);

//   if (error) {
//     Serial.print(F("deserializeJson() failed: "));
//     Serial.println(error.c_str());
//     return;
//   }

//   // Ambil data dasar
//   const char* lastVersion       = doc["lastVersion"];  
//   int totalNode                 = doc["totalNode"];   
//   int totalRepeater             = doc["totalRepeater"];  
//   const char* calBatPusat       = doc["calBatPusat"];
//   const char* timeReportPeriodic= doc["timeReportPeriodic"];
//   const char* timePeriodicSensor= doc["timePeriodicSensor"];
//   int jumlahSensor              = doc["jumlahSensor"];
//   const char* crc32web          = doc["crc32web"];

//   Serial.println("=== Info Umum ===");
//   Serial.printf("lastVersion: %s\n", lastVersion);
//   Serial.printf("totalNode: %d\n", totalNode);
//   Serial.printf("totalRepeater: %d\n", totalRepeater);
//   Serial.printf("calBatPusat: %s\n", calBatPusat);
//   Serial.printf("timeReportPeriodic: %s\n", timeReportPeriodic);
//   Serial.printf("timePeriodicSensor: %s\n", timePeriodicSensor);
//   Serial.printf("jumlahSensor: %d\n", jumlahSensor);
//   Serial.printf("crc32web: %s\n\n", crc32web);

//   // Loop sensor
//   Serial.println("=== Data Sensor ===");
//   JsonObject sensors = doc["sensor"];
//   for (JsonPair kv : sensors) {
//     const char* index  = kv.key().c_str();
//     JsonObject sensor  = kv.value().as<JsonObject>();

//     const char* id     = sensor["id"];
//     const char* name   = sensor["sensor"];

//     Serial.printf("[%s] ID=%s, Nama=%s\n", index, id, name);

//     // contoh ambil kalibrasi
//     Serial.printf("  calTempM=%s, calTempB=%s, calHumM=%s, calHumB=%s\n",
//                   sensor["calTempM"].as<const char*>(),
//                   sensor["calTempB"].as<const char*>(),
//                   sensor["calHumM"].as<const char*>(),
//                   sensor["calHumB"].as<const char*>());

//     Serial.printf("  calPresM=%s, calPresB=%s, calVelM=%s, calVelB=%s, calBat=%s\n\n",
//                   sensor["calPresM"].as<const char*>(),
//                   sensor["calPresB"].as<const char*>(),
//                   sensor["calVelM"].as<const char*>(),
//                   sensor["calVelB"].as<const char*>(),
//                   sensor["calBat"].as<const char*>());
//   }
// }

void parseResponse() {
  StaticJsonDocument<4096> doc;

  DeserializationError error = deserializeJson(doc, getResponse, getResponseIndex);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Info umum
  const char* lastVersion        = doc["lastVersion"];  
  int totalNode                  = doc["totalNode"];   
  int totalRepeater              = doc["totalRepeater"];  
  const char* calBatPusat        = doc["calBatPusat"];
  int timeReportPeriodic         = doc["timeReportPeriodic"];
  const char* timePeriodicSensor = doc["timePeriodicSensor"];
  int jumlahSensor               = doc["jumlahSensor"];
  const char* crc32web           = doc["crc32web"];

  Serial.println("=== Info Umum ===");
  Serial.printf("lastVersion: %s\n", lastVersion);
  Serial.printf("totalNode: %d\n", totalNode);
  Serial.printf("totalRepeater: %d\n", totalRepeater);
  Serial.printf("calBatPusat: %s\n", calBatPusat);
  Serial.printf("timeReportPeriodic: %d\n", timeReportPeriodic);
  Serial.printf("timePeriodicSensor: %s\n", timePeriodicSensor);
  Serial.printf("jumlahSensor: %d\n", jumlahSensor);
  Serial.printf("crc32web: %s\n\n", crc32web);

  saveInfoUmum(lastVersion, totalNode, totalRepeater, calBatPusat, 
             timeReportPeriodic, timePeriodicSensor, jumlahSensor, crc32web);

  timeToReport = timeReportPeriodic;

  // Data Sensor
  Serial.println("=== Data Sensor ===");
  JsonObject sensors = doc["sensor"];

  sensorCount = 0; // reset dulu
  for (JsonPair kv : sensors) {
    if (sensorCount >= MAX_SENSOR) break; // jangan sampai overflow array

    JsonObject sensor = kv.value().as<JsonObject>();
    SensorData &sensorD = sensorsData[sensorCount];

    sensorD.id       = sensor["id"].as<const char*>();
    sensorD.name     = sensor["sensor"].as<const char*>();
    sensorD.calTempM = sensor["calTempM"].as<float>();
    sensorD.calTempB = sensor["calTempB"].as<float>();
    sensorD.calHumM  = sensor["calHumM"].as<float>();
    sensorD.calHumB  = sensor["calHumB"].as<float>();
    sensorD.calPresM = sensor["calPresM"].as<float>();
    sensorD.calPresB = sensor["calPresB"].as<float>();
    sensorD.calVelM  = sensor["calVelM"].as<float>();
    sensorD.calVelB  = sensor["calVelB"].as<float>();
    sensorD.calBat   = sensor["calBat"].as<float>();

    Serial.printf("[%d] ID=%s, Nama=%s\n", sensorCount, sensorD.id.c_str(), sensorD.name.c_str());
    Serial.printf("  calTempM=%.2f, calTempB=%.2f, calHumM=%.2f, calHumB=%.2f\n",
                  sensorD.calTempM, sensorD.calTempB, sensorD.calHumM, sensorD.calHumB);
    Serial.printf("  calPresM=%.2f, calPresB=%.2f, calVelM=%.2f, calVelB=%.2f, calBat=%.2f\n\n",
                  sensorD.calPresM, sensorD.calPresB, sensorD.calVelM, sensorD.calVelB, sensorD.calBat);

    saveSensorData(sensorCount, sensorsData[sensorCount]);


    sensorCount++;
  }

  float suhuReal1 = (sensorsData[0].calTempM * data[0].toFloat()) + sensorsData[0].calTempB;
  float suhuReal2 = (sensorsData[1].calTempM * data[2].toFloat()) + sensorsData[1].calTempB;
  float suhuReal3 = (sensorsData[2].calTempM * data[4].toFloat()) + sensorsData[2].calTempB;
  float suhuReal4 = (sensorsData[3].calTempM * data[6].toFloat()) + sensorsData[3].calTempB;
  float suhuReal5 = (sensorsData[4].calTempM * data[8].toFloat()) + sensorsData[4].calTempB;
  float suhuReal6 = (sensorsData[5].calTempM * data[10].toFloat()) + sensorsData[5].calTempB;
  float suhuReal7 = (sensorsData[6].calTempM * data[12].toFloat()) + sensorsData[6].calTempB;
  float suhuReal8 = (sensorsData[7].calTempM * data[14].toFloat()) + sensorsData[7].calTempB;
  float suhuReal9 = (sensorsData[8].calTempM * data[17].toFloat()) + sensorsData[8].calTempB;

  Serial.println("suhuReal1  =");
  Serial.println(suhuReal1);
  Serial.println("suhuReal2 =");
  Serial.println(suhuReal2);
  Serial.println("suhuReal3  =");
  Serial.println(suhuReal3);
  Serial.println("suhuReal4 =");
  Serial.println(suhuReal4);
  Serial.println("suhuReal5  =");
  Serial.println(suhuReal5);
  Serial.println("suhuReal6 =");
  Serial.println(suhuReal6);
  Serial.println("suhuReal7  =");
  Serial.println(suhuReal7);
  Serial.println("suhuReal8 =");
  Serial.println(suhuReal8);
  Serial.println("suhuReal9  =");
  Serial.println(suhuReal9);
}

