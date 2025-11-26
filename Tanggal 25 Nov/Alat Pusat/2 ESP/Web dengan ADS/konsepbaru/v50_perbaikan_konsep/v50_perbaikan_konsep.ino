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
#include <HTTPClient.h>
#include <CRC32.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Time.h>

#include <ArduinoHttpClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <PID_v1.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define VERSION 1
SemaphoreHandle_t  sdCardMutex;
bool bPertamaKirim = 1;

Preferences preferences;

double SetpointFan1, InputFan1, OutputFan1, last_InputFan1, OutPIDFan1;
float kpFan1 = 0;
float kiFan1 = 0;
float kdFan1 = 0;
PID PIDFan1(&InputFan1, &OutputFan1, &SetpointFan1, kpFan1, kiFan1, kdFan1, DIRECT);


double SetpointFan2, InputFan2, OutputFan2, last_InputFan2, OutPIDFan2;
float kpFan2 = 0;
float kiFan2 = 0;
float kdFan2 = 0;
PID PIDFan2(&InputFan2, &OutputFan2, &SetpointFan2, kpFan2, kiFan2, kdFan2, DIRECT);

double SetpointFan3, InputFan3, OutputFan3, last_InputFan3, OutPIDFan3;
float kpFan3 = 0;
float kiFan3 = 0;
float kdFan3 = 0;
PID PIDFan3(&InputFan3, &OutputFan3, &SetpointFan3, kpFan3, kiFan3, kdFan3, DIRECT);

double SetpointFan4, InputFan4, OutputFan4, last_InputFan4, OutPIDFan4;
float kpFan4 = 0;
float kiFan4 = 0;
float kdFan4 = 0;
PID PIDFan4(&InputFan4, &OutputFan4, &SetpointFan4, kpFan4, kiFan4, kdFan4, DIRECT);

// TaskHandle_t Task5;


ESP32Time rtc;


//varbaru
DynamicJsonDocument doc(10580);
JsonArray rows = doc.createNestedArray("rows");
double arrayJson[40][12];
String arraywaktu[12];
int counterParsing = 0; 
int BanyakDataPengiriman = 0;
unsigned long timeUpdate;

#define ROWS 40
#define COL_SIZE 256

uint8_t buffer[ROWS][COL_SIZE];   // buffer matrix
size_t bytesPerRow[ROWS];         // menyimpan ukuran valid tiap baris buffer

// const char* ssid = "Marnov";
// const char* password = "jujurdanamanah";

const char* ssid = "Web Tunnel Monitoring";
const char* password = "12345678";

// waktu
WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 0;
unsigned long waktuUpdate = 0;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
String SendURL;


// var baru

#define WINDOW_SIZE 5

// int readings[WINDOW_SIZE];   // buffer untuk simpan data
// int indexReading = 0;        // index data saat ini
// long total = 0;              // jumlah data dalam buffer
// int average = 0;

float readkec1 [WINDOW_SIZE];
float readkec2 [WINDOW_SIZE];
float readkec3 [WINDOW_SIZE];
float readkec4 [WINDOW_SIZE];
float readkec5 [WINDOW_SIZE];
float readkec6 [WINDOW_SIZE];
float readkec7 [WINDOW_SIZE];
float readUkec1 [WINDOW_SIZE];
float readUkec2 [WINDOW_SIZE];

float Totalkec1 = 0;
float Totalkec2 = 0;
float Totalkec3 = 0;
float Totalkec4 = 0;
float Totalkec5 = 0;
float Totalkec6 = 0;
float Totalkec7 = 0;
float TotalUkec1 = 0;
float TotalUkec2 = 0;

float Averagekec1 = 0;
float Averagekec2 = 0;
float Averagekec3 = 0;
float Averagekec4 = 0;
float Averagekec5 = 0;
float Averagekec6 = 0;
float Averagekec7 = 0;
float AverageUkec1 = 0;
float AverageUkec2 = 0;

int indexMA = 0;               // posisi data terbaru (rolling)
int countMA = 0;               // jumlah data valid (maksimal WINDOW_SIZE)





const char *firmwareURL = "https://firebasestorage.googleapis.com/v0/b/otastorage-503b9.appspot.com/o/Sigapp_sensor.bin?alt=media";


int errorcount = 0; 
int part = 0;
int LED_INDX = 2;
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
char getResponse[4096];   // sesuaikan ukuran dengan response JSON yang kamu dapat
int getResponseIndex = 0; // panjang data valid dalam getResponse
bool bOTAStart = 0;
int timeToReport;
int errortimeToReport;
bool bstopLoop = 0;

int iLastVersion;
int iTotalNode;
int iTimeReportPeriodic = 1;
int iTimePeriodicSensor;
int iTotalRepeater;
int iJumlahSensor;
float fCalBatPusat;
char CRC32Web[12];
float fCalBatR[11];
float fCalBatS[11];
float fCalTempM[11];
float fCalTempB[11];
float fCalHumM[11];
float fCalHumB[11];
float fCalVeloM[11];
float fCalVeloB[11];
float fCalPresM[11];
float fCalPresB[11];


// WiFiClientSecure client;  

// HttpClient http(client, server_tambang, port);

char serverName[256] = "asia-east1-tambang-2b501.cloudfunctions.net";   // REPLACE WITH YOUR Server IP ADDRESS

char serverPath[256] = "/Tambang/periodic-csv/1C692094D05C?feedback=all";      // ServerPath /upload or /file-upload


char serverNameSpreadSheet[256] = "script.google.com";
char serverPathSpreadSheet[256] = "/macros/s/AKfycbzZfP1UmszC5JceiMZjyEcWA2HOOhwMyJ5zUW2M5ZCVNFsUoFRmiEsRJcfJZtyDJ8LBxw/exec";
// const int serverPort = 443;

const char* KEY = "AKfycbw5sVGed1dCH6JcbBMuvv-Ao_jgd3eXFpIDm-g0-IkvFkK0PR08-LGw6ysa_Kzy7U6wpw";
const char* APP_SERVER = "script.google.com";

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

// const int MAX_SENSOR = 9;   // sesuaikan dengan jumlah sensor maksimum
// SensorData sensorsData[MAX_SENSOR];
// int sensorCount = 0;         // jumlah sensor terisi
const int MAX_SENSOR = 60;  // sesuaikan jumlah sensor maksimal
String lastVal[MAX_SENSOR];
int dropCount[MAX_SENSOR];




String suhu1;
String kec1;
String Humid1;

String suhu2;
String kec2;
String Humid2;

String suhu3;
String kec3;
String Humid3;

String suhu4;
String kec4;
String Humid4;

String suhu5;
String kec5;
String Humid5;

String suhu6;
String kec6;
String Humid6;

String suhu7;
String kec7;
String Humid7;

String Usuhu1;
String Ukec1;
String UHumid1;
String USDP1;

String Usuhu2;
String Ukec2;
String UHumid2;
String USDP2;

String SuhuPipaData;
String SuhuDindingData;
String GabunganSuhu;

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
int Max_PID_Tunnel_1;
int Heater_st_1 ;
int Fan_Power_1 ;
int Suhu_Tunnel_1 = 0;

int Power_Tunnel_2 ;
int Max_PID_Tunnel_2;
int Heater_st_2 ;
int Fan_Power_2 ;
int Suhu_Tunnel_2 = 0;

int Power_Tunnel_3 ;
int Max_PID_Tunnel_3;
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
int16_t adc0, adc1, adc2, adc3;

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

float Watt1 = 0 ;
float Watt2 = 0 ;
float Watt3 = 0 ;
float Watt4 = 0 ;

int PWM1 = 0 ;
int PWM2 = 0 ;
int PWM3 = 0 ;
int PWM4 = 0 ;

float Arus1 ;
float Arus2 ;
float Arus3 ;
float Arus4 ;

int SetPointInput1 = 0;
int SetPointInput2 = 0;
int SetPointInput3 = 0;
int SetPointInput4 = 0;

int kecepatan1 = 0;
int kecepatan2 = 0;
int kecepatan3 = 0;
int kecepatan4 = 0;

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
String SDataMAC;

unsigned long ADSMillis;
unsigned long IntervalADSMillis = 1000;

unsigned long CopyFileMillis;
unsigned long IntervalCopyFileMillis = 30000;

unsigned long KirimServerMillis;
unsigned long IntervalKirimServerMillis;

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

static uint8_t heater1[] = {0x1C, 0x69, 0x20, 0x96, 0x77, 0x60};
static uint8_t heater2[] = {0xEC, 0xE3, 0x34, 0xD7, 0x70, 0x60};
static uint8_t heater3[] = {0x1C, 0x69, 0x20, 0x96, 0xEC, 0xC8};

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

static uint8_t* allMACs[] = {
  sensorDalam1, sensorDalam2, sensorDalam3, sensorDalam4, sensorDalam5, sensorDalam6, sensorDalam7,
  sensorUjung1, sensorUjung2,
  heater1, heater2, heater3,
  sensorDinding1, sensorDinding2, sensorDinding3
};

const uint8_t RECEIVER_COUNT = 12;

static uint8_t* receivers[RECEIVER_COUNT] = {
  sensorDalam1, sensorDalam2, sensorDalam3, sensorDalam4, sensorDalam5,
  sensorDalam6, sensorDalam7,
  sensorUjung1, sensorUjung2,
  sensorDinding1,sensorDinding2,sensorDinding3
};

// Jumlah data yang dikirim setiap node
const uint8_t dataPerNode[RECEIVER_COUNT] = {
  3, 3, 3, 3, 3, 3, 3,   // 7 sensorDalam → 2 data per node
  4, 4, 1, 1, 1                   // 2 sensorUjung → 3 data per node
};

#define MAX_DATA 150
String data[MAX_DATA];  // Stores data1 to data20
String data1;
int CounterKoma;
bool StatusComData;
String Last_data[MAX_DATA];
int CounterDataError = 0;
int a;
bool DataNotValid = 0;
String DataBuffer;
String DataASCII;
int stringData;
String perBagianData;


bool sent = true;

