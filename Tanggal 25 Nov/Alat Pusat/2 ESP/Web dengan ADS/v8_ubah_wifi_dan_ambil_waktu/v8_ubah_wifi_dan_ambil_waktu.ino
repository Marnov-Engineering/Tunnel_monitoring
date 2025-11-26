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

#include <NTPClient.h>
#include <WiFiUdp.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

Preferences preferences;

const char* ssid = "Web Tunnel Monitoring";
const char* password = "12345678";

// waktu
WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 0;
unsigned long waktuUpdate = 0;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// var baru

int setPointPower = 0;
// float Kecepatan = 0;
// float Arus = 0.00000;
// float calM = 1.00000;
// float calB = 0.00000;
// int Safety = true;
bool Update_OTA = 0;
bool isSdCardLogging = 0;

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
String SuhuPipaData;
String SuhuDindingData;
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

String SData;
String SDataHeat;

unsigned long ADSMillis;
unsigned long IntervalADSMillis = 1000;

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


String data[41];  // Stores data1 to data20

bool sent = true;

void vSetupEEPROM(){
  preferences.begin("my-app", false);
  calM_Wcs1  = preferences.getFloat("calM_Wcs1", 0);
  calB_Wcs1  = preferences.getFloat("calB_Wcs1", 0);
  calM_Wcs2  = preferences.getFloat("calM_Wcs2", 0);
  calB_Wcs2  = preferences.getFloat("calB_Wcs2", 0);
  calM_Wcs3  = preferences.getFloat("calM_Wcs3", 0);
  calB_Wcs3  = preferences.getFloat("calB_Wcs3", 0);
  calM_Wcs4  = preferences.getFloat("calM_Wcs4", 0);
  calB_Wcs4  = preferences.getFloat("calB_Wcs4", 0);
  CalM_DinD1 =  preferences.getFloat("CalM_DinD1", 0);
  CalB_DinD1 =  preferences.getFloat("CalB_DinD1", 0);
  CalM_DinD2 =  preferences.getFloat("CalM_DinD2", 0);
  CalB_DinD2 =  preferences.getFloat("CalB_DinD2", 0);
  CalM_DinD3 =  preferences.getFloat("CalM_DinD3", 0);
  CalB_DinD3 =  preferences.getFloat("CalB_DinD3", 0);
  Suhu_Tunnel = preferences.getInt("Suhu_Tunnel", 0);
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


void vLoopESPnow(){
  static time_t lastSend = 0;
  static unsigned int counter = 0;
  static int i = 0;
  static bool countToReport = false;
  static unsigned long cycleStartTime = 0;

  if (i == 0 && !countToReport) {
    cycleStartTime = millis();
    // Serial.print("masuksini>> ");
  }

  if (quickEspNow.readyToSendData() && sent && ((millis() - lastSend) > SEND_MSG_MSEC) && i < 12) {
    lastSend = millis();
    String message = String(msg) + " " + String(counter++);
    sent = false;

    // if (i == 18) { // valve20
    //   message = "";
    //   for (int j = 0; j < 8; j++) {
    //     message += String(random(0, 2));
    //     if (j < 7) message += ",";
    //   }
    // }

    // quickEspNow.send(receivers[i], (uint8_t*)message.c_str(), message.length());
    // i++;
    // Serial.println("now>> " + i);

    // if (i == 20) {
    //   countToReport = true;
    // }
    // static int i = 0;
    if (i < RECEIVER_COUNT) {
      quickEspNow.send(receivers[i], (uint8_t*)message.c_str(), message.length());
      i++;
      // Serial.printf("now>> %d\n", i);

      if (i == RECEIVER_COUNT) {
        countToReport = true;
        Serial.print("countToReport>> ");
        Serial.println(countToReport);
      }
      
      // countToReport = true;
    }

  }

  if (countToReport && (millis() - cycleStartTime >= 1500)) {
    Serial.print("masuk sini>> ");
    String output = "";
    for (int j = 0; j < 41; j++) {
      output += data[j];
      if (j < 40) output += ",";
      Serial.print("data[");
      Serial.print(j);
      Serial.print("] = ");
      Serial.println(data[j]);  // Cetak nilai masing-masing
      // data[j] = "";  // Clear for next cycle
    }

    
    reset_arr++;
    Serial.println("now>> " + output);

    Serial.println("reset_arr>> " + reset_arr);
    // Serial2.print("<" + output + ">");

    countToReport = false;
    i = 0;
    sent = true;
    if(reset_arr == 10){

      for (int j = 0; j < 41; j++) {
        data[j] = "";  // Clear for next cycle
      }
      reset_arr = 0;
    }
  }

  // if (Serial2.available()) {
  //   Serial2.setTimeout(10);  // 10 ms timeout
  //   String serialInput = Serial2.readStringUntil('\n');
  //   int idxValve = serialInput.indexOf("valve>");
  //   if (idxValve != -1) {
  //     serialInput = serialInput.substring(idxValve + 6); // 6 is length of "valve>"
  //     serialInput.trim();
  //     Serial.println("Valve data: " + serialInput);
  //     quickEspNow.send(receivers[18], (uint8_t*)serialInput.c_str(), serialInput.length());
  //     // You can process valveData as needed here
  //   }
  // }
  // Serial.printf("[DEBUG] sent=%d, i=%d, countToReport=%d, time=%lu\n", sent, i, countToReport, millis() - cycleStartTime);

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

  // Cetak semua nilai yang diterima
  for (int i = 0; i <= index; i++) {
    Serial.printf("data[%d] = %s\n", i, data[i].c_str());

  }

  SuhuDS1 = (CalM_DinD1*data[27].toFloat())+CalB_DinD1;
  SuhuDS2 = (CalM_DinD2*data[33].toFloat())+CalB_DinD2;
  SuhuDS3 = (CalM_DinD3*data[39].toFloat())+CalB_DinD3;

  WCSstate1 = data[26];
  WCSstate2 = data[32];
  WCSstate3 = data[38];

  Power_Tunnel_1 = data[25].toInt();
  Heater_st_1 =  data[23].toInt();
  Fan_Power_1 = data[24].toInt();

  Power_Tunnel_2 = data[31].toInt();
  Heater_st_2 =  data[29].toInt();
  Fan_Power_2 = data[30].toInt();

  Power_Tunnel_3 = data[37].toInt();
  Heater_st_3 =  data[35].toInt();
  Fan_Power_3 = data[36].toInt();


  BME1 = data[28].toInt();
  BME2 = data[34].toInt();
  BME3 = data[40].toInt();
  
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

  deleteFile(SD, "/SuhuPipaData.js");
  deleteFile(SD, "/SuhuDindingData.js");

  vTulisDataAwalSuhu();
  vTulisDataAwalAngin();
  vTulisDataAwalTekanan();
  vTulisDataAwalSuhuPipa();
  vTulisDataAwalSuhuDinding();
}

void vTulisKeSDcard(){
  if(isSdCardLogging){
      waktuUpdate = getCurrentTime();

      writeBufferSuhu();
      writeBufferWind();
      writeBufferTekanan();
      writeBufferSuhuPipa();
      writeBufferSuhuDinding();

      
      writeToSdCard("SuhuData");
      writeToSdCard("WindData");
      writeToSdCard("TekananData");
      writeToSdCard("SuhuPipaData");
      writeToSdCard("SuhuDindingData");

      Serial.println("isSdCardLogging");

      isSdCardLogging = false; // Reset the flag after sending
    }
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  vSetupLittlefs();
  vSetupSDcard();
  vAwalSdCard();
  vSetupWifi();
  vSetupServo();
  vAsyncWebServer();
  // vSetupEspNow();
  vSetupEEPROM();
  vSetupADS();
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
}

void loop() {
  // put your main code here, to run repeatedly:
  // vLoopESPnow();
  if(millis() - ADSMillis >= IntervalADSMillis){
    vPembacaanADS();
    waktuUpdate = getCurrentTime();
    
    Serial.println(waktuUpdate);
    ADSMillis = millis();
  }
  vPembacaanSerial();
  vTulisKeSDcard();
}
