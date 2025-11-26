#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <QuickEspNow.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "MainWebServer.h"
#include "ControlingWebserver.h"
#include <Adafruit_ADS1X15.h>
#include <ESP32Servo.h>
#include <Preferences.h>

Preferences preferences;

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

void vSetupLittlefs(){
  // Inisialisasi LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Gagal mount LittleFS!");
    return;
  }
}

void vSetupWifi(){
  // Serial2.begin(115200);
  // WiFi.mode(WIFI_MODE_STA);
  // WiFi.disconnect(false, true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("Test Fan web 2", "12345678");

  Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
  Serial.println(WiFi.softAPIP());
  //   WiFi.begin("Marnov", "jujurdanamanah");
  // Serial.print("Connecting to WiFi ..");
  //   while (WiFi.status() != WL_CONNECTED) {
  //     Serial.print('.');
  //     delay(1000);
  //   }
  // Serial.println(WiFi.localIP());
  Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
  Serial.println(WiFi.softAPIP());

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

String vSendData() {
  char data[150];  // Sesuaikan ukuran buffer jika perlu
  sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%d,%d,%d,%d", Safety, Watt1, Watt2, Watt3, Watt4, kecepatan1, kecepatan2, kecepatan3, kecepatan4, calM_Wcs1, calM_Wcs2, calM_Wcs3, calM_Wcs4, calB_Wcs1, calB_Wcs2, calB_Wcs3, calB_Wcs4, Arus1, Arus2, Arus3, Arus4);
  SData = String(data);
  Serial.print("WebSet = ");
  Serial.println(SData);

  return SData;
}

String vSendDataHeat() {
  char data[150];  // Sesuaikan ukuran buffer jika perlu

  DindSuhu1 = SuhuDS1;
  DindSuhu2 = SuhuDS2;
  DindSuhu3 = SuhuDS3;

  sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.2f,%.2f,%.2f,%d,%d,%d", Suhu_Tunnel_1, Power_Tunnel_1, Heater_st_1, Fan_Power_1, Suhu_Tunnel_2, Power_Tunnel_2, Heater_st_2, Fan_Power_2, Suhu_Tunnel_3, Power_Tunnel_3, Heater_st_3, Fan_Power_3, CalM_DinD1, CalB_DinD1, CalM_DinD2, CalB_DinD2, CalM_DinD3, CalB_DinD3, DindSuhu1, DindSuhu2, DindSuhu3, WCSstate1, WCSstate2, WCSstate3);
  SDataHeat = String(data);
  Serial.print("SDataHeat = ");
  Serial.println(SDataHeat);

  return SDataHeat;
}

void vAsyncWebServer() {
  // server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  server.serveStatic("/highcharts.js", LittleFS, "/highcharts.js");
  server.serveStatic("/exporting.js", LittleFS, "/exporting.js");
  server.serveStatic("/highcharts-more.js", LittleFS, "/highcharts-more.js");

    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
      String suhu1 = data[0];
      String kec1 = data[1];
      String suhu2 = data[2];
      String kec2 = data[3];
      String suhu3 = data[4];
      String kec3 = data[5];
      String suhu4 = data[6];
      String kec4 =  data[7];
      String suhu5 = data[8];
      String kec5 = data[9];
      String suhu6 = data[10];
      String kec6 = data[11];
      String suhu7 = data[12];
      String kec7 = data[13];

      String Usuhu1 = data[14];
      String Ukec1 = data[15];
      String USDP1 = data[16];
      String Usuhu2 = data[17];
      String Ukec2 = data[18];
      String USDP2 = data[19];


      // String json = "{";
      // json += "\"suhu1\":" + String(suhu1, 2)+ ",";
      // json += "\"suhu2\":" + String(suhu2, 2)+ ",";
      // json += "\"suhu3\":" + String(suhu3, 2)+ ",";
      // json += "\"suhu4\":" + String(suhu4, 2)+ ",";
      // json += "\"suhu5\":" + String(suhu5, 2)+ ",";
      // json += "\"suhu6\":" + String(suhu6, 2)+ ",";
      // json += "\"suhu7\":" + String(suhu7, 2)+ ",";

      // json += "\"kec1\":" + String(kec1, 2)+ ",";
      // json += "\"kec2\":" + String(kec2, 2)+ ",";
      // json += "\"kec3\":" + String(kec3, 2)+ ",";
      // json += "\"kec4\":" + String(kec4, 2)+ ",";
      // json += "\"kec5\":" + String(kec5, 2)+ ",";
      // json += "\"kec6\":" + String(kec6, 2)+ ",";
      // json += "\"kec7\":" + String(kec7, 2);

      String json = "{";
      json += "\"suhu1\":" + safeValue(suhu1) + ",";
      json += "\"suhu2\":" + safeValue(suhu2) + ",";
      json += "\"suhu3\":" + safeValue(suhu3) + ",";
      json += "\"suhu4\":" + safeValue(suhu4) + ",";
      json += "\"suhu5\":" + safeValue(suhu5) + ",";
      json += "\"suhu6\":" + safeValue(suhu6) + ",";
      json += "\"suhu7\":" + safeValue(suhu7) + ",";

      json += "\"kec1\":" + safeValue(kec1)+ ",";
      json += "\"kec2\":" + safeValue(kec2)+ ",";
      json += "\"kec3\":" + safeValue(kec3)+ ",";
      json += "\"kec4\":" + safeValue(kec4)+ ",";
      json += "\"kec5\":" + safeValue(kec5)+ ",";
      json += "\"kec6\":" + safeValue(kec6)+ ",";
      json += "\"kec7\":" + safeValue(kec7)+ ",";

      json += "\"Usuhu1\":" + safeValue(Usuhu1)+ ",";
      json += "\"Usuhu2\":" + safeValue(Usuhu2)+ ",";
      json += "\"Ukec1\":" + safeValue(Ukec1)+ ",";
      json += "\"Ukec2\":" + safeValue(Ukec2)+ ",";
      json += "\"USDP1\":" + safeValue(USDP1)+ ",";
      json += "\"USDP2\":" + safeValue(USDP2);

      json += "}";
      request->send(200, "application/json", json);
      Serial.println(json);

      // for (int j = 0; j < 22; j++) {
      //   data[j] = "";  // Clear for next cycle
      // }
      reset_arr = 0;
    });

    server.on("/dataDin", HTTP_GET, [](AsyncWebServerRequest *request){

      // SuhuDS1 = (CalM_DinD1*data[20].toFloat())+CalB_DinD1;
      // SuhuDS2 = (CalM_DinD2*data[21].toFloat())+CalB_DinD2;
      // SuhuDS3 = (CalM_DinD3*data[22].toFloat())+CalB_DinD3;

      String Dsuhu1 = String(SuhuDS1);
      String Dsuhu2 = String(SuhuDS2);
      String Dsuhu3 = String(SuhuDS3);

      String json = "{";
      json += "\"Dsuhu1\":" + safeValue(Dsuhu1)+ ",";
      json += "\"Dsuhu2\":" + safeValue(Dsuhu2)+ ",";
      json += "\"Dsuhu3\":" + safeValue(Dsuhu3)+ ",";

      json += "\"BME1\":" + safeValue(String(BME1))+ ",";
      json += "\"BME2\":" + safeValue(String(BME2))+ ",";
      json += "\"BME3\":" + safeValue(String(BME3));

      json += "}";
      request->send(200, "application/json", json);
      Serial.println(json);

    });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/controling", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", controling_html);
  });

  // server.on("/controling", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send(LittleFS, "/controling.html", "text/html"); });

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {

    if (request->hasParam("In_Watt_Max1")) {
      Watt1 = request->getParam("In_Watt_Max1")->value().toInt();
      Serial.print("Watt1 = ");
      Serial.println(Watt1);
      servo1.write(Watt1);
    }

    if (request->hasParam("In_Watt_Max2")) {
      Watt2 = request->getParam("In_Watt_Max2")->value().toInt();
      Serial.print("Watt2 = ");
      Serial.println(Watt2);
      servo2.write(Watt2);
    }

    if (request->hasParam("In_Watt_Max3")) {
      Watt3 = request->getParam("In_Watt_Max3")->value().toInt();
      Serial.print("Watt3 = ");
      Serial.println(Watt3);
      servo3.write(Watt3);
    }

    if (request->hasParam("In_Watt_Max4")) {
      Watt4 = request->getParam("In_Watt_Max4")->value().toInt();
      Serial.print("Watt4 = ");
      Serial.println(Watt4);
      servo4.write(Watt4);
    }

    if (request->hasParam("In_Rref")) {
      Safety = request->getParam("In_Rref")->value().toInt();
      Serial.print("Safety :");
      Serial.println(Safety);
      if(Safety == 1){
        servo1.write(30);
        servo2.write(30);
        servo3.write(30);
        servo4.write(30);
      }
      else{
        servo1.write(0);
        servo2.write(0);
        servo3.write(0);
        servo4.write(0);
      }
    }


    if (request->hasParam("In_Heat_st1")) {
      Heater_st_1 = request->getParam("In_Heat_st1")->value().toInt();
      Serial.print("Heater_st_1 :");
      Serial.println(Heater_st_1);
      
      if(Heater_st_1 == 1){
        String Perintah = "KIRIM HEATER 1 Heat_st = 1";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
      else{
        String Perintah = "KIRIM HEATER 1 Heat_st = 0";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
    }

    if (request->hasParam("In_Fan_Power1")) {
      Fan_Power_1 = request->getParam("In_Fan_Power1")->value().toInt();
      Serial.print("Fan_Power_1 :");
      Serial.println(Fan_Power_1);
      if(Fan_Power_1 == 1){
        String Perintah = "KIRIM HEATER 1 Fan_Power = 1";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
      else{
        String Perintah = "KIRIM HEATER 1 Fan_Power = 0";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
    }

    if (request->hasParam("In_Power_Tunnel1")) {
      Power_Tunnel_1 = request->getParam("In_Power_Tunnel1")->value().toInt();
      Serial.print("Power_Tunnel_1 :");
      Serial.println(Power_Tunnel_1);
      char order[100];
      sprintf(order, "KIRIM HEATER 1 Power_Tunnel = %d", Power_Tunnel_1);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_Suhu_Tunnel1")) {
      Suhu_Tunnel_1 = request->getParam("In_Suhu_Tunnel1")->value().toInt();
      Serial.print("Suhu_Tunnel_1 :");
      Serial.println(Suhu_Tunnel_1);
      preferences.putInt("Suhu_Tunnel", Suhu_Tunnel_1);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (request->hasParam("In_Heat_st2")) {
      Heater_st_2 = request->getParam("In_Heat_st2")->value().toInt();
      Serial.print("Heater_st_2 :");
      Serial.println(Heater_st_2);
      
      if(Heater_st_2 == 1){
        String Perintah = "KIRIM HEATER 2 Heat_st = 1";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
      else{
        String Perintah = "KIRIM HEATER 2 Heat_st = 0";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
    }

    if (request->hasParam("In_Fan_Power2")) {
      Fan_Power_2 = request->getParam("In_Fan_Power2")->value().toInt();
      Serial.print("Fan_Power_2 :");
      Serial.println(Fan_Power_2);
      if(Fan_Power_2 == 1){
        String Perintah = "KIRIM HEATER 2 Fan_Power = 1";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
      else{
        String Perintah = "KIRIM HEATER 2 Fan_Power = 0";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
    }

    if (request->hasParam("In_Power_Tunnel2")) {
      Power_Tunnel_2 = request->getParam("In_Power_Tunnel2")->value().toInt();
      Serial.print("Power_Tunnel_2 :");
      Serial.println(Power_Tunnel_2);
      char order[100];
      sprintf(order, "KIRIM HEATER 2 Power_Tunnel = %d", Power_Tunnel_2);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_Suhu_Tunnel2")) {
      Suhu_Tunnel_2 = request->getParam("In_Suhu_Tunnel2")->value().toInt();
      Serial.print("Suhu_Tunnel_2 :");
      Serial.println(Suhu_Tunnel_2);
      preferences.putInt("Suhu_Tunnel_2", Suhu_Tunnel_2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (request->hasParam("In_Heat_st3")) {
      Heater_st_3 = request->getParam("In_Heat_st3")->value().toInt();
      Serial.print("Heater_st_3 :");
      Serial.println(Heater_st_3);
      
      if(Heater_st_3 == 1){
        String Perintah = "KIRIM HEATER 3 Heat_st = 1";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
      else{
        String Perintah = "KIRIM HEATER 3 Heat_st = 0";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
    }

    if (request->hasParam("In_Fan_Power3")) {
      Fan_Power_3 = request->getParam("In_Fan_Power3")->value().toInt();
      Serial.print("Fan_Power_3 :");
      Serial.println(Fan_Power_3);
      if(Fan_Power_3 == 1){
        String Perintah = "KIRIM HEATER 3 Fan_Power = 1";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
      else{
        String Perintah = "KIRIM HEATER 3 Fan_Power = 0";
        Serial2.println("<" + Perintah + ">");  // format dengan pembuka & penutup
      }
    }

    if (request->hasParam("In_Power_Tunnel3")) {
      Power_Tunnel_3 = request->getParam("In_Power_Tunnel3")->value().toInt();
      Serial.print("Power_Tunnel_3 :");
      Serial.println(Power_Tunnel_3);
      char order[100];
      sprintf(order, "KIRIM HEATER 3 Power_Tunnel = %d", Power_Tunnel_3);
      Serial2.println("<" + String(order) + ">");  // format dengan pembuka & penutup
      // h = Power_Tunnel;
      // preferences.putInt("Power_Tunnel", Power_Tunnel);
    }

    if (request->hasParam("In_Suhu_Tunnel3")) {
      Suhu_Tunnel_3 = request->getParam("In_Suhu_Tunnel3")->value().toInt();
      Serial.print("Suhu_Tunnel_3 :");
      Serial.println(Suhu_Tunnel_3);
      preferences.putInt("Suhu_Tunnel_3", Suhu_Tunnel_3);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (request->hasParam("In_CalM_DinD1")) {
      CalM_DinD1 = request->getParam("In_CalM_DinD1")->value().toFloat();
      Serial.print("CalM_DinD1 = ");
      Serial.println(CalM_DinD1);
      preferences.putFloat("CalM_DinD1", CalM_DinD1);
    }
    if (request->hasParam("In_CalB_DinD1")) {
      CalB_DinD1 = request->getParam("In_CalB_DinD1")->value().toFloat();
      Serial.print("CalB_DinD1 = ");
      Serial.println(CalB_DinD1);
      preferences.putFloat("CalB_DinD1", CalB_DinD1);
    }

    if (request->hasParam("In_CalM_DinD2")) {
      CalM_DinD2 = request->getParam("In_CalM_DinD2")->value().toFloat();
      Serial.print("CalM_DinD2 = ");
      Serial.println(CalM_DinD2);
      preferences.putFloat("CalM_DinD2", CalM_DinD2);
    }
    if (request->hasParam("In_CalB_DinD2")) {
      CalB_DinD2 = request->getParam("In_CalB_DinD2")->value().toFloat();
      Serial.print("CalB_DinD2 = ");
      Serial.println(CalB_DinD2);
      preferences.putFloat("CalB_DinD2", CalB_DinD2);
    }

    if (request->hasParam("In_CalM_DinD3")) {
      CalM_DinD3 = request->getParam("In_CalM_DinD3")->value().toFloat();
      Serial.print("CalM_DinD3 = ");
      Serial.println(CalM_DinD3);
      preferences.putFloat("CalM_DinD3", CalM_DinD3);
    }
    if (request->hasParam("In_CalB_DinD3")) {
      CalB_DinD3 = request->getParam("In_CalB_DinD3")->value().toFloat();
      Serial.print("CalB_DinD3 = ");
      Serial.println(CalB_DinD3);
      preferences.putFloat("CalB_DinD3", CalB_DinD3);
    }

    ////////////////////////////////////////////////////////////////////
    
    if (request->hasParam("In_CalM_Wcs1")) {
      calM_Wcs1 = request->getParam("In_CalM_Wcs1")->value().toFloat();
      Serial.print("calM_Wcs1 = ");
      Serial.println(calM_Wcs1);
      preferences.putFloat("calM_Wcs1", calM_Wcs1);
    }
    if (request->hasParam("In_CalB_Wcs1")) {
      calB_Wcs1 = request->getParam("In_CalB_Wcs1")->value().toFloat();
      Serial.print("calB_Wcs1 = ");
      Serial.println(calB_Wcs1);
      preferences.putFloat("calB_Wcs1", calB_Wcs1);
    }

    if (request->hasParam("In_CalM_Wcs2")) {
      calM_Wcs2 = request->getParam("In_CalM_Wcs2")->value().toFloat();
      Serial.print("calM_Wcs2 = ");
      Serial.println(calM_Wcs2);
      preferences.putFloat("calM_Wcs2", calM_Wcs2);
    }
    if (request->hasParam("In_CalB_Wcs2")) {
      calB_Wcs2 = request->getParam("In_CalB_Wcs2")->value().toFloat();
      Serial.print("calB_Wcs2 = ");
      Serial.println(calB_Wcs2);
      preferences.putFloat("calB_Wcs2", calB_Wcs2);
    }

    if (request->hasParam("In_CalM_Wcs3")) {
      calM_Wcs3 = request->getParam("In_CalM_Wcs3")->value().toFloat();
      Serial.print("calM_Wcs3 = ");
      Serial.println(calM_Wcs3);
      preferences.putFloat("calM_Wcs3", calM_Wcs3);
    }
    if (request->hasParam("In_CalB_Wcs3")) {
      calB_Wcs3 = request->getParam("In_CalB_Wcs3")->value().toFloat();
      Serial.print("calB_Wcs3 = ");
      Serial.println(calB_Wcs3);
      preferences.putFloat("calB_Wcs3", calB_Wcs3);
    }

    if (request->hasParam("In_CalM_Wcs4")) {
      calM_Wcs4 = request->getParam("In_CalM_Wcs4")->value().toFloat();
      Serial.print("calM_Wcs4 = ");
      Serial.println(calM_Wcs4);
      preferences.putFloat("calM_Wcs4", calM_Wcs4);
    }
    if (request->hasParam("In_CalB_Wcs4")) {
      calB_Wcs4 = request->getParam("In_CalB_Wcs4")->value().toFloat();
      Serial.print("calB_Wcs4 = ");
      Serial.println(calB_Wcs4);
      preferences.putFloat("calB_Wcs4", calB_Wcs4);
    }
    
    request->send(204);
  });

  server.on("/dataSet", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", vSendData().c_str());
  });

  server.on("/dataSetHeat", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", vSendDataHeat().c_str());
  });

  server.on("/updateServer", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("SEDANG PROSES OTA");
    Serial.println("updateFirmware");
    // Update_OTA = 1;
    
    // updateFirmware();
    request->send(200);
  });

  //  server.on("/controling", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send(LittleFS, "/controling.html", "text/html"); });


  server.begin();
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
    } else {
      serialBuffer += c;
    }
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  vSetupLittlefs();
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
    ADSMillis = millis();
  }
  vPembacaanSerial();
}