void vSetupEEPROM(){
  preferences.begin("my-app", false);

  part = preferences.getInt("part", 0);

  PWM1 = preferences.getInt("PWM1", 0);
  PWM2 = preferences.getInt("PWM2", 0);
  // servo1.write(PWM1);
  // servo2.write(PWM1);

  errorcount = preferences.getInt("errorcount", 0);
  calM_Wcs1  = preferences.getFloat("calM_Wcs1", 0);
  calB_Wcs1  = preferences.getFloat("calB_Wcs1", 0);
  calM_Wcs2  = preferences.getFloat("calM_Wcs2", 0);
  calB_Wcs2  = preferences.getFloat("calB_Wcs2", 0);
  calM_Wcs3  = preferences.getFloat("calM_Wcs3", 0);
  calB_Wcs3  = preferences.getFloat("calB_Wcs3", 0);
  calM_Wcs4  = preferences.getFloat("calM_Wcs4", 0);
  calB_Wcs4  = preferences.getFloat("calB_Wcs4", 0);

  preferences.getBytes("SensDlm1", &sensorDalam1, sizeof(sensorDalam1));
  preferences.getBytes("SensDlm2", &sensorDalam2, sizeof(sensorDalam2));
  preferences.getBytes("SensDlm3", &sensorDalam3, sizeof(sensorDalam3));
  preferences.getBytes("SensDlm4", &sensorDalam4, sizeof(sensorDalam4));
  preferences.getBytes("SensDlm5", &sensorDalam5, sizeof(sensorDalam5));
  preferences.getBytes("SensDlm6", &sensorDalam6, sizeof(sensorDalam6));
  preferences.getBytes("SensDlm7", &sensorDalam7, sizeof(sensorDalam7));

  preferences.getBytes("SensUjn1", &sensorUjung1, sizeof(sensorUjung1));
  preferences.getBytes("SensUjn2", &sensorUjung2, sizeof(sensorUjung2));

  preferences.getBytes("Heater1", &heater1, sizeof(heater1));
  preferences.getBytes("Heater2", &heater2, sizeof(heater2));
  preferences.getBytes("Heater3", &heater3, sizeof(heater3));

  preferences.getBytes("Dindng1", &sensorDinding1, sizeof(sensorDinding1));
  preferences.getBytes("Dindng2", &sensorDinding2, sizeof(sensorDinding2));
  preferences.getBytes("Dindng3", &sensorDinding3, sizeof(sensorDinding3));

  // kpFan1  = preferences.getFloat("kpFan1", 0);
  // kiFan1  = preferences.getFloat("kiFan1", 0);
  // kdFan1  = preferences.getFloat("kdFan1", 0);
  // kpFan2  = preferences.getFloat("kpFan2", 0);
  // kiFan2  = preferences.getFloat("kiFan2", 0);
  // kdFan2  = preferences.getFloat("kdFan2", 0);

  Serial.println("eeprom>> ");
  Serial.println("calM_Wcs1: " + String(calM_Wcs1, 6));
  Serial.println("calB_Wcs1: " + String(calB_Wcs1, 6));
  Serial.println("calM_Wcs2: " + String(calM_Wcs2, 6));
  Serial.println("calB_Wcs2: " + String(calB_Wcs2, 6));
  Serial.println("calM_Wcs3: " + String(calM_Wcs3, 6));
  Serial.println("calB_Wcs3: " + String(calB_Wcs3, 6));
  Serial.println("calM_Wcs4: " + String(calM_Wcs4, 6));
  Serial.println("calB_Wcs4: " + String(calB_Wcs4, 6));

  Serial.println("kpFan1: " + String(kpFan1, 6));
  Serial.println("kiFan1: " + String(kiFan1, 6));
  Serial.println("kdFan1: " + String(kdFan1, 6));

  Serial.println("kpFan2: " + String(kpFan2, 6));
  Serial.println("kiFan2: " + String(kiFan2, 6));
  Serial.println("kdFan2: " + String(kdFan2, 6));


  Serial.println("eeprom>> ");
  Serial.println("SensDlm1: " + String(sensorDalam1[0], HEX) + String(sensorDalam1[1], HEX) + String(sensorDalam1[2], HEX) + String(sensorDalam1[3], HEX) + String(sensorDalam1[4], HEX) + String(sensorDalam1[5], HEX));
  Serial.println("SensDlm2: " + String(sensorDalam2[0], HEX) + String(sensorDalam2[1], HEX) + String(sensorDalam2[2], HEX) + String(sensorDalam2[3], HEX) + String(sensorDalam2[4], HEX) + String(sensorDalam2[5], HEX));
  Serial.println("SensDlm3: " + String(sensorDalam3[0], HEX) + String(sensorDalam3[1], HEX) + String(sensorDalam3[2], HEX) + String(sensorDalam3[3], HEX) + String(sensorDalam3[4], HEX) + String(sensorDalam3[5], HEX));
  Serial.println("SensDlm4: " + String(sensorDalam4[0], HEX) + String(sensorDalam4[1], HEX) + String(sensorDalam4[2], HEX) + String(sensorDalam4[3], HEX) + String(sensorDalam4[4], HEX) + String(sensorDalam4[5], HEX));
  Serial.println("SensDlm5: " + String(sensorDalam5[0], HEX) + String(sensorDalam5[1], HEX) + String(sensorDalam5[2], HEX) + String(sensorDalam5[3], HEX) + String(sensorDalam5[4], HEX) + String(sensorDalam5[5], HEX));
  Serial.println("SensDlm6: " + String(sensorDalam6[0], HEX) + String(sensorDalam6[1], HEX) + String(sensorDalam6[2], HEX) + String(sensorDalam6[3], HEX) + String(sensorDalam6[4], HEX) + String(sensorDalam6[5], HEX));
  Serial.println("SensDlm7: " + String(sensorDalam7[0], HEX) + String(sensorDalam7[1], HEX) + String(sensorDalam7[2], HEX) + String(sensorDalam7[3], HEX) + String(sensorDalam7[4], HEX) + String(sensorDalam7[5], HEX));
  Serial.println("sensorUjung1: " + String(sensorUjung1[0], HEX) + String(sensorUjung1[1], HEX) + String(sensorUjung1[2], HEX) + String(sensorUjung1[3], HEX) + String(sensorUjung1[4], HEX) + String(sensorUjung1[5], HEX));
  Serial.println("sensorUjung2: " + String(sensorUjung2[0], HEX) + String(sensorUjung2[1], HEX) + String(sensorUjung2[2], HEX) + String(sensorUjung2[3], HEX) + String(sensorUjung2[4], HEX) + String(sensorUjung2[5], HEX));
  Serial.println("Heater1: " + String(heater1[0], HEX) + String(heater1[1], HEX) + String(heater1[2], HEX) + String(heater1[3], HEX) + String(heater1[4], HEX) + String(heater1[5], HEX));
  Serial.println("Heater2: " + String(heater2[0], HEX) + String(heater2[1], HEX) + String(heater2[2], HEX) + String(heater2[3], HEX) + String(heater2[4], HEX) + String(heater2[5], HEX));
  Serial.println("Heater3: " + String(heater3[0], HEX) + String(heater3[1], HEX) + String(heater3[2], HEX) + String(heater3[3], HEX) + String(heater3[4], HEX) + String(heater3[5], HEX));
  Serial.println("Dindng1: " + String(sensorDinding1[0], HEX) + String(sensorDinding1[1], HEX) + String(sensorDinding1[2], HEX) + String(sensorDinding1[3], HEX) + String(sensorDinding1[4], HEX) + String(sensorDinding1[5], HEX));
  Serial.println("Dindng2: " + String(sensorDinding2[0], HEX) + String(sensorDinding2[1], HEX) + String(sensorDinding2[2], HEX) + String(sensorDinding2[3], HEX) + String(sensorDinding2[4], HEX) + String(sensorDinding2[5], HEX));
  Serial.println("Dindng3: " + String(sensorDinding3[0], HEX) + String(sensorDinding3[1], HEX) + String(sensorDinding3[2], HEX) + String(sensorDinding3[3], HEX) + String(sensorDinding3[4], HEX) + String(sensorDinding3[5], HEX));  
  
  // preferences.clear();
  // nvs_flash_erase();
  // nvs_flash_init();

  Serial.println(">>eeprom ");



  


  // const int jumlahSensor = iJumlahSensor +1;
  // for (int i = 1; i < jumlahSensor; i++) {
  //   // Serial.printf("Sensor %d: calTempM=%.3f, calHumB=%.3f, calPresM=%.3f, calVelM=%.3f, calHumM=%.3f, calVelB=%.3f, calTempB=%.3f, calPresB=%.3f, calBat=%.3f\n",
  //   //               i, calTempM[i], calHumB[i], calPresM[i], calVelM[i], calHumM[i], calVelB[i], calTempB[i], calPresB[i], calBat[i]);
  //   fCalBatS[i] = preferences.getFloat(("S" + String(i) + "_Bat").c_str(), 0);
  //   fCalTempM[i] = preferences.getFloat(("S" + String(i) + "_TempM").c_str(), 0);
  //   fCalTempB[i] = preferences.getFloat(("S" + String(i) + "_TempB").c_str(), 0);
  //   fCalHumM[i] = preferences.getFloat(("S" + String(i) + "_HumM").c_str(), 0);
  //   fCalHumB[i] = preferences.getFloat(("S" + String(i) + "_HumB").c_str(), 0); 
  //   fCalVeloM[i] = preferences.getFloat(("S" + String(i) + "_VelM").c_str(), 0);
  //   fCalVeloB[i] = preferences.getFloat(("S" + String(i) + "_VelB").c_str(), 0);
  //   fCalPresM[i] = preferences.getFloat(("S" + String(i) + "_PresM").c_str(), 0);
  //   fCalPresB[i] = preferences.getFloat(("S" + String(i) + "_PresB").c_str(), 0);

  //   Serial.printf("Sensor %d: fCalTempM=%.3f, fCalHumB=%.3f, fCalPresM=%.3f, fCalVelM=%.3f, fCalHumM=%.3f, fCalVelB=%.3f, fCalTempB=%.3f, fCalPresB=%.3f, fCalBat=%.3f\n",
  //                 i, fCalTempM[i], fCalHumB[i], fCalPresM[i], fCalVeloM[i], fCalHumM[i], fCalVeloB[i], fCalTempB[i], fCalPresB[i], fCalBatS[i]);
  // }
  // Serial.printf("Key available for S15_TempM: %d\n", preferences.isKey("S15_TempM"));
  // Serial.printf("Key available for S15_TempM: %d\n", preferences.isKey("errorcount"));
}

void ensureNvsReady() {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    nvs_flash_init();
  }
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
  unsigned long epochTime = getCurrentTime();
  rtc.setTime(epochTime);
  Serial.print("epochTime : ");
  Serial.println(epochTime);
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
        // WiFi.disconnect();
        // WiFi.begin(ssid, password);
        // Serial.println("Belum terhubung Wifi");
        refreshWiFi();
      }
    }
    else {
      // unsigned long waktu = getCurrentTime();
      // Serial.println("waktu =");
      // Serial.println(waktu);
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
    // while (1);
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
  vProcADS();
  float volts0, volts1, volts2, volts3;

  volts0 = ads.computeVolts(adc0);
  volts1 = ads.computeVolts(adc1);
  volts2 = ads.computeVolts(adc2);
  volts3 = ads.computeVolts(adc3);

  // Serial.println("-----------------------------------------------------------");
  // Serial.print("AIN0: "); Serial.print(adc0); Serial.print("  "); Serial.print(volts0); Serial.println("V");
  // Serial.print("AIN1: "); Serial.print(adc1); Serial.print("  "); Serial.print(volts1); Serial.println("V");
  // Serial.print("AIN2: "); Serial.print(adc2); Serial.print("  "); Serial.print(volts2); Serial.println("V");
  // Serial.print("AIN3: "); Serial.print(adc3); Serial.print("  "); Serial.print(volts3); Serial.println("V");

  // Serial.println("-----------------------------------------------------------");
  // Serial.print("Arus1: ");
  // Serial.println(Arus1);
  // Serial.print("Arus2: ");
  // Serial.println(Arus2);
  // Serial.print("Arus3: ");
  // Serial.println(Arus3);
  // Serial.print("Arus4: ");
  // Serial.println(Arus4);
  // Serial.println("-----------------------------------------------------------");

  // Serial.print("Watt1: ");
  // Serial.println(Watt1);
  // Serial.print("Watt2: ");
  // Serial.println(Watt2);
  // Serial.print("Watt3: ");
  // Serial.println(Watt3);
  // Serial.print("Watt4: ");
  // Serial.println(Watt4);
}

void vProcADS(){
  
  // float volts0, volts1, volts2, volts3;

  const int nSamples = 20;
  float sumADC1 = 0;
  float sumADC2 = 0;
  float sumADC3 = 0;
  float sumADC4 = 0;
  for (int i = 0; i < nSamples; i++) {
    // sumADC += ads.readADC_SingleEnded(0);
    sumADC1 += ads.readADC_SingleEnded(0);
    sumADC2 += ads.readADC_SingleEnded(1);
    sumADC3 += ads.readADC_SingleEnded(2);
    sumADC4 += ads.readADC_SingleEnded(3);
    delay(2); // beri jeda kecil
  }
  adc0 = (sumADC1 / nSamples);
  adc1 = (sumADC2 / nSamples);
  adc2 = (sumADC3 / nSamples);
  adc3 = (sumADC4 / nSamples);

  Arus1 = (sumADC1 / nSamples) * calM_Wcs1 + calB_Wcs1;
  Arus2 = (sumADC2 / nSamples) * calM_Wcs2 + calB_Wcs2;
  Arus3 = (sumADC3 / nSamples) * calM_Wcs3 + calB_Wcs3;
  Arus4 = (sumADC4 / nSamples) * calM_Wcs4 + calB_Wcs4;

  Watt1 = Arus1*12;
  Watt2 = Arus2*12;
  Watt3 = Arus3*12;
  Watt4 = Arus4*12;

  if(Watt1 < 0){
    Watt1 = 0;
  }
  if(Watt2 < 0){
    Watt2 = 0;
  }
  if(Watt3 < 0){
    Watt3 = 0;
  }
  if(Watt4 < 0){
    Watt4 = 0;
  }
}

