/*
  1. perbaikan versi ini, merapikan program agar diblock dan dipindahkan setiap fungsinya
  2. menambahkan eeprom
  3. menambahkan servo

*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <SD.h>
#include <SPI.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Time.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <ESP32Servo.h>

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

#define RX_PIN 16
#define TX_PIN 17

float Arus1 = random(0, 5000) / 100.0;     // 0.00–50.00 A
float Arus2 = random(0, 5000) / 100.0;
float Arus3 = random(0, 5000) / 100.0;
float Arus4 = random(0, 5000) / 100.0;

int adc0 = random(0, 4096);
int adc1 = random(0, 4096);
int adc2 = random(0, 4096);
int adc3 = random(0, 4096);

float SetPointInput1 = random(2000, 8000) / 100.0; // 20.00–80.00
float SetPointInput2 = random(2000, 8000) / 100.0;
float SetPointInput3 = random(2000, 8000) / 100.0;
float SetPointInput4 = random(2000, 8000) / 100.0;

float Watt1 = random(0, 2000) / 10.0;
float Watt2 = random(0, 2000) / 10.0;
float Watt3 = random(0, 2000) / 10.0;
float Watt4 = random(0, 2000) / 10.0;

int kecepatan1 = random(0, 3000);
int kecepatan2 = random(0, 3000);
int kecepatan3 = random(0, 3000);
int kecepatan4 = random(0, 3000);

int WCSstate1;
int WCSstate2;
int WCSstate3;

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

float setpoint1, setpoint2, setpoint3;
float kp1, kp2, kp3;
float ki1, ki2, ki3;
float kd1, kd2, kd3;
float out_pid1, out_pid2, out_pid3;

float CalM_DinD1 = 1 ;
float CalB_DinD1 = 0 ;

float CalM_DinD2 = 1 ;
float CalB_DinD2 = 0;

float CalM_DinD3 = 1 ;
float CalB_DinD3 = 0 ;

float calM_Wcs1 =  0.004351 ;
float calB_Wcs1 =  - 85.004 ;

float calM_Wcs2 = 0.004351 ;
float calB_Wcs2 = - 85.004 ;

float calM_Wcs3 =  0.004351 ;
float calB_Wcs3 =  - 85.004 ;

float calM_Wcs4 =  0.004351 ;
float calB_Wcs4 =  - 85.004 ;

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

float SuhuDS1;
float SuhuDS2;
float SuhuDS3;

float  BME1;
float  BME2;
float  BME3;

String SData;
unsigned long timeUpdate;
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
int counterParsing = 0; 
String perBagianData;
ESP32Time rtc;
Preferences preferences;
Servo servo1, servo2, servo3, servo4;
int errorcount = 0; 
int part = 0;
int LED_INDX = 2;
// waktu
WiFiUDP ntpUDP;
DynamicJsonDocument doc(10580);
JsonArray rows = doc.createNestedArray("rows");
double arrayJson[40][12];
String arraywaktu[12];

const long utcOffsetInSeconds = 0;
unsigned long waktuUpdate = 0;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// ---------- WiFi Credentials ----------
// Replace with your WiFi network SSID and password
const char* WIFI_SSID = "Web Tunnel Monitoring";
const char* WIFI_PASSWORD = "12345678";

// ---------- Server ----------
AsyncWebServer webServer(80);

// ---------- Remote upload target (HTTPS) ----------
static const char* SERVER_NAME = "asia-east1-tambang-2b501.cloudfunctions.net"; // host only
static const char* SERVER_PATH = "/Tambang/periodic-csv/1C692094D05C?feedback=all"; // path starting with '/'
static const int SERVER_PORT = 443; // 443 for HTTPS
static const char* SD_FILE_PATH = "/CSVFire/1760601368.csv"; // change to your SD file

WiFiClientSecure httpsClient;

// ---------- Helper: wait for WiFi connection with timeout ----------
bool waitForWiFi(IPAddress& acquiredIp, uint32_t timeoutMs) {
  const uint32_t startMs = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startMs) < timeoutMs) {
    delay(250);
    Serial.print('.');
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    acquiredIp = WiFi.localIP();
    return true;
  }
  return false;
}

// ---------- HTTP POST helper using HTTPClient (C-strings) ----------
// Returns true on 2xx success, false otherwise. Response body copied into responseOut (null-terminated).
bool postData(const char* url,
              const char* payload,
              const char* contentType,
              char* responseOut,
              size_t responseOutSize,
              int& httpStatusCodeOut) {
  if (responseOut && responseOutSize > 0) {
    responseOut[0] = '\0';
  }

  if (WiFi.status() != WL_CONNECTED) {
    httpStatusCodeOut = -1;
    if (responseOut && responseOutSize > 0) {
      strncpy(responseOut, "WiFi not connected", responseOutSize - 1);
      responseOut[responseOutSize - 1] = '\0';
    }
    return false;
  }

  HTTPClient httpClient;
  bool beginOk = httpClient.begin(url);
  if (!beginOk) {
    httpStatusCodeOut = -2;
    if (responseOut && responseOutSize > 0) {
      strncpy(responseOut, "Failed to begin HTTP connection", responseOutSize - 1);
      responseOut[responseOutSize - 1] = '\0';
    }
    return false;
  }

  // addHeader requires Arduino String; create minimal temporaries from C-strings
  httpClient.addHeader(String("Content-Type"), String(contentType));

  int httpCode = httpClient.POST((uint8_t*)payload, strlen(payload));
  httpStatusCodeOut = httpCode;

  if (httpCode > 0) {
    // Read body into buffer without using Arduino String
    WiFiClient* stream = httpClient.getStreamPtr();
    size_t totalRead = 0;
    uint32_t start = millis();
    const uint32_t readTimeoutMs = 2000; // 2s budget to drain body
    while (httpClient.connected() && (millis() - start) < readTimeoutMs) {
      size_t availableBytes = stream->available();
      if (availableBytes) {
        size_t toRead = availableBytes;
        if (responseOut && responseOutSize > 0) {
          if (toRead > (responseOutSize - 1) - totalRead) {
            toRead = (responseOutSize - 1) - totalRead;
          }
          size_t justRead = stream->readBytes((uint8_t*)responseOut + totalRead, toRead);
          totalRead += justRead;
          if (totalRead >= responseOutSize - 1) {
            break;
          }
        } else {
          // Discard if no buffer provided
          uint8_t discard[64];
          size_t chunk = toRead > sizeof(discard) ? sizeof(discard) : toRead;
          stream->readBytes(discard, chunk);
        }
      } else {
        delay(10);
      }
    }
    if (responseOut && responseOutSize > 0) {
      responseOut[totalRead < responseOutSize ? totalRead : responseOutSize - 1] = '\0';
    }
    httpClient.end();
    return httpCode >= 200 && httpCode < 300;
  } else {
    if (responseOut && responseOutSize > 0) {
      // Provide a generic error without relying on String APIs
      snprintf(responseOut, responseOutSize, "HTTP error: %d", httpCode);
      responseOut[responseOutSize - 1] = '\0';
    }
    httpClient.end();
    return false;
  }
}

// HTML is served from LittleFS: /index.html

// ---------- HTTPS multipart upload of a file from SD ----------
// Boundary is "SnoveLab" to match the reference; adjust form field names as needed by your server
bool sendFileFromSD(const char* host,
                    int port,
                    const char* path,
                    const char* filePathOnSd) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return false;
  }

  File file = SD.open(filePathOnSd, FILE_READ);
  if (!file) {
    Serial.print("Failed to open SD file: ");
    Serial.println(filePathOnSd);
    return false;
  }

  // Insecure to simplify CA rotation handling
  httpsClient.setInsecure();
  if (!httpsClient.connect(host, port)) {
    Serial.println("HTTPS connect failed");
    file.close();
    return false;
  }

  const char* boundary = "SnoveLab";
  // Prepare parts (C-strings)
  char headFile[256];
  char headCRC[128];
  char headTimestamp[160];
  char headSignal[128];
  char tail[64];

  const char* filename = file.name();
  snprintf(headFile, sizeof(headFile),
           "--%s\r\nContent-Disposition: form-data; name=\"File\"; filename=\"%s\"\r\nContent-Type: text/csv\r\n\r\n",
           boundary, filename ? filename : "upload.csv");
  snprintf(headCRC, sizeof(headCRC),
           "--%s\r\nContent-Disposition: form-data; name=\"CRC32\"\r\n\r\n%s\r\n",
           boundary, "4e2835f7");
  snprintf(headTimestamp, sizeof(headTimestamp),
           "--%s\r\nContent-Disposition: form-data; name=\"timestamp\"\r\n\r\n%s\r\n",
           boundary, "123456");
  snprintf(headSignal, sizeof(headSignal),
           "--%s\r\nContent-Disposition: form-data; name=\"signal\"\r\n\r\n%s",
           boundary, "20");
  snprintf(tail, sizeof(tail), "\r\n--%s--\r\n", boundary);

  uint32_t fileLen = file.size();
  uint32_t extraLen = strlen(headFile) + strlen(headCRC) + strlen(headTimestamp) + strlen(headSignal) + strlen(tail);
  uint32_t totalLen = fileLen + extraLen;

  // Write request line and headers
  httpsClient.print("POST "); httpsClient.print(path); httpsClient.println(" HTTP/1.1");
  httpsClient.print("Host: "); httpsClient.println(host);
  httpsClient.println("Connection: close");
  {
    char clBuf[32];
    snprintf(clBuf, sizeof(clBuf), "%lu", (unsigned long)totalLen);
    httpsClient.print("Content-Length: "); httpsClient.println(clBuf);
  }
  httpsClient.print("Content-Type: multipart/form-data; boundary="); httpsClient.println(boundary);
  httpsClient.println();

  // Write multipart body
  httpsClient.print(headCRC);
  httpsClient.print(headFile);

  // Stream file content
  const size_t bufSize = 1024;
  uint8_t buf[bufSize];
  while (file.available()) {
    size_t n = file.read(buf, bufSize);
    if (n > 0) {
      httpsClient.write(buf, n);
    }
  }
  httpsClient.println();

  httpsClient.print(headTimestamp);
  httpsClient.print(headSignal);
  httpsClient.print(tail);
  file.close();

  // Read response (optional)
  char respBuf[512];
  size_t respLen = 0;
  unsigned long startMs = millis();
  const unsigned long timeoutMs = 10000;
  while (millis() - startMs < timeoutMs) {
    while (httpsClient.available()) {
      char c = httpsClient.read();
      if (respLen < sizeof(respBuf) - 1) {
        respBuf[respLen++] = c;
      }
      startMs = millis();
    }
    if (respLen > 0) break;
    delay(50);
  }
  respBuf[respLen] = '\0';
  Serial.println(respBuf);

  httpsClient.stop();
  return true;
}

// ---------- Periodic file upload from SD ----------
const uint32_t UPLOAD_INTERVAL_MS = 30000; // 30 seconds
unsigned long lastUploadMs = 0;

String vSendData() {
  char data[150];  // Sesuaikan ukuran buffer jika perlu
  sprintf(data, "%.2f,%.2f,%.2f,%.2f,%d,%d,%d,%d,%.2f,%.2f,%.2f,%d,%d,%d,%d,%d,%d,%.5f,%.5f,%.5f,%d,%d,%d",  
  // Arus1, Arus2, Arus3, Arus4, 
  // adc0, adc1, adc2, adc3,
  // SetPointInput1, SetPointInput2, SetPointInput3, SetPointInput4, 
  Watt1, Watt2, Watt3, Watt4, 
  kecepatan1, kecepatan2, kecepatan3, kecepatan4, 
  out_pid1,out_pid2,out_pid3,
  Heater_st_1, Heater_st_2, Heater_st_3,
  Fan_Power_1, Fan_Power_2, Fan_Power_3,
  setpoint1, setpoint2, setpoint3,
  WCSstate1, WCSstate2, WCSstate3
  );
  SData = String(data);
  // Serial.print("WebSet = ");
  // Serial.println(SData);

  return SData;
}

String safeValueSTR(String val) {
  // static String lastVal = "0";    // menyimpan nilai terakhir valid
  // static int dropCount = 0;       // menghitung drop berturut-turut

  val.trim();
  if (val.length() == 0 || val == "nan" || val == "-1.00" || val == "-999.00") return "0";

  return val;
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    // Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        // Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
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

void writeBufferSpreadSheet(const char* path) {

    JsonArray rows = doc["rows"].as<JsonArray>();
    if (rows.isNull()) {
      rows = doc.createNestedArray("rows");
    }
    for (int i = 0; i < counterParsing; i++) {
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

    Serial.println("Buffer tunnel data");
    Serial.println(payload);
    
    appendFile(SD, path, payload.c_str());
    // tulis ke file
    // appendFile(SD, "/TunnelData.csv", buffer);
  
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

void vPemindahanData(){
  // readkec1[indexMA] = safeValueSTR(data[1]).toFloat();
  // readkec2[indexMA] = safeValueSTR(data[4]).toFloat();
  // readkec3[indexMA] = safeValueSTR(data[7]).toFloat();
  // readkec4[indexMA] = safeValueSTR(data[10]).toFloat();
  // readkec5[indexMA] = safeValueSTR(data[13]).toFloat();
  // readkec6[indexMA] = safeValueSTR(data[16]).toFloat();
  // readkec7[indexMA] = safeValueSTR(data[19]).toFloat();
  // readUkec1[indexMA] = safeValueSTR(data[23]).toFloat();
  // readUkec2[indexMA] = safeValueSTR(data[27]).toFloat();

  // indexMA = (indexMA + 1) % WINDOW_SIZE;
  // if (countMA < WINDOW_SIZE) countMA++;
  // //////////////////////////////////////// Kec 1
  // Totalkec1 = 0;
  // for (int i = 0; i < countMA; i++) {
  //   Totalkec1 += readkec1[i];
  // }
  // Averagekec1 = Totalkec1 / countMA;

  // //////////////////////////////////////// Kec 2
  // Totalkec2 = 0;
  // for (int i = 0; i < countMA; i++) {
  //   Totalkec2 += readkec2[i];
  // }
  // Averagekec2 = Totalkec2 / countMA;

  // //////////////////////////////////////// Kec 3
  // Totalkec3 = 0;
  // for (int i = 0; i < countMA; i++) {
  //   Totalkec3 += readkec3[i];
  // }
  // Averagekec3 = Totalkec3 / countMA;

  // //////////////////////////////////////// Kec 4
  // Totalkec4 = 0;
  // for (int i = 0; i < countMA; i++) {
  //   Totalkec4 += readkec4[i];
  // }
  // Averagekec4 = Totalkec4 / countMA;

  // //////////////////////////////////////// Kec 5
  // Totalkec5 = 0;
  // for (int i = 0; i < countMA; i++) {
  //   Totalkec5 += readkec5[i];
  // }
  // Averagekec5 = Totalkec5 / countMA;

  // //////////////////////////////////////// Kec 6
  // Totalkec6 = 0;
  // for (int i = 0; i < countMA; i++) {
  //   Totalkec6 += readkec6[i];
  // }
  // Averagekec6 = Totalkec6 / countMA;

  // //////////////////////////////////////// Kec 7
  // Totalkec7 = 0;
  // for (int i = 0; i < countMA; i++) {
  //   Totalkec7 += readkec7[i];
  // }
  // Averagekec7 = Totalkec7 / countMA;

  // //////////////////////////////////////// UKec 1
  // TotalUkec1 = 0;
  // for (int i = 0; i < countMA; i++) {
  //   TotalUkec1 += readUkec1[i];
  // }
  // AverageUkec1 = TotalUkec1 / countMA;

  // //////////////////////////////////////// UKec 2
  // TotalUkec2 = 0;
  // for (int i = 0; i < countMA; i++) {
  //   TotalUkec2 += readUkec2[i];
  // }
  // AverageUkec2 = TotalUkec2 / countMA;

  suhu1 = data[0];
  kec1 = data[1];
  Humid1 = data[2];

  suhu2 = data[3];
  kec2 = data[4];
  Humid2 = data[5];

  suhu3 = data[6];
  kec3 = data[7];
  Humid3 = data[8];

  suhu4 = data[9];
  kec4 = data[10];
  Humid4 = data[11];

  suhu5 = data[12];
  kec5 = data[13];
  Humid5 = data[14];

  suhu6 = data[15];
  kec6 = data[16];
  Humid6 = data[17];

  suhu7 = data[18];
  kec7 = data[19];
  Humid7 = data[20];

  Usuhu1 = data[21];
  UHumid1 = data[22]; 
  Ukec1 = data[23];
  USDP1 = data[24];

  Usuhu2 = data[25];
  UHumid2 = data[26]; 
  Ukec2 = data[27];
  USDP2 = data[28];
  // 29 tidak dipakai
  // 30 tidak dipakai
  // 31 tidak dipakai

  // ================= HEATER 1 =================

  Heater_st_1     = data[32].toInt();
  Fan_Power_1     = data[33].toInt();
  Power_Tunnel_1  = data[34].toInt();
  WCSstate1       = !data[35].toInt();
  SuhuDS1         = safeValueSTR(data[36]).toFloat();
  BME1            = safeValueSTR(data[37]).toFloat();
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
  SuhuDS2         = safeValueSTR(data[50]).toFloat();
  BME2            = safeValueSTR(data[51]).toFloat();
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
  SuhuDS3         = safeValueSTR(data[64]).toFloat();
  BME3            = safeValueSTR(data[65]).toFloat();
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
  // printAllMACs();
  Serial.println("selesai Mengirim data");
  // reset_arr bisa diatur di sini juga kalau diperlukan
  // reset_arr = 0;
}

void writeBufferTunnel(const char* path) {
      
    waktuUpdate = rtc.getEpoch();
    char buffer[1000];  // perbesar biar cukup
    sprintf(buffer,
          "1,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "2,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "3,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "4,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "5,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "6,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "7,%lu,nan,%s,%s,%s,nan,nan,nan,nan\n"
          "8,%lu,nan,%s,%s,%s,%s,1,2\n"
          "9,%lu,nan,%s,%s,%s,%s,1,2\n"
          "10,%lu,nan,%.2f,nan,nan,nan,nan,nan\n"
          "11,%lu,nan,%.2f,nan,nan,nan,nan,nan\n"
          "12,%lu,nan,%.2f,nan,nan,nan,nan,nan\n"
          "13,%lu,nan,%.2f,nan,nan,nan,nan,nan\n"
          "14,%lu,nan,%.2f,nan,nan,nan,nan,nan\n"
          "15,%lu,nan,%.2f,nan,nan,nan,nan,nan\n",
          waktuUpdate, safeValueSTR(suhu1).c_str(), safeValueSTR(Humid1).c_str(), safeValueSTR(kec1).c_str(),
          waktuUpdate, safeValueSTR(suhu2).c_str(), safeValueSTR(Humid2).c_str(), safeValueSTR(kec2).c_str(),
          waktuUpdate, safeValueSTR(suhu3).c_str(), safeValueSTR(Humid3).c_str(), safeValueSTR(kec3).c_str(),
          waktuUpdate, safeValueSTR(suhu4).c_str(), safeValueSTR(Humid4).c_str(), safeValueSTR(kec4).c_str(),
          waktuUpdate, safeValueSTR(suhu5).c_str(), safeValueSTR(Humid5).c_str(), safeValueSTR(kec5).c_str(),
          waktuUpdate, safeValueSTR(suhu6).c_str(), safeValueSTR(Humid6).c_str(), safeValueSTR(kec6).c_str(),
          waktuUpdate, safeValueSTR(suhu7).c_str(), safeValueSTR(Humid7).c_str(), safeValueSTR(kec7).c_str(),
          waktuUpdate, safeValueSTR(Usuhu1).c_str(), safeValueSTR(UHumid1).c_str(), safeValueSTR(Ukec1).c_str(),  safeValueSTR(USDP1).c_str(),
          waktuUpdate, safeValueSTR(Usuhu2).c_str(), safeValueSTR(UHumid2).c_str(), safeValueSTR(Ukec2).c_str(),  safeValueSTR(USDP2).c_str(),
          waktuUpdate,SuhuDS1,
          waktuUpdate,SuhuDS2,
          waktuUpdate,SuhuDS3,
          waktuUpdate,BME1,
          waktuUpdate,BME2,
          waktuUpdate,BME3 );

    Serial.println("Buffer tunnel data");
    Serial.println(buffer);
    
    appendFile(SD, path, buffer);
    // tulis ke file
    // appendFile(SD, "/TunnelData.csv", buffer);
  
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void vTulisDataBaruFireEpoch(unsigned long NamaEpochCSV) {
    char bufferEpochName[40];  // perbesar biar cukup
    sprintf(bufferEpochName, "/CSVFire/%lu.csv", NamaEpochCSV);
    writeFile(SD, bufferEpochName, "");
}

void vTulisDataBaruSpreadEpoch(unsigned long NamaEpochCSV) {
    char bufferEpochName[40];  // perbesar biar cukup
    sprintf(bufferEpochName, "/CSVSpread/%lu.txt", NamaEpochCSV);
    writeFile(SD, bufferEpochName, "");
}

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
    vPengambilanDataSpreadSheet();

    counterParsing++;
    char bufferEpochName[40];  // perbesar biar cukup
    sprintf(bufferEpochName, "/CSVFire/%lu.csv", timeUpdate);
    writeBufferTunnel(bufferEpochName);


    if (counterParsing == 12){
      Serial.print("awal penulisan = ");
      Serial.println(millis());
      sprintf(bufferEpochName, "/CSVSpread/%lu.txt", timeUpdate);
      writeBufferSpreadSheet(bufferEpochName);

      timeUpdate = rtc.getEpoch();
      vTulisDataBaruFireEpoch(timeUpdate);
      vTulisDataBaruSpreadEpoch(timeUpdate);
      Serial.println("Penulisan sd card baru");
      counterParsing = 0;
      Serial.print("akhir penulisan = ");
      Serial.println(millis());
    }  
  }
}

unsigned long getCurrentTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;

}
void vSetupWifi(){
  // WiFi Station mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Connecting to WiFi SSID '%s'", WIFI_SSID);
  IPAddress ip;
  if (!waitForWiFi(ip, 20000)) { // wait up to 20 seconds
    Serial.println("Failed to connect to WiFi.");
  } else {
    Serial.print("Connected. IP Address: ");
    Serial.println(ip);
  }
}

void vSetupLittleFS(){
  // Routes
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed");
  } else {
    Serial.println("LittleFS mounted");
  }
}

void vAsyncwebserver(){
  // Serve static files from LittleFS root; default file is index.html
  webServer.serveStatic("/", LittleFS, "/").setDefaultFile("user.html");
  
  webServer.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){

    String Dsuhu1 = String(SuhuDS1);
    String Dsuhu2 = String(SuhuDS2);
    String Dsuhu3 = String(SuhuDS3);
    String SBME1 = String(BME1);
    String SBME2 = String(BME2);
    String SBME3 = String(BME3);


    char json[800];  // pastikan buffer cukup besar
      sprintf(json,
        "{"
        "\"time\":%lu000,"
        "\"suhu1\":%s,"
        "\"suhu2\":%s,"
        "\"suhu3\":%s,"
        "\"suhu4\":%s,"
        "\"suhu5\":%s,"
        "\"suhu6\":%s,"
        "\"suhu7\":%s,"

        "\"Dsuhu1\":%s,"
        "\"Dsuhu2\":%s,"
        "\"Dsuhu3\":%s,"

        "\"Psuhu1\":%s,"
        "\"Psuhu2\":%s,"
        "\"Psuhu3\":%s,"

        "\"Humid1\":%s,"
        "\"Humid2\":%s,"
        "\"Humid3\":%s,"
        "\"Humid4\":%s,"
        "\"Humid5\":%s,"
        "\"Humid6\":%s,"
        "\"Humid7\":%s,"
        "\"UHumid1\":%s,"
        "\"UHumid2\":%s,"

        "\"kec1\":%s,"
        "\"kec2\":%s,"
        "\"kec3\":%s,"
        "\"kec4\":%s,"
        "\"kec5\":%s,"
        "\"kec6\":%s,"
        "\"kec7\":%s,"

        "\"Usuhu1\":%s,"
        "\"Usuhu2\":%s,"
        "\"Ukec1\":%s,"
        "\"Ukec2\":%s,"
        "\"USDP1\":%s,"
        "\"USDP2\":%s"
        "}",

        // ======== Data di bawah ini sesuai urutan di atas ========
        (unsigned long)(waktuUpdate + 25200),
        safeValueSTR(suhu1), safeValueSTR(suhu2), safeValueSTR(suhu3), safeValueSTR(suhu4), safeValueSTR(suhu5), safeValueSTR(suhu6), safeValueSTR(suhu7),
        safeValueSTR(Dsuhu1), safeValueSTR(Dsuhu2), safeValueSTR(Dsuhu3),
        safeValueSTR(SBME1), safeValueSTR(SBME2), safeValueSTR(SBME3),
        safeValueSTR(Humid1), safeValueSTR(Humid2), safeValueSTR(Humid3), safeValueSTR(Humid4), safeValueSTR(Humid5), safeValueSTR(Humid6), safeValueSTR(Humid7),
        safeValueSTR(UHumid1), safeValueSTR(UHumid2),
        safeValueSTR(kec1), safeValueSTR(kec2), safeValueSTR(kec3), safeValueSTR(kec4), safeValueSTR(kec5), safeValueSTR(kec6), safeValueSTR(kec7),
        safeValueSTR(Usuhu1), safeValueSTR(Usuhu2),
        safeValueSTR(Ukec1), safeValueSTR(Ukec2),
        safeValueSTR(USDP1), safeValueSTR(USDP2)
      );
    
    request->send(200, "application/json", json);
    Serial.println(json);

    // for (int j = 0; j < 22; j++) {
    //   data[j] = "";  // Clear for next cycle
  });
  
  webServer.on("/dataSet", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", vSendData().c_str());
  });
    // Start server
  webServer.begin();
  Serial.println("Async WebServer started.");
}

void vSetupSDCard(){
  // Mount SD card
  if (!SD.begin()) {
    Serial.println("SD mount failed");
  } else {
    Serial.println("SD mounted");
  }
}

void vSetupEEPROM(){
  preferences.begin("my-app", false);

  part = preferences.getInt("part", 0);

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

  Serial.println("eeprom>> ");
  Serial.println("calM_Wcs1: " + String(calM_Wcs1, 6));
  Serial.println("calB_Wcs1: " + String(calB_Wcs1, 6));
  Serial.println("calM_Wcs2: " + String(calM_Wcs2, 6));
  Serial.println("calB_Wcs2: " + String(calB_Wcs2, 6));
  Serial.println("calM_Wcs3: " + String(calM_Wcs3, 6));
  Serial.println("calB_Wcs3: " + String(calB_Wcs3, 6));
  Serial.println("calM_Wcs4: " + String(calM_Wcs4, 6));
  Serial.println("calB_Wcs4: " + String(calB_Wcs4, 6));


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

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(200);
  Serial.println();
  Serial.println("Booting...");
  vSetupWifi();
  timeClient.begin();
  timeClient.update();
  rtc.setTime(getCurrentTime());

  vSetupLittleFS();
  vAsyncwebserver();
  vSetupSDCard();
  // vSetupEEPROM();
  // errorcount++;
  // preferences.putInt("errorcount", errorcount);
  vSetupServo();

  vTulisDataBaruFireEpoch(timeUpdate);
  vTulisDataBaruSpreadEpoch(timeUpdate);

  
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long nowMs = millis();
    if (nowMs - lastUploadMs >= UPLOAD_INTERVAL_MS) {
      lastUploadMs = nowMs;
      Serial.println("Uploading file over HTTPS...");
      bool ok = sendFileFromSD(SERVER_NAME, SERVER_PORT, SERVER_PATH, SD_FILE_PATH);
      Serial.println(ok ? "Upload done" : "Upload failed");
    }
  }
  vAmbilDataDariSerial2();
}