String safeValue(String val) {
  // static String lastVal = "0";    // menyimpan nilai terakhir valid
  // static int dropCount = 0;       // menghitung drop berturut-turut

  val.trim();
  if (val.length() == 0 || val == "nan" || val == "-1.00" || val == "-999.00") return "0";

  return val;
}

// String safeValue(String val, int id) {

//   val.trim();
//  bool isDrop = (val.length() == 0 || val.equalsIgnoreCase("nan") || val.equals("-1.00") || val.equals("0"));

//   if (isDrop) {
//     Serial.printf("data drop [%d] = last value %s\n", id, lastVal[id].c_str());
//     dropCount[id]++;
//     if (dropCount[id] < 3) {
//       // masih drop 1–2 kali → pakai nilai sebelumnya
//       // if (lastVal[id].length() == 0) return "0"; // jika belum ada nilai sebelumnya
      
//       return lastVal[id];
//     } else {
//       // drop 3x berturut-turut → pakai "0"
//       lastVal[id] = "0";
//       return "0";
//     }
//   } else {
//     // data valid → reset counter
//     dropCount[id] = 0;
//     lastVal[id] = val;
//     return val;
//   }
// }

// String safeValue(String val, int id) {
//   const int MAX_SENSOR = 60;
//   static String lastVal[MAX_SENSOR];
//   static bool pernahValid[MAX_SENSOR];

//   val.trim();

//   // kalau kosong/nan → langsung nol
//   if (val.length() == 0 || val.equalsIgnoreCase("nan") || val.equals("-1.00")) {
//     return (pernahValid[id]) ? lastVal[id] : "0";
//   }

//   float fVal = val.toFloat();
//   float fLast = (pernahValid[id]) ? lastVal[id].toFloat() : 0.0f;

//   // logika perbandingan
//   if ((fLast - fVal) == fVal) {
//     // drop → kembalikan last, update val jadi "0"
//     val = "0";
//     return lastVal[id];
//   } else {
//     // data valid → simpan sebagai lastVal
//     lastVal[id] = val;
//     pernahValid[id] = true;
//     return val;
//   }
// }


void printAllMACs() {
  char buffer[1024];   // buffer besar untuk semua MAC
  char temp[32];       // buffer sementara
  buffer[0] = '\0';    // kosongkan buffer

  strcat(buffer, "^"); // awalan *

  for (int i = 0; i < (sizeof(allMACs) / sizeof(allMACs[0])); i++) {
    sprintf(temp, "%02X:%02X:%02X:%02X:%02X:%02X",
            allMACs[i][0], allMACs[i][1], allMACs[i][2],
            allMACs[i][3], allMACs[i][4], allMACs[i][5]);

    strcat(buffer, temp);
    if (i < (sizeof(allMACs) / sizeof(allMACs[0])) - 1) {
      strcat(buffer, ",");  // tambah koma kecuali terakhir
    }
  }

  strcat(buffer, "*"); // akhiran *

  Serial2.println(buffer);
  Serial.println(buffer);
}



void parseSerialData(String input) {
  Serial.print("input>> ");
  Serial.println(input);

  // int index = 0;
  // int start = 0;
  // for (int i = 0; i < input.length(); i++) {
  //   if (input.charAt(i) == ',') {
  //     data[index++] = input.substring(start, i);
  //     start = i + 1;
  //   }
  // }
  // data[index] = input.substring(start); // elemen terakhir

  int index = 0;
  int start = 0;

  for (int i = 0; i < input.length(); i++) {
    if (input.charAt(i) == ',') {
      if (index < MAX_DATA) {
        data[index++] = input.substring(start, i);
      } else {
        Serial.println("⚠️ Jumlah elemen melebihi kapasitas data[]!");
        break;
      }
      start = i + 1;
    }
  }

  // elemen terakhir
  if (index < MAX_DATA) {
    data[index] = input.substring(start);
  }

  // Tampilkan hasil parsing
  for (int i = 0; i <= index; i++) {
    Serial.print("data["); Serial.print(i); Serial.print("] = ");
    Serial.println(data[i]);
  }

  

  // SuhuDS1 = (CalM_DinD1*data[27].toFloat())+CalB_DinD1;
  // SuhuDS2 = (CalM_DinD2*data[38].toFloat())+CalB_DinD2;
  // SuhuDS3 = (CalM_DinD3*data[44].toFloat())+CalB_DinD3;


  readkec1[indexMA] = safeValue(data[1]).toFloat();
  readkec2[indexMA] = safeValue(data[4]).toFloat();
  readkec3[indexMA] = safeValue(data[7]).toFloat();
  readkec4[indexMA] = safeValue(data[10]).toFloat();
  readkec5[indexMA] = safeValue(data[13]).toFloat();
  readkec6[indexMA] = safeValue(data[16]).toFloat();
  readkec7[indexMA] = safeValue(data[19]).toFloat();
  readUkec1[indexMA] = safeValue(data[23]).toFloat();
  readUkec2[indexMA] = safeValue(data[27]).toFloat();

  indexMA = (indexMA + 1) % WINDOW_SIZE;
  if (countMA < WINDOW_SIZE) countMA++;
  //////////////////////////////////////// Kec 1
  Totalkec1 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec1 += readkec1[i];
  }
  Averagekec1 = Totalkec1 / countMA;

  //////////////////////////////////////// Kec 2
  Totalkec2 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec2 += readkec2[i];
  }
  Averagekec2 = Totalkec2 / countMA;

  //////////////////////////////////////// Kec 3
  Totalkec3 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec3 += readkec3[i];
  }
  Averagekec3 = Totalkec3 / countMA;

  //////////////////////////////////////// Kec 4
  Totalkec4 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec4 += readkec4[i];
  }
  Averagekec4 = Totalkec4 / countMA;

  //////////////////////////////////////// Kec 5
  Totalkec5 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec5 += readkec5[i];
  }
  Averagekec5 = Totalkec5 / countMA;

  //////////////////////////////////////// Kec 6
  Totalkec6 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec6 += readkec6[i];
  }
  Averagekec6 = Totalkec6 / countMA;

  //////////////////////////////////////// Kec 7
  Totalkec7 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec7 += readkec7[i];
  }
  Averagekec7 = Totalkec7 / countMA;

  //////////////////////////////////////// UKec 1
  TotalUkec1 = 0;
  for (int i = 0; i < countMA; i++) {
    TotalUkec1 += readUkec1[i];
  }
  AverageUkec1 = TotalUkec1 / countMA;

  //////////////////////////////////////// UKec 2
  TotalUkec2 = 0;
  for (int i = 0; i < countMA; i++) {
    TotalUkec2 += readUkec2[i];
  }
  AverageUkec2 = TotalUkec2 / countMA;


  // suhu1 = data[0];
  // kec1 = data[1];
  // Humid1 = data[2];

  // suhu2 = data[3];
  // kec2 = data[4];
  // Humid2 = data[5];

  // suhu3 = data[6];
  // kec3 = data[7];
  // Humid3 = data[8];

  // suhu4 = data[9];
  // kec4 =  data[10];
  // Humid4 = data[11];

  // suhu5 = data[12];
  // kec5 = data[13];
  // Humid5 = data[14];

  // suhu6 = data[15];
  // kec6 = data[16];
  // Humid6 = data[17];

  // suhu7 = data[18];
  // kec7 = data[19];
  // Humid7 = data[20];

  // Usuhu1 = data[21];
  // UHumid1 = data[22]; 
  // Ukec1 = data[23];
  // USDP1 = data[24];

  // Usuhu2 = data[25];
  // UHumid2 = data[26]; 
  // Ukec2 = data[27];
  // USDP2 = data[28];

  suhu1 = data[0];
  kec1 = String(Averagekec1);
  Humid1 = data[2];

  suhu2 = data[3];
  kec2 = String(Averagekec2);
  Humid2 = data[5];

  suhu3 = data[6];
  kec3 = String(Averagekec3);
  Humid3 = data[8];

  suhu4 = data[9];
  kec4 = String(Averagekec4);
  Humid4 = data[11];

  suhu5 = data[12];
  kec5 = String(Averagekec5);
  Humid5 = data[14];

  suhu6 = data[15];
  kec6 = String(Averagekec6);
  Humid6 = data[17];

  suhu7 = data[18];
  kec7 = String(Averagekec7);
  Humid7 = data[20];

  Usuhu1 = data[21];
  UHumid1 = data[22]; 
  Ukec1 = String(AverageUkec1);
  USDP1 = data[24];

  Usuhu2 = data[25];
  UHumid2 = data[26]; 
  Ukec2 = String(AverageUkec2);
  USDP2 = data[28];
  // 29 tidak dipakai
  // 30 tidak dipakai
  // 31 tidak dipakai

  // ================= HEATER 1 =================

  Heater_st_1     = data[32].toInt();
  Fan_Power_1     = data[33].toInt();
  Power_Tunnel_1  = data[34].toInt();
  WCSstate1       = !data[35].toInt();
  SuhuDS1         = safeValue(data[36]).toFloat();
  BME1            = safeValue(data[37]).toFloat();
  setpoint1       = data[38].toFloat();
  kp1             = data[39].toFloat();
  ki1             = data[40].toFloat();
  kd1             = data[41].toFloat();
  out_pid1        = data[42].toFloat();
  CalM_DinD1      = data[43].toFloat();
  CalB_DinD1      = data[44].toFloat();
  Max_PID_Tunnel_1 = data[45].toInt();

  // ================= HEATER 2 =================
  Heater_st_2     = data[46].toInt();
  Fan_Power_2     = data[47].toInt();
  Power_Tunnel_2  = data[48].toInt();
  WCSstate2       = !data[49].toInt();
  SuhuDS2         = safeValue(data[50]).toFloat();
  BME2            = safeValue(data[51]).toFloat();
  setpoint2       = data[52].toFloat();
  kp2             = data[53].toFloat();
  ki2             = data[54].toFloat();
  kd2             = data[55].toFloat();
  out_pid2        = data[56].toFloat();
  CalM_DinD2      = data[57].toFloat();
  CalB_DinD2      = data[58].toFloat();
  Max_PID_Tunnel_2 = data[59].toInt();

  // ================= HEATER 3 =================
  Heater_st_3     = data[60].toInt();
  Fan_Power_3     = data[61].toInt();
  Power_Tunnel_3  = data[62].toInt();
  WCSstate3       = !data[63].toInt();
  SuhuDS3         = safeValue(data[64]).toFloat();
  BME3            = safeValue(data[65]).toFloat();
  setpoint3       = data[66].toFloat();
  kp3             = data[67].toFloat();
  ki3             = data[68].toFloat();
  kd3             = data[69].toFloat();
  out_pid3        = data[70].toFloat();
  CalM_DinD3      = data[71].toFloat();
  CalB_DinD3      = data[72].toFloat();
  Max_PID_Tunnel_3 = data[73].toInt();


  if (SuhuDS1 < 0) SuhuDS1 = 0;
  if (SuhuDS2 < 0) SuhuDS2 = 0;
  if (SuhuDS3 < 0) SuhuDS3 = 0;

  // // Cetak semua nilai yang diterima
  // for (int i = 0; i <= index; i++) {
  //   Serial.printf("data[%d] = %s\n", i, data[i].c_str());

  // }
  isSdCardLogging = true;
  if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
    vTulisKeSDcard();
    xSemaphoreGive(sdCardMutex);
  }
  // accessToGoogleSheets();
  Serial.println("selesai mendapatkan data");
  printAllMACs();
  Serial.println("selesai Mengirim data");
  // reset_arr bisa diatur di sini juga kalau diperlukan
  // reset_arr = 0;
}

void vPemindahanData(){
  readkec1[indexMA] = safeValue(data[1]).toFloat();
  readkec2[indexMA] = safeValue(data[4]).toFloat();
  readkec3[indexMA] = safeValue(data[7]).toFloat();
  readkec4[indexMA] = safeValue(data[10]).toFloat();
  readkec5[indexMA] = safeValue(data[13]).toFloat();
  readkec6[indexMA] = safeValue(data[16]).toFloat();
  readkec7[indexMA] = safeValue(data[19]).toFloat();
  readUkec1[indexMA] = safeValue(data[23]).toFloat();
  readUkec2[indexMA] = safeValue(data[27]).toFloat();

  indexMA = (indexMA + 1) % WINDOW_SIZE;
  if (countMA < WINDOW_SIZE) countMA++;
  //////////////////////////////////////// Kec 1
  Totalkec1 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec1 += readkec1[i];
  }
  Averagekec1 = Totalkec1 / countMA;

  //////////////////////////////////////// Kec 2
  Totalkec2 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec2 += readkec2[i];
  }
  Averagekec2 = Totalkec2 / countMA;

  //////////////////////////////////////// Kec 3
  Totalkec3 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec3 += readkec3[i];
  }
  Averagekec3 = Totalkec3 / countMA;

  //////////////////////////////////////// Kec 4
  Totalkec4 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec4 += readkec4[i];
  }
  Averagekec4 = Totalkec4 / countMA;

  //////////////////////////////////////// Kec 5
  Totalkec5 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec5 += readkec5[i];
  }
  Averagekec5 = Totalkec5 / countMA;

  //////////////////////////////////////// Kec 6
  Totalkec6 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec6 += readkec6[i];
  }
  Averagekec6 = Totalkec6 / countMA;

  //////////////////////////////////////// Kec 7
  Totalkec7 = 0;
  for (int i = 0; i < countMA; i++) {
    Totalkec7 += readkec7[i];
  }
  Averagekec7 = Totalkec7 / countMA;

  //////////////////////////////////////// UKec 1
  TotalUkec1 = 0;
  for (int i = 0; i < countMA; i++) {
    TotalUkec1 += readUkec1[i];
  }
  AverageUkec1 = TotalUkec1 / countMA;

  //////////////////////////////////////// UKec 2
  TotalUkec2 = 0;
  for (int i = 0; i < countMA; i++) {
    TotalUkec2 += readUkec2[i];
  }
  AverageUkec2 = TotalUkec2 / countMA;

  suhu1 = data[0];
  kec1 = String(Averagekec1);
  Humid1 = data[2];

  suhu2 = data[3];
  kec2 = String(Averagekec2);
  Humid2 = data[5];

  suhu3 = data[6];
  kec3 = String(Averagekec3);
  Humid3 = data[8];

  suhu4 = data[9];
  kec4 = String(Averagekec4);
  Humid4 = data[11];

  suhu5 = data[12];
  kec5 = String(Averagekec5);
  Humid5 = data[14];

  suhu6 = data[15];
  kec6 = String(Averagekec6);
  Humid6 = data[17];

  suhu7 = data[18];
  kec7 = String(Averagekec7);
  Humid7 = data[20];

  Usuhu1 = data[21];
  UHumid1 = data[22]; 
  Ukec1 = String(AverageUkec1);
  USDP1 = data[24];

  Usuhu2 = data[25];
  UHumid2 = data[26]; 
  Ukec2 = String(AverageUkec2);
  USDP2 = data[28];
  // 29 tidak dipakai
  // 30 tidak dipakai
  // 31 tidak dipakai

  // ================= HEATER 1 =================

  Heater_st_1     = data[32].toInt();
  Fan_Power_1     = data[33].toInt();
  Power_Tunnel_1  = data[34].toInt();
  WCSstate1       = !data[35].toInt();
  SuhuDS1         = safeValue(data[36]).toFloat();
  BME1            = safeValue(data[37]).toFloat();
  setpoint1       = data[38].toFloat();
  kp1             = data[39].toFloat();
  ki1             = data[40].toFloat();
  kd1             = data[41].toFloat();
  out_pid1        = data[42].toFloat();
  CalM_DinD1      = data[43].toFloat();
  CalB_DinD1      = data[44].toFloat();
  Max_PID_Tunnel_1 = data[45].toInt();

  // ================= HEATER 2 =================
  Heater_st_2     = data[46].toInt();
  Fan_Power_2     = data[47].toInt();
  Power_Tunnel_2  = data[48].toInt();
  WCSstate2       = !data[49].toInt();
  SuhuDS2         = safeValue(data[50]).toFloat();
  BME2            = safeValue(data[51]).toFloat();
  setpoint2       = data[52].toFloat();
  kp2             = data[53].toFloat();
  ki2             = data[54].toFloat();
  kd2             = data[55].toFloat();
  out_pid2        = data[56].toFloat();
  CalM_DinD2      = data[57].toFloat();
  CalB_DinD2      = data[58].toFloat();
  Max_PID_Tunnel_2 = data[59].toInt();

  // ================= HEATER 3 =================
  Heater_st_3     = data[60].toInt();
  Fan_Power_3     = data[61].toInt();
  Power_Tunnel_3  = data[62].toInt();
  WCSstate3       = !data[63].toInt();
  SuhuDS3         = safeValue(data[64]).toFloat();
  BME3            = safeValue(data[65]).toFloat();
  setpoint3       = data[66].toFloat();
  kp3             = data[67].toFloat();
  ki3             = data[68].toFloat();
  kd3             = data[69].toFloat();
  out_pid3        = data[70].toFloat();
  CalM_DinD3      = data[71].toFloat();
  CalB_DinD3      = data[72].toFloat();
  Max_PID_Tunnel_3 = data[73].toInt();


  if (SuhuDS1 < 0) SuhuDS1 = 0;
  if (SuhuDS2 < 0) SuhuDS2 = 0;
  if (SuhuDS3 < 0) SuhuDS3 = 0;

  // // Cetak semua nilai yang diterima
  // for (int i = 0; i <= index; i++) {
  //   Serial.printf("data[%d] = %s\n", i, data[i].c_str());

  // }
  // isSdCardLogging = true;
  // if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
  //   vTulisKeSDcard();
  //   xSemaphoreGive(sdCardMutex);
  // }

  // accessToGoogleSheets();
  Serial.println("selesai mendapatkan data");
  printAllMACs();
  Serial.println("selesai Mengirim data");
  // reset_arr bisa diatur di sini juga kalau diperlukan
  // reset_arr = 0;
}

// void accessToGoogleSheets() {
//   // unsigned long epochTime = getCurrentTime();
//   timeUpdate = epochTime + 25200;
//   Serial.println("EPOCH :");
//   Serial.println(epochTime);
//   Serial.println("timeUpdate :");
//   Serial.println(timeUpdate);
//   HTTPClient http;
//   char tempURL[900];
//   char httpRequestData[900];
//   String URL = "https://script.google.com/macros/s/";
//   sprintf(httpRequestData, "/exec?Time=%lu000&S1=%s&S2=%s&S3=%s&S4=%s&S5=%s&S6=%s&S7=%s&SU1=%s&SU2=%s&SD1=%.2f&SD2=%.2f&SD3=%.2f&SP1=%.2f&SP2=%.2f&SP3=%.2f&H1=%s&H2=%s&H3=%s&H4=%s&H5=%s&H6=%s&H7=%s&HU1=%s&HU2=%s&K1=%s&K2=%s&K3=%s&K4=%s&K5=%s&K6=%s&K7=%s&KU1=%s&KU2=%s&PU1=%s&PU2=%s&PID1=%.2f&PID2=%.2f&PID3=%.2f&Error=%d",
//   epochTime,
//   suhu1,suhu2,suhu3,suhu4,suhu5,suhu6,suhu7,
//   Usuhu1,Usuhu2,
//   SuhuDS1,SuhuDS2,SuhuDS3,
//   BME1,BME2,BME3,
//   Humid1,Humid2,Humid3,Humid4,Humid5,Humid6,Humid7,
//   UHumid1,UHumid2,
//   kec1,kec2,kec3,kec4,kec5,kec6,kec7,
//   Ukec1,Ukec2,
//   USDP1,USDP2,
//   out_pid1,out_pid2,out_pid3,
//   CounterDataError
//   );

//   strcpy(tempURL, URL.c_str());
//   strcat(tempURL, KEY);
//   strcat(tempURL, httpRequestData);
//   SendURL = String(tempURL);
//   // Serial.println("[HTTP] begin...");
//   // Serial.println(SendURL);
//   // access to your Google Sheets
//   Serial.println();
//   // configure target server and url
//   http.begin(SendURL.c_str());
//   // Serial.println("[HTTP] GET...");
//   // start connection and send HTTP header
//   int httpCode = http.GET();
//   // httpCode will be negative on error
//   if(httpCode > 0) {
//       // HTTP header has been send and Server response header has been handled
//       Serial.print("[HTTP] GET... code: ");
//       Serial.println(httpCode);
//       // file found at server
//       if(httpCode == HTTP_CODE_OK) {
//           String payload = http.getString();
//           Serial.println(payload);
//       }
//   }
//   else {
//       Serial.print("[HTTP] GET... failed, error: ");
//       Serial.println(http.errorToString(httpCode).c_str());
//   }
// }

void vPembacaanSerial(){
  // static String serialBuffer = "";
  static char serialBuff[1000];
  static int indexChar = 0;

  while (Serial2.available()) {
    char c = Serial2.read();
    Serial.print(c);

    if (c == '<') {
      indexChar = 0;
      // serialBuffer = "";
      // char serialBuff[1000];
    } 

    
    else if (c == '>') {

      Serial.print("indexChar> ");
      Serial.println(indexChar);

      // Data lengkap diterima

      serialBuff[indexChar++] = '\0';


      Serial.println(String(serialBuff));
      parseSerialData(String(serialBuff));

      isSdCardLogging = true;
    }


    else {
        serialBuff[indexChar++] = c;
    }
  }



}

String ambilData(String data, char pemisah, int urutan) {
  stringData = 0;
  perBagianData = "";
  for (int i = 0; i < data.length(); i++){
    if (data[i] == pemisah){
      stringData++;
    }
    else if (stringData == urutan){
      perBagianData.concat(data[i]);
    }
    else if (stringData > urutan){
      return perBagianData;
      break;
    }
  }
  return perBagianData;
}

// void vCheckASCII(){
//   int str_len = data1.length()+1;
//   char databaru[str_len];
//   Serial.print("str_len = ");
//   Serial.println(CounterKoma);
//   data1.toCharArray(databaru,str_len);
//   char buffer[4*sizeof(databaru)]; //sized for the worst case scenario of each being in the hundreds plus a space between each and a null
//   char* buffPtr = buffer;
//   int Pointer = 0;
//   int ASCIICode = 0;
//   DataNotValid = 0;
//   Serial.print("sizeof(databaru)");
//   Serial.println(sizeof(databaru));
//   for(byte i = 0; i < str_len - 1; i++){
//     itoa((int)databaru[i],buffPtr,10); //convert the next character to a string and store it in the buffer
//     buffPtr += strlen(buffPtr); //move on to the position of the null character
//     DataBuffer = String(buffer);
//     DataASCII = DataBuffer.substring(Pointer,DataBuffer.length());
//     ASCIICode = DataASCII.toInt();
//     Serial.print("ASCIICode = ");
//     Serial.println(ASCIICode);
//     if (ASCIICode >=48 && ASCIICode <=57){

//     } else if (ASCIICode == 46 || ASCIICode == 44 || ASCIICode == 45 || ASCIICode == 13){
    
//     } else {
//       DataNotValid = 1;
//       Serial.print("RUSAK ASCIICode = ");
//       Serial.print(ASCIICode);
//       Serial.println("  , DATA RUSAK");
//     }
//     *buffPtr = ' '; //replace with a space
//     buffPtr++; //move on ready for next
//     Pointer += 3;
//   }
//   buffPtr--; //move back a character to where the final space (' ') is
//   *buffPtr = '\0'; //replace it with a null to terminate the string
//   Serial.println(buffer);
// }
void vCheckASCII() {
  int str_len = data1.length() + 1;
  char databaru[str_len];
  data1.toCharArray(databaru, str_len);

  char buffer[4 * str_len];
  char* buffPtr = buffer;
  buffer[0] = '\0';

  DataNotValid = 0;
  for (int i = 0; i < str_len - 1; i++) {
    int code = (int)databaru[i];
    // Serial.print("ASCIICode = ");
    // Serial.println(code);

    if ((code >= 48 && code <= 57) || code == 46 || code == 44 || code == 45 || code == 13) {
      // valid
    } else {
      DataNotValid = 1;
      Serial.print("RUSAK ASCIICode = ");
      Serial.print(code);
      Serial.println("  , DATA RUSAK");
    }
  }

  if (!DataNotValid) Serial.println("Semua karakter valid");
}





void vAmbilDataDariSerial2(){
  if (Serial2.available()>0) {
    String CheckData;
    data1 = Serial2.readStringUntil('\n');
    CheckData = data1;
    Serial.println("");
    Serial.println(data1);
    int x = CheckData.indexOf(',');
    CounterKoma=0;
    while (x!=-1){
      CounterKoma++;
      CheckData.remove(0,x+1);
      x = CheckData.indexOf(',');
    }
    Serial.print("CounterKoma = ");
    Serial.println(CounterKoma);
    vCheckASCII();
    // DataNotValid = 0;
    
    if (CounterKoma == 73 && DataNotValid == 0) {
      StatusComData = 1;
      Serial.print("StatusComData = ");
      Serial.println(StatusComData);

      int totalData = CounterKoma + 1; // jumlah elemen sebenarnya
      if (totalData > 75) totalData = 75; // jaga-jaga agar tidak keluar batas

      for (a = 0; a < totalData; a++) {
        data[a] = ambilData(data1, ',', a);
      }

      for (int i = 0; i < totalData; i++) {
        Serial.printf("data[%d]=%s,", i, data[i].c_str());
        Last_data[i] = data[i];
      }

      data1 = "";
      a = 0;
    }
    else {
      StatusComData = 0;
      CounterDataError++;
      Serial.println(" ");
      Serial.print("Data ERROR,");
      Serial.print("CounterKoma = ");
      Serial.println(CounterKoma);
      for (int i = 0; i <75 ; i++){
        data[i] = Last_data[i];
        Serial.print("Last_data[");
        Serial.print(i);
        Serial.print("]=");
        Serial.print(Last_data[i]);
        Serial.print(",");
      }
    }
    vPemindahanData();
    
    counterParsing++;
    char bufferEpochName[40];  // perbesar biar cukup
    sprintf(bufferEpochName, "/CSVFire/%lu.csv", timeUpdate);
    writeBufferTunnel(bufferEpochName);

    if (counterParsing == 12){
      timeUpdate = rtc.getEpoch();
      vTulisDataBaruFireEpoch(timeUpdate);
      vTulisDataBaruSpreadEpoch(timeUpdate);
      Serial.print("Penulisan sd card baru");
      counterParsing = 0;
    }

    // }
    // vPengambilanDataSpreadSheet();
    
  }
}

void vAwalSdCard(){
  deleteFile(SD, "/SuhuData.js");
  deleteFile(SD, "/HumidData.js");
  deleteFile(SD, "/WindData.js");
  deleteFile(SD, "/TekananData.js");
  deleteFile(SD, "/TunnelData.csv");

  // deleteFile(SD, "/SuhuPipaData.js");
  // deleteFile(SD, "/SuhuDindingData.js");
  deleteFile(SD, "/OutPID.js");
  // deleteFile(SD, "/GabunganSuhu.js");

  vTulisDataAwalSuhu();
  vTulisDataAwalHumid();
  vTulisDataAwalAngin();
  vTulisDataAwalTekanan();
  vTulisDataAwalTunnel();

  // vTulisDataAwalSuhuPipa();
  // vTulisDataAwalSuhuDinding();
  vTulisDataAwalOutPID();
  // vTulisDataAwalGabunganSuhu();
}

void vAwalSetupSdCard(){

  readFile(SD, "/SuhuData.js");
  delay(50);
  readFile(SD, "/HumidData.js");
  delay(50);
  readFile(SD, "/WindData.js");
  delay(50);
  readFile(SD, "/TekananData.js");
  delay(50);
  readFile(SD, "/TunnelData.csv");
  delay(50);
  readFile(SD, "/OutPID.js");
  delay(50);

  vTulisDataAwalSuhu();
  vTulisDataAwalHumid();
  vTulisDataAwalAngin();
  vTulisDataAwalTekanan();
  vTulisDataAwalTunnel();

  // vTulisDataAwalSuhuPipa();
  // vTulisDataAwalSuhuDinding();
  vTulisDataAwalOutPID();
  // vTulisDataAwalGabunganSuhu();
}

void vTulisKeSDcard(){
      // waktuUpdate = getCurrentTime();
      
      waktuUpdate = rtc.getEpoch();
      writeBufferTunnel("/TunnelData.csv");
      Serial.println("isSdCardLogging");
}

void vKirimFirebase(){

  // unsigned long epochTime = getCurrentTime();
  unsigned long epochTime = rtc.getEpoch();

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

  // makeCopyDataFirebase(SD, "/TunnelData.csv");
  if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
      
    renameDataFirebase(SD, "/TunnelData.csv");
    vTulisDataAwalTunnel();

    xSemaphoreGive(sdCardMutex);
  }

  // verifyCopyCRC(SD, "/TunnelData.csv");

  
}

void vKirimFirebaseTest(){

  // unsigned long epochTime = getCurrentTime();
  unsigned long epochTime = rtc.getEpoch();

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
  
  renameDataFirebase(SD, "/TunnelData.csv");
  vTulisDataAwalTunnel();

}

void vRenameDataCSV(){

  // unsigned long epochTime = getCurrentTime();
  unsigned long epochTime = rtc.getEpoch();

  Serial.print("epochTime = ");
  Serial.println(epochTime);

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
  
  if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
    renameDataFirebase(SD, "/TunnelData.csv");
    vTulisDataAwalTunnel();
    xSemaphoreGive(sdCardMutex);
  }

  // verifyCopyCRC(SD, "/TunnelData.csv");

  
}

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

void initData() {
  // set semua buffer ke 0
  for (int i = 0; i < WINDOW_SIZE; i++) {
    readkec1[i] = 0;
    readkec2[i] = 0;
    readkec3[i] = 0;
    readkec4[i] = 0;
    readkec5[i] = 0;
    readkec6[i] = 0;
    readkec7[i] = 0;
    readUkec1[i] = 0;
    readUkec2[i] = 0;

  }

  // set semua total & average ke 0
  Totalkec1 = 0;
  Averagekec1 = 0;
  Totalkec2 = 0;
  Averagekec2 = 0;
  Totalkec3 = 0;
  Averagekec3 = 0;
  Totalkec4 = 0;
  Averagekec4 = 0;
  Totalkec5 = 0;
  Averagekec5 = 0;
  Totalkec6 = 0;
  Averagekec6 = 0;
  Totalkec7 = 0;
  Averagekec7 = 0;
  TotalUkec1 = 0;
  AverageUkec1 = 0;
  TotalUkec2 = 0;
  AverageUkec2 = 0;

  // reset index & counter
  indexMA = 0;
  countMA = 0;
}

void refreshWiFi() {
  Serial.println("\n[WiFi] Memutus koneksi...");
  // WiFi.disconnect(true, true); // putus dan hapus konfigurasi
  delay(50);

  Serial.println("[WiFi] Menghubungkan kembali...");
  WiFi.begin(ssid, password);

  // unsigned long startAttemptTime = millis();

  // Tunggu koneksi maksimal 10 detik
  // while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
  //   // delay(500);
  //   // Serial.print(".");
  // }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Terhubung kembali!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WiFi] Gagal reconnect.");
  }
}

void VsetupPID(){
  PIDFan1.SetOutputLimits(0, 180);
  PIDFan1.SetSampleTime(10000);
  PIDFan1.SetMode(AUTOMATIC);

  PIDFan2.SetOutputLimits(0, 180);
  PIDFan2.SetSampleTime(10000);
  PIDFan2.SetMode(AUTOMATIC);
}

void vFormatWaktu(){
  unsigned long epochTime = rtc.getEpoch();
  Serial.print("epochTime = ");
  Serial.println(epochTime);
  struct tm timeinfo;
  gmtime_r((time_t*)&epochTime, &timeinfo);  // pecah epoch ke tm
  currentYear  = timeinfo.tm_year + 1900;
  currentMonth = timeinfo.tm_mon + 1;
  currentDay   = timeinfo.tm_mday;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_INDX, OUTPUT);
  Serial.begin(115200);
  
  

  vSetupLittlefs();
  vSetupSDcard();
  // vAwalSdCard();
  vAwalSetupSdCard();
  initData();
  // for (int i = 0; i < WINDOW_SIZE; i++) {
  //   readings[i] = 0;
  // }

  vSetupEEPROM();
  errorcount++;
  preferences.putInt("errorcount", errorcount);
  vSetupWifi();
  timeClient.begin();
  timeClient.update();
  // getCurrentTime();
  rtc.setTime(getCurrentTime());
  // unsigned long waktuse = rtc.getEpoch();
  // Serial.println("waktu epoch =");
  // Serial.println(waktuse);

  vSetupServo();
  // VsetupPID();
  vAsyncWebServer();
  // vSetupEspNow();
  // ensureNvsReady();
  // preferences.clear();
  // nvs_flash_erase();
  // nvs_flash_init();
  // loadInfoUmum();
  vSetupADS();  //harus nyala
  sdCardMutex = xSemaphoreCreateMutex();

  timeUpdate = rtc.getEpoch();
  vTulisDataBaruFireEpoch(timeUpdate);
  vTulisDataBaruSpreadEpoch(timeUpdate);
  Serial.println("Selesai membuat file!");
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  IntervalKirimServerMillis = 60000;

  // vKirimFirebase();
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  // IntervalKirimServerMillis = 60000;
  // vRenameDataCSV();

  //   unsigned long epochTime = getCurrentTime();
  //   rtc.setTime(epochTime);
  //   Serial.println("Connection successful!");
  //   // deleteFile(SD, "/TunnelData.csv");
  //   // vTulisDataAwalTunnel();
  //   char fileNameUpload[80] = "";
  //   char fileLocUpload[80] = "";
  //   snprintf(fileNameUpload, sizeof(fileNameUpload), "Up_Tunnel_%04d-%02d-%02d.csv",
  //           currentYear, currentMonth, currentDay);
  //   snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d.csv",
  //           currentYear, currentMonth, currentDay);

  // unsigned long cTime = millis();
  // if (vBacaFileDariSD(fileLocUpload)) {
  //   vSendFirebase();
  // }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Serial.print("millis taken for sending to fb>> ");
  // Serial.println(millis() - cTime);

  xTaskCreatePinnedToCore(
    TaskSendData,   /* Task function. */
    "TaskSendData",     /* name of task. */
    20480,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    NULL,      /* Task handle to keep track of created task */
    1);          /* pin task to core 0 */
}

void TaskSendData( void * pvParameters ){
  (void) pvParameters;
  TickType_t xLastWakeTime;
  TickType_t xFrequency = 1000/ portTICK_PERIOD_MS;
  xLastWakeTime = xTaskGetTickCount ();
  for(;;){
    if (!bOTAStart) {
      if(bPertamaKirim){
        bPertamaKirim = 0;
        
      }else{
        // if(counterParsing >= 12){
        //   digitalWrite(LED_INDX, HIGH);
        //   BanyakDataPengiriman = counterParsing;
        //   counterParsing = 0;
        //   vPengirimanSpreadsheet();
        //   if(part > 0){
        //     vFormatWaktu();
        //     Serial.println("Masuk part!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        //     Serial.print("part = ");
        //     Serial.println(part);
        //     for(int i = 0; i < part; i++){
        //       char fileLocUpload[80] = "";
        //       snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d-Part%d.csv",
        //       currentYear, currentMonth, currentDay, i+1);
        //       Serial.println("Nama file part =");
        //       Serial.println(fileLocUpload);
        //       if (vBacaFileDariSD(fileLocUpload)) {
        //         vSendFirebasePart(fileLocUpload);
        //       }
        //     }
        //   }
        //   Serial.println("Beres part#############################################################################");
        //   vRenameDataCSV();
        //   Serial.println("Rename successful!");
        //   char fileLocUpload[80] = "";
        //   snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d.csv",
        //           currentYear, currentMonth, currentDay);
        //   if (vBacaFileDariSD(fileLocUpload)) {
        //     vSendFirebase();
        //   }
          
        //   preferences.putInt("part", part);
        //   Serial.printf("Stack free TaskSendData: %d\n", uxTaskGetStackHighWaterMark(NULL));
        //   digitalWrite(LED_INDX, LOW);
        // }
      }
    }
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
  }
}

void loop() {
  // put your main code here, to run repeatedly:
    if(millis() - ADSMillis >= IntervalADSMillis){
      vPembacaanADS();  //harus nyala

      vReconnectWifi();
      Serial.print("Part = ");
      Serial.println(part);
      Serial.print("error count restarted = ");
      Serial.println(errorcount);
      Serial.print("counterParsing = ");
      Serial.println(counterParsing);
      Serial.print("Nama csv = ");
      Serial.print(timeUpdate);
      Serial.print(".csv");
      Serial.print("Free Heap: ");
      
      Serial.println(ESP.getFreeHeap());
      Serial.println("LOOP SELESAI");
      ADSMillis = millis();
    }

    vAmbilDataDariSerial2();
    

    // if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
    //   // if(millis() - KirimServerMillis >= IntervalKirimServerMillis && !bSedangKirimData){
    //   //   // vKirimFirebase();
    //   //   bSedangKirimData = 1;
    //   //   // v_SendFile();
    //   //   // bSedangKirimData = 0;
    //   //   Serial.println("SELESAI Mengirim File");
    //   //   KirimServerMillis = millis();
    //   // }
    //   // vPembacaanSerial();
    //   xSemaphoreGive(sdCardMutex);
    // }
}

void v_SendFile() {
  // Use this only for HTTPS
  // client.setInsecure();//skip verification because our server changes rootCA every month or so. Skip this as long as we get certificate we accept
  if (client.connect(serverName, serverPort)) {
    Serial.print("Awal start :");
    Serial.println(millis());

    unsigned long epochTime = getCurrentTime();
    rtc.setTime(epochTime);

    Serial.println("Connection successful!");
    // deleteFile(SD, "/TunnelData.csv");
    // vTulisDataAwalTunnel();
    char fileNameUpload[80] = "";
    char fileLocUpload[80] = "";
    snprintf(fileNameUpload, sizeof(fileNameUpload), "Up_Tunnel_%04d-%02d-%02d.csv",
            currentYear, currentMonth, currentDay);
    snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d.csv",
            currentYear, currentMonth, currentDay);
    

    // File file = SD.open(fileLocUpload, FILE_READ);

    // // CRC32 dari file
    // String CRC32File = vCheckCRCfile(SD, fileLocUpload, 10000);

    File file;
    String CRC32File;

    if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){

      file = SD.open(fileLocUpload, FILE_READ);
      CRC32File = vCheckCRCfile(SD, fileLocUpload, 10000);
      file.close();
      // CRC32File = "9e769355";
      xSemaphoreGive(sdCardMutex);
    }

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
    // fileLen = file.size();

    // readFile(SD,fileLocUpload); 
    if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
      file = SD.open(fileLocUpload, FILE_READ);
      fileLen = file.size();
      file.close();
      xSemaphoreGive(sdCardMutex);
    }


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
    // while (file.available()) {
    //   char c= file.read();
    //   client.write(c);
    //   Serial.write(c);
    // }



    // while (file.available()) {
    //     size_t bytesRead = file.read(buffer, bufferSize);
    //     client.write(buffer, bytesRead);
    //   Serial.write(buffer, bytesRead);
    // }

    if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
      const size_t bufferSize = 256; // Reduce if needed
      uint8_t buffer[bufferSize];
      // uint8_t buffer[40][256];
      file = SD.open(fileLocUpload, FILE_READ);
      while (file.available()) {
          size_t bytesRead = file.read(buffer, bufferSize);
          client.write(buffer, bytesRead);
        Serial.write(buffer, bytesRead);
      }
      file.close();
      xSemaphoreGive(sdCardMutex);
    }
    //Use this for HTTP
    // client.write(file);


    client.flush();
    Serial.flush();
    client.print(tail);
    Serial.println(tail);
    // file.close();

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


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // while ((startTimer + timoutTimer) > millis()) {
    //   Serial.print(".");
    //   vTaskDelay(100);
    //   const size_t BUFFER_SIZE = 256;     // ukuran chunk
    //   char buf[BUFFER_SIZE];
    //   int len;

    //   while (client.available()) {
    //     char c = client.read();
    //     Serial.print(c);


    //      if (c == '\n') {
    //         headerLine[headerIndex] = '\0';  // end the line

    //         if (!statusCaptured) {
    //           // Example: "HTTP/1.1 200 OK"
    //           // Skip "HTTP/1.1 " (9 chars) → status starts at index 9
    //           strncpy(httpStatus, headerLine + 9, 3);
    //           httpStatus[3] = '\0';  // null terminate
    //           statusCaptured = true;


    //         }

    //         headerIndex = 0;  // reset for next header
    //       }
    //       else if (c != '\r') {
    //         if (headerIndex < 128 - 1) {
    //           headerLine[headerIndex++] = c;
    //         }
    //       }

    //     if(c == '{'){
    //       state = true;
    //     }
    //     if(state){
    //       getResponse[getResponseIndex] = c;
    //       getResponseIndex++;
    //     }
    //   }
    //   if(state == true && !client.available()){
    //     break;
    //   }

    // }

    // while ((startTimer + timoutTimer) > millis()) {
    //     Serial.print(".");
    //     vTaskDelay(100);

    //     const size_t BUFFER_SIZE = 256;     // ukuran chunk
    //     char buf[BUFFER_SIZE];
    //     int len;

    //     while (client.available()) {
    //       len = client.readBytes(buf, BUFFER_SIZE);  // baca maksimal 256 byte sekali

    //       for (int i = 0; i < len; i++) {
    //         char c = buf[i];
    //         Serial.write(c);

    //         // --- parsing header ---
    //         if (!state) {   // masih di header
    //           if (c == '\n') {
    //             headerLine[headerIndex] = '\0';  // akhiri line

    //             if (!statusCaptured) {
    //               // contoh: "HTTP/1.1 200 OK"
    //               if (strncmp(headerLine, "HTTP/1.1 ", 9) == 0) {
    //                 strncpy(httpStatus, headerLine + 9, 3);
    //                 httpStatus[3] = '\0';
    //                 statusCaptured = true;
    //               }
    //             }

    //             headerIndex = 0;  // reset line buffer
    //           }
    //           else if (c != '\r') {
    //             if (headerIndex < sizeof(headerLine) - 1) {
    //               headerLine[headerIndex++] = c;
    //             }
    //           }
    //         }

    //         // --- deteksi awal JSON body ---
    //         if (c == '{') {
    //           state = true;
    //         }

    //         // --- simpan body JSON ---
    //         if (state) {
    //           if (getResponseIndex < sizeof(getResponse) - 1) {
    //             getResponse[getResponseIndex++] = c;
    //             getResponse[getResponseIndex] = '\0';  // null terminate
    //           } else {
    //             Serial.println("\n!!! Buffer getResponse penuh, hentikan baca !!!");
    //             break;
    //           }
    //         }
    //       }

    //       // kasih waktu ke watchdog
    //       vTaskDelay(1);
    //     }

    //   // kalau sudah mulai JSON dan tidak ada data lagi, keluar
    //   if (state == true && !client.available()) {
    //     break;
    //   }
    // }


    // Serial.println();

    // bstopLoop = 1;
    // Serial.print("HTTP Status: ");
    // Serial.println(httpStatus);  // should print "200"

    // Serial.print("getResponse>> ");
    // for (int i = 0; i < getResponseIndex; i++){
    //   Serial.print(getResponse[i]);
    // }
    // Serial.println("");
    //   if (getResponseIndex > 0) {
    //     // terminasi string
    //     getResponse[getResponseIndex] = '\0';

    //     // === Panggil fungsi parsing yang sudah kamu buat ===
    //     vParsingResponseWebV2(getResponse);
    //   }


    // // vParsingResponseWebV2(getResponse);

    // // parseResponse();

    // Serial.println();
    
    // client.stop();
           ////////////////////////////////////////////////////////////////////////// 


    while ((startTimer + timoutTimer) > millis()) {
      Serial.print(".");
      vTaskDelay(100);

      const size_t BUFFER_SIZE = 256;     // ukuran chunk
      char buf[BUFFER_SIZE];
      int len;

      while (client.available()) {
        len = client.readBytes(buf, BUFFER_SIZE);  // baca maksimal 256 byte sekali

        for (int i = 0; i < len; i++) {
          char c = buf[i];
          Serial.write(c);

          // --- parsing header ---
          if (!state) {   // masih di header
            if (c == '\n') {
              headerLine[headerIndex] = '\0';  // akhiri line

              if (!statusCaptured) {
                // contoh: "HTTP/1.1 200 OK"
                if (strncmp(headerLine, "HTTP/1.1 ", 9) == 0) {
                  strncpy(httpStatus, headerLine + 9, 3);
                  httpStatus[3] = '\0';
                  statusCaptured = true;
                }
              }

              headerIndex = 0;  // reset line buffer
            }
            else if (c != '\r') {
              if (headerIndex < sizeof(headerLine) - 1) {
                headerLine[headerIndex++] = c;
              }
            }
          }

          // --- deteksi awal JSON body ---
          if (c == '{') {
            state = true;
          }

          // --- simpan body JSON ---
          if (state) {
            if (getResponseIndex < sizeof(getResponse) - 1) {
              getResponse[getResponseIndex++] = c;
              getResponse[getResponseIndex] = '\0';  // null terminate
            } else {
              Serial.println("\n!!! Buffer getResponse penuh, hentikan baca !!!");
              break;
            }
          }
        }

        // kasih waktu ke watchdog
        vTaskDelay(1);
      }

      // kalau sudah mulai JSON dan tidak ada data lagi, keluar
      if (state == true && !client.available()) {
        break;
      }
    }

    // --- selesai ambil feedback ---
    Serial.println();
    Serial.print("HTTP Status: ");
    Serial.println(httpStatus);
    int statusCode = atoi(httpStatus); // konversi ke int

    

    Serial.print("getResponse>> ");
    Serial.println(getResponse);

    

    // tutup koneksi
    client.stop();

    // RESET semua state biar siap untuk loop berikutnya
    headerIndex = 0;
    
    state = false;
    statusCaptured = false;
    httpStatus[0] = '\0';
    memset(headerLine, 0, sizeof(headerLine));
    // memset(getResponse, 0, sizeof(getResponse));
    memset(getResponse, 0, sizeof(getResponse));

    Serial.println("=== Feedback selesai & state sudah direset ===");

    // deleteFile(SD, "/TunnelData.csv");
    // vTulisDataAwalTunnel();
    
    if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
      if(statusCode != 200){
        // appendAndReplace(SD, "/TunnelData.csv",  fileLocUpload);
        char partLocUpload[80] = "";
        part++;
        preferences.putInt("part", part);
        snprintf(partLocUpload, sizeof(partLocUpload), "/Up_Tunnel_%04d-%02d-%02d-Part%d.csv",
                currentYear, currentMonth, currentDay, part);
        
        renameFile(SD, fileLocUpload, partLocUpload );
        
      }
      
      else {
        deleteFile(SD, fileLocUpload);
        // panggil parser kalau ada data
        if (getResponseIndex > 0) {
          vParsingResponseWebV2(getResponse);
        }
      }
      
      xSemaphoreGive(sdCardMutex);
    }
    
    getResponseIndex = 0;
    bstopLoop = 0;
    Serial.println("jalankan loop");
    part = 0;

  } else {
    
    Serial.println("Not Connected");
    refreshWiFi();
    char fileLocUpload[80] = "";
    char partLocUpload[80] = "";
    part++;
    preferences.putInt("part", part);
    snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d.csv",
            currentYear, currentMonth, currentDay);

    snprintf(partLocUpload, sizeof(partLocUpload), "/Up_Tunnel_%04d-%02d-%02d-Part%d.csv",
            currentYear, currentMonth, currentDay, part);
    // appendAndReplace(SD, "/TunnelData.csv",  fileLocUpload);
    
    if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
      renameFile(SD, fileLocUpload, partLocUpload );
      xSemaphoreGive(sdCardMutex);
    }
    vReconnectWifi();
  }
  Serial.print("Awal start :");
  Serial.println(millis());
}

bool vBacaFileDariSD(const char* fileLocUpload ) {
    if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
      File file;
      file = SD.open(fileLocUpload, FILE_READ);
      if (!file) {
        Serial.println("Gagal membuka file SD!");
        xSemaphoreGive(sdCardMutex);
        return false;
      }

      Serial.println("Membaca file dari SD...");
      for (int i = 0; i < ROWS; i++) {
        if (!file.available()) {
          bytesPerRow[i] = 0;
          break;
        }
        bytesPerRow[i] = file.read(buffer[i], COL_SIZE);
      }

      file.close();
      Serial.println("Pembacaan file selesai.");
      xSemaphoreGive(sdCardMutex);
      return true;
    }
    
}


void vSendFirebase() {
  // Use this only for HTTPS
  // client.setInsecure();//skip verification because our server changes rootCA every month or so. Skip this as long as we get certificate we accept
  if (client.connect(serverName, serverPort)) {
    Serial.print("Awal start :");
    Serial.println(millis());
    
    unsigned long epochTime = getCurrentTime();
    rtc.setTime(epochTime);
    Serial.println("Connection successful!");

    char fileNameUpload[80] = "";
    char fileLocUpload[80] = "";
    snprintf(fileNameUpload, sizeof(fileNameUpload), "Up_Tunnel_%04d-%02d-%02d.csv",
            currentYear, currentMonth, currentDay);
    snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d.csv",
            currentYear, currentMonth, currentDay);

    File file;
    String CRC32File;

    if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){

      file = SD.open(fileLocUpload, FILE_READ);
      CRC32File = vCheckCRCfile(SD, fileLocUpload, 10000);
      file.close();
      // CRC32File = "9e769355";
      xSemaphoreGive(sdCardMutex);
    }

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
    
    if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
      file = SD.open(fileLocUpload, FILE_READ);
      fileLen = file.size();
      file.close();
      xSemaphoreGive(sdCardMutex);
    }


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

    Serial.println("Mengirim data buffer ke client...");
    size_t totalBytes = 0;
    for (int i = 0; i < ROWS; i++) {
      if (bytesPerRow[i] == 0) break; // tidak ada data lagi
      client.write(buffer[i], bytesPerRow[i]);
      Serial.write(buffer[i], bytesPerRow[i]);
      totalBytes += bytesPerRow[i];
    }
    Serial.print("Selesai kirim buffer. Total byte: ");
    Serial.println(totalBytes); 

    client.flush();
    Serial.flush();
    client.print(tail);
    Serial.println(tail);
    // file.close();

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

      const size_t BUFFER_SIZE = 256;     // ukuran chunk
      char buf[BUFFER_SIZE];
      int len;

      while (client.available()) {
        len = client.readBytes(buf, BUFFER_SIZE);  // baca maksimal 256 byte sekali

        for (int i = 0; i < len; i++) {
          char c = buf[i];
          Serial.write(c);

          // --- parsing header ---
          if (!state) {   // masih di header
            if (c == '\n') {
              headerLine[headerIndex] = '\0';  // akhiri line

              if (!statusCaptured) {
                // contoh: "HTTP/1.1 200 OK"
                if (strncmp(headerLine, "HTTP/1.1 ", 9) == 0) {
                  strncpy(httpStatus, headerLine + 9, 3);
                  httpStatus[3] = '\0';
                  statusCaptured = true;
                }
              }
              headerIndex = 0;  // reset line buffer
            }
            else if (c != '\r') {
              if (headerIndex < sizeof(headerLine) - 1) {
                headerLine[headerIndex++] = c;
              }
            }
          }

          // --- deteksi awal JSON body ---
          if (c == '{') {
            state = true;
          }

          // --- simpan body JSON ---
          if (state) {
            if (getResponseIndex < sizeof(getResponse) - 1) {
              getResponse[getResponseIndex++] = c;
              getResponse[getResponseIndex] = '\0';  // null terminate
            } else {
              Serial.println("\n!!! Buffer getResponse penuh, hentikan baca !!!");
              break;
            }
          }
        }

        // kasih waktu ke watchdog
        vTaskDelay(1);
      }

      // kalau sudah mulai JSON dan tidak ada data lagi, keluar
      if (state == true && !client.available()) {
        break;
      }
    }

    // --- selesai ambil feedback ---
    Serial.println();
    Serial.print("HTTP Status: ");
    Serial.println(httpStatus);
    int statusCode = atoi(httpStatus); // konversi ke int

    

    Serial.print("getResponse>> ");
    Serial.println(getResponse);

    

    // tutup koneksi
    client.stop();

    // RESET semua state biar siap untuk loop berikutnya
    headerIndex = 0;
    
    state = false;
    statusCaptured = false;
    httpStatus[0] = '\0';
    

    Serial.println("=== Feedback selesai & state sudah direset ===");

    // deleteFile(SD, "/TunnelData.csv");
    // vTulisDataAwalTunnel();
    
    // if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
      if(statusCode != 200){
        // appendAndReplace(SD, "/TunnelData.csv",  fileLocUpload);
        char partLocUpload[80] = "";
        part++;
        preferences.putInt("part", part);
        snprintf(partLocUpload, sizeof(partLocUpload), "/Up_Tunnel_%04d-%02d-%02d-Part%d.csv",
                currentYear, currentMonth, currentDay, part);

        if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
          renameFile(SD, fileLocUpload, partLocUpload );
          xSemaphoreGive(sdCardMutex);
        }
      }
      else {
        if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
          deleteFile(SD, fileLocUpload);
          xSemaphoreGive(sdCardMutex);
        }
        if (getResponseIndex > 0) {
          vParsingResponseWebV2(getResponse); // vSendFirebase
        }
        part = 0;
        preferences.putInt("part", part);
      }

    memset(headerLine, 0, sizeof(headerLine));
    // memset(getResponse, 0, sizeof(getResponse));
    memset(getResponse, 0, sizeof(getResponse));
      
    //   xSemaphoreGive(sdCardMutex);
    // }
    
    getResponseIndex = 0;
    bstopLoop = 0;
    Serial.println("jalankan loop");
    

  } else {
    
    Serial.println("Not Connected");
    refreshWiFi();
    char fileLocUpload[80] = "";
    char partLocUpload[80] = "";
    part++;
    preferences.putInt("part", part);
    snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d.csv",
            currentYear, currentMonth, currentDay);

    snprintf(partLocUpload, sizeof(partLocUpload), "/Up_Tunnel_%04d-%02d-%02d-Part%d.csv",
            currentYear, currentMonth, currentDay, part);
    // appendAndReplace(SD, "/TunnelData.csv",  fileLocUpload);
    
    if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
      renameFile(SD, fileLocUpload, partLocUpload );
      xSemaphoreGive(sdCardMutex);
    }
    vReconnectWifi();
  }
  Serial.print("Awal start :");
  Serial.println(millis());
}

void vSendFirebasePart(const char* fileLocUpload) {
  if (client.connect(serverName, serverPort)) {
    
    unsigned long epochTime = getCurrentTime();
    rtc.setTime(epochTime);
    Serial.println("Connection successful!");

    char fileNameUpload[80] = "";
    // char fileLocUpload[80] = "";

    snprintf(fileNameUpload, sizeof(fileNameUpload), "Up_Tunnel_%04d-%02d-%02d-Part%d.csv",
            currentYear, currentMonth, currentDay, index+1);
    // snprintf(fileLocUpload, sizeof(fileLocUpload), "/Up_Tunnel_%04d-%02d-%02d-Part%d.csv",
    //         currentYear, currentMonth, currentDay, index+1);

    Serial.println("fileNameUpload =");
    Serial.println(fileNameUpload);
    Serial.println("Dalam void firebase part =");
    Serial.println(fileLocUpload);

    File file;
    String CRC32File;

    if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
      file = SD.open(fileLocUpload, FILE_READ);
      CRC32File = vCheckCRCfile(SD, fileLocUpload, 10000);
      file.close();
      xSemaphoreGive(sdCardMutex);
    }

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
    

    if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
      file = SD.open(fileLocUpload, FILE_READ);
      fileLen = file.size();
      file.close();
      xSemaphoreGive(sdCardMutex);
    }


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

    Serial.println("Mengirim data buffer ke client...");
    size_t totalBytes = 0;
    for (int i = 0; i < ROWS; i++) {
      if (bytesPerRow[i] == 0) break; // tidak ada data lagi
      client.write(buffer[i], bytesPerRow[i]);
      Serial.write(buffer[i], bytesPerRow[i]);
      totalBytes += bytesPerRow[i];
    }
    Serial.print("Selesai kirim buffer. Total byte: ");
    Serial.println(totalBytes); 

    client.flush();
    Serial.flush();
    client.print(tail);
    Serial.println(tail);
    
    int timoutTimer = 10000;
    unsigned long startTimer = millis();
    boolean state = false;

    char headerLine[128];
    char httpStatus[4];
    int headerIndex = 0;
    bool statusCaptured = false;

    while ((startTimer + timoutTimer) > millis()) {
      Serial.print(".");
      vTaskDelay(100);

      const size_t BUFFER_SIZE = 256;     // ukuran chunk
      char buf[BUFFER_SIZE];
      int len;

      while (client.available()) {
        len = client.readBytes(buf, BUFFER_SIZE);  // baca maksimal 256 byte sekali

        for (int i = 0; i < len; i++) {
          char c = buf[i];
          Serial.write(c);

          if (!state) {   // masih di header
            if (c == '\n') {
              headerLine[headerIndex] = '\0';  // akhiri line

              if (!statusCaptured) {
                // contoh: "HTTP/1.1 200 OK"
                if (strncmp(headerLine, "HTTP/1.1 ", 9) == 0) {
                  strncpy(httpStatus, headerLine + 9, 3);
                  httpStatus[3] = '\0';
                  statusCaptured = true;
                }
              }

              headerIndex = 0;  // reset line buffer
            }
            else if (c != '\r') {
              if (headerIndex < sizeof(headerLine) - 1) {
                headerLine[headerIndex++] = c;
              }
            }
          }

          if (c == '{') {
            state = true;
          }

          // --- simpan body JSON ---
          if (state) {
            if (getResponseIndex < sizeof(getResponse) - 1) {
              getResponse[getResponseIndex++] = c;
              getResponse[getResponseIndex] = '\0';  // null terminate
            } else {
              Serial.println("\n!!! Buffer getResponse penuh, hentikan baca !!!");
              break;
            }
          }
        }
        vTaskDelay(1);
      }

      // kalau sudah mulai JSON dan tidak ada data lagi, keluar
      if (state == true && !client.available()) {
        break;
      }
    }

    // --- selesai ambil feedback ---
    Serial.println();
    Serial.print("HTTP Status: ");
    Serial.println(httpStatus);
    int statusCode = atoi(httpStatus); // konversi ke int

    

    Serial.print("getResponse>> ");
    Serial.println(getResponse);
    client.stop();

    if(statusCode == 200){
      if (getResponseIndex > 0) {
        vParsingResponseWebV2(getResponse);
      }
      if(xSemaphoreTake(sdCardMutex, pdMS_TO_TICKS(50000)) == pdTRUE){
        deleteFile(SD, fileLocUpload);
        xSemaphoreGive(sdCardMutex);

      }
    }
    
    // RESET semua state biar siap untuk loop berikutnya
    headerIndex = 0;
    getResponseIndex = 0;
    state = false;
    statusCaptured = false;
    httpStatus[0] = '\0';
    memset(headerLine, 0, sizeof(headerLine));
    // memset(getResponse, 0, sizeof(getResponse));
    memset(getResponse, 0, sizeof(getResponse));
    getResponseIndex = 0;

    Serial.println("=== Feedback selesai & state sudah direset ===");
    bstopLoop = 0;
    Serial.println("jalankan loop");

  } else {
    Serial.println("Not Connected");
    refreshWiFi();
  }
}

void vParsingResponseWebV2(const char* jsonResponse){
  // Tentukan jumlah sensor dan repeater berdasarkan JSON
  const int jumlahSensor = 15;
  const int jumlahRepeater = 0;

  // Deklarasi array untuk menyimpan data dari JSON sebagai float
  float calTempM[jumlahSensor + 1];
  float calHumB[jumlahSensor + 1];
  float calPresM[jumlahSensor + 1];
  float calVelM[jumlahSensor + 1];
  float calHumM[jumlahSensor + 1];
  float calVelB[jumlahSensor + 1];
  float calTempB[jumlahSensor + 1];
  float calPresB[jumlahSensor + 1];
  float calBat[jumlahSensor + 1];

  // Deklarasi array untuk repeater
  float calBatRepeater[jumlahRepeater + 1];

  // Variabel untuk menyimpan data lainnya
  int lastVersion;
  int totalNode;
  int totalRepeater;
  float calBatPusat;
  int timeReportPeriodic;
  int timePeriodicSensor;
  int jumlahSensorParsed;
  String crc32web;

  // Parse JSON
  // DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, jsonResponse);
  if (error) {
    Serial.print(F("Failed to parse JSON: "));
    Serial.println(error.f_str());
    return;
  }

  // Ambil data umum
  lastVersion = doc["lastVersion"].as<int>();
  totalNode = doc["totalNode"].as<int>();
  totalRepeater = doc["totalRepeater"].as<int>();
  calBatPusat = doc["calBatPusat"].as<float>();
  timeReportPeriodic = doc["timeReportPeriodic"].as<int>();
  timePeriodicSensor = doc["timePeriodicSensor"].as<int>();
  jumlahSensorParsed = doc["jumlahSensor"].as<int>();
  crc32web = doc["crc32web"].as<String>();
  
  // Ambil dan cetak data sensor
  Serial.println("Sensor Data:");
  JsonObject sensor = doc["sensor"];

  for (int i = 0; i < jumlahSensorParsed; i++) {
    JsonObject sensorData = sensor[String(i)];
    calTempM[i + 1] = sensorData["calTempM"].as<float>();
    calHumB[i + 1] = sensorData["calHumB"].as<float>();
    calPresM[i + 1] = sensorData["calPresM"].as<float>();
    calVelM[i + 1] = sensorData["calVelM"].as<float>();
    calHumM[i + 1] = sensorData["calHumM"].as<float>();
    calVelB[i + 1] = sensorData["calVelB"].as<float>();
    calTempB[i + 1] = sensorData["calTempB"].as<float>();
    calPresB[i + 1] = sensorData["calPresB"].as<float>();
    calBat[i + 1] = sensorData["calBat"].as<float>();
  }


  iTotalNode = totalNode;
  iTotalRepeater = totalRepeater;
  iLastVersion = lastVersion;
  iJumlahSensor = jumlahSensorParsed;
  iTimeReportPeriodic = timeReportPeriodic;
  iTimePeriodicSensor = timePeriodicSensor;
  fCalBatPusat = calBatPusat;

  IntervalKirimServerMillis = iTimeReportPeriodic * 60000;


  strcpy(CRC32Web, crc32web.c_str());

  Serial.println("Update :");
  Serial.printf("lastVersion: %d\n", iLastVersion);
  Serial.printf("totalNode: %d\n", iTotalNode);
  Serial.printf("totalRepeater: %d\n", iTotalRepeater);
  Serial.printf("calBatPusat: %.3f\n", fCalBatPusat);
  Serial.printf("timeReportPeriodic: %d\n", iTimeReportPeriodic);
  Serial.printf("timePeriodicSensor: %d\n", iTimePeriodicSensor);
  Serial.printf("jumlahSensor: %d\n", iJumlahSensor);
  Serial.printf("Crc32web: %s\n", CRC32Web);

  
  // Output data sensor
  Serial.println("\nSensor Data:");
  for (int i = 1; i < jumlahSensorParsed +1; i++) {
    // Serial.printf("Sensor %d: calTempM=%.3f, calHumB=%.3f, calPresM=%.3f, calVelM=%.3f, calHumM=%.3f, calVelB=%.3f, calTempB=%.3f, calPresB=%.3f, calBat=%.3f\n",
    //               i, calTempM[i], calHumB[i], calPresM[i], calVelM[i], calHumM[i], calVelB[i], calTempB[i], calPresB[i], calBat[i]);
    fCalBatS[i] = calBat[i];
    fCalTempM[i] = calTempM[i];
    fCalTempB[i] = calTempB[i];
    fCalHumM[i] = calHumM[i];
    fCalHumB[i] = calHumB[i];
    fCalVeloM[i] = calVelM[i];
    fCalVeloB[i] = calVelB[i];
    fCalPresM[i] = calPresM[i];
    fCalPresB[i] = calPresB[i];
    Serial.printf("Sensor %d: fCalTempM=%.3f, fCalHumB=%.3f, fCalPresM=%.3f, fCalVelM=%.3f, fCalHumM=%.3f, fCalVelB=%.3f, fCalTempB=%.3f, fCalPresB=%.3f, fCalBat=%.3f\n",
                  i, fCalTempM[i], fCalHumB[i], fCalPresM[i], fCalVeloM[i], fCalHumM[i], fCalVeloB[i], fCalTempB[i], fCalPresB[i], fCalBatS[i]);
  }
  Serial.println(" ");
  doc.clear();
}

void vPengambilanDataSpreadSheet(){
  Serial.println("Pengambilan data untuk spreadsheet");
  Serial.print("untuk data ke ");
  Serial.println(counterParsing);
  unsigned long baseEpoch = rtc.getEpoch();
  char epochMs[20];
    sprintf(epochMs, "%lu000", baseEpoch);
    Serial.print("epochMs :");
    Serial.println(epochMs);

    arraywaktu[counterParsing] = String(epochMs);
    arrayJson[ 1][counterParsing] = suhu1.toFloat();
    arrayJson[ 2][counterParsing] = suhu2.toFloat();
    arrayJson[ 3][counterParsing] = suhu3.toFloat();
    arrayJson[ 4][counterParsing] = suhu4.toFloat();
    arrayJson[ 5][counterParsing] = suhu5.toFloat();
    arrayJson[ 6][counterParsing] = suhu6.toFloat();
    arrayJson[ 7][counterParsing] = suhu7.toFloat();
    arrayJson[ 8][counterParsing] = Usuhu1.toFloat();
    arrayJson[ 9][counterParsing] = Usuhu2.toFloat();
    arrayJson[10][counterParsing] = SuhuDS1;
    arrayJson[11][counterParsing] = SuhuDS2;
    arrayJson[12][counterParsing] = SuhuDS3;
    arrayJson[13][counterParsing] = BME1;
    arrayJson[14][counterParsing] = BME2;
    arrayJson[15][counterParsing] = BME3;
    arrayJson[16][counterParsing] = Humid1.toFloat();
    arrayJson[17][counterParsing] = Humid2.toFloat();
    arrayJson[18][counterParsing] = Humid3.toFloat();
    arrayJson[19][counterParsing] = Humid4.toFloat();
    arrayJson[20][counterParsing] = Humid5.toFloat();
    arrayJson[21][counterParsing] = Humid6.toFloat();
    arrayJson[22][counterParsing] = Humid7.toFloat();
    arrayJson[23][counterParsing] = UHumid1.toFloat();
    arrayJson[24][counterParsing] = UHumid2.toFloat();
    arrayJson[25][counterParsing] = kec1.toFloat();
    arrayJson[26][counterParsing] = kec2.toFloat();
    arrayJson[27][counterParsing] = kec3.toFloat();
    arrayJson[28][counterParsing] = kec4.toFloat();
    arrayJson[29][counterParsing] = kec5.toFloat();
    arrayJson[30][counterParsing] = kec6.toFloat();
    arrayJson[31][counterParsing] = kec7.toFloat();
    arrayJson[32][counterParsing] = Ukec1.toFloat();
    arrayJson[33][counterParsing] = Ukec2.toFloat();
    arrayJson[34][counterParsing] = USDP1.toFloat();
    arrayJson[35][counterParsing] = USDP2.toFloat();
    arrayJson[36][counterParsing] = out_pid1;
    arrayJson[37][counterParsing] = out_pid2;
    arrayJson[38][counterParsing] = out_pid3;
    arrayJson[39][counterParsing] = CounterDataError;

    for (int j = 0; j < counterParsing +1; j++) {
    Serial.printf("Data ke-%d: ", j);
    Serial.printf("Epoch-> %s: ", arraywaktu[j]);
    for (int i = 0; i < 40; i++) {
      Serial.print(arrayJson[i][j], 2);
      if (i < 39) Serial.print(", ");
    }
    Serial.println();
  }
}

void vPengirimanSpreadsheet() {
  Serial.println("\n--- Mulai Upload Spreadsheet ---");
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    return;
  }

  JsonArray rows = doc["rows"].as<JsonArray>();
  if (rows.isNull()) {
    rows = doc.createNestedArray("rows");
  }
  
  for (int i = 0; i < BanyakDataPengiriman; i++) {
    JsonObject row = rows.createNestedObject();

    row["Time"] = arraywaktu[i];
    row["S1"] = arrayJson[1][i];
    row["S2"] = arrayJson[2][i];
    row["S3"] = arrayJson[3][i];
    row["S4"] = arrayJson[4][i];
    row["S5"] = arrayJson[5][i];
    row["S6"] = arrayJson[6][i];
    row["S7"] = arrayJson[7][i];
    row["SU1"] = arrayJson[8][i];
    row["SU2"] = arrayJson[9][i];
    row["SD1"] = arrayJson[10][i];
    row["SD2"] = arrayJson[11][i];
    row["SD3"] = arrayJson[12][i];
    row["SP1"] = arrayJson[13][i];
    row["SP2"] = arrayJson[14][i];
    row["SP3"] = arrayJson[15][i];
    row["H1"] = arrayJson[16][i];
    row["H2"] = arrayJson[17][i];
    row["H3"] = arrayJson[18][i];
    row["H4"] = arrayJson[19][i];
    row["H5"] = arrayJson[20][i];
    row["H6"] = arrayJson[21][i];
    row["H7"] = arrayJson[22][i];
    row["HU1"] = arrayJson[23][i];
    row["HU2"] = arrayJson[24][i];
    row["K1"] = arrayJson[25][i];
    row["K2"] = arrayJson[26][i];
    row["K3"] = arrayJson[27][i];
    row["K4"] = arrayJson[28][i];
    row["K5"] = arrayJson[29][i];
    row["K6"] = arrayJson[30][i];
    row["K7"] = arrayJson[31][i];
    row["KU1"] = arrayJson[32][i];
    row["KU2"] = arrayJson[33][i];
    row["PU1"] = arrayJson[34][i];
    row["PU2"] = arrayJson[35][i];
    row["PID1"] = arrayJson[36][i];
    row["PID2"] = arrayJson[37][i];
    row["PID3"] = arrayJson[38][i];
    row["Error"] = arrayJson[39][i];
    delay(1);
  }

  String payload;
  serializeJson(doc, payload);

  uint32_t TotalPanjangJson = payload.length();

  // --- Koneksi ke Google Script ---
  if (!client.connect(serverName, serverPort)) {
    Serial.println("Gagal konek ke server!");
    return;
  }

  // --- Kirim HTTP Header ---
  client.print("POST ");
  client.print(serverPathSpreadSheet);
  client.println(" HTTP/1.1");

  client.print("Host: ");
  client.println(serverNameSpreadSheet);
  client.println("User-Agent: ESP32");
  client.println("Connection: close");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(TotalPanjangJson);
  client.println(); // pemisah header-body

  // --- Kirim body JSON ---
  Serial.println("payload =");
  Serial.println(payload);
  client.print(payload);
  client.println();

  Serial.println("✅ Data dikirim, menunggu respons...");

  // --- Tunggu respons dari server ---
  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 5000) {
    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
  }

  client.stop();
  Serial.println("--- Upload selesai ---");
}



