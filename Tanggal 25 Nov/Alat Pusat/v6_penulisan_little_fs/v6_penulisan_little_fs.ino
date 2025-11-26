#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <QuickEspNow.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "MainWebServer.h"
#include "ControlingWebserver.h"

#include <NTPClient.h>

AsyncWebServer server(80);

int Safety = 0 ;
int reset_arr = 0;

int Watt1 = 0 ;
int Watt2 = 0 ;
int Watt3 = 0 ;
int Watt4 = 0 ;

int kecepatan1 = 0;
int kecepatan2 = 0;
int kecepatan3 = 0;
int kecepatan4 = 0;

float calM_Wcs1 = 0 ;
float calB_Wcs1 = 0 ;

float calM_Wcs2 = 0 ;
float calB_Wcs2 = 0 ;

float calM_Wcs3 = 0 ;
float calB_Wcs3 = 0 ;

float calM_Wcs4 = 0 ;
float calB_Wcs4 = 0 ;

String SData;
String SDataHeat;
String DATAJS;


static const String msg = "send cmd";
const unsigned int SEND_MSG_MSEC = 50;

unsigned long OTAMillis;
unsigned long IntervalOTAMillis = 200;

unsigned long UpdateTime;
unsigned long Last_UpdateTime;
static int mill = 0;


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

unsigned long epochTime;

// static uint8_t dust13[] = {0xFC, 0xF5, 0xC4, 0xA6, 0xEA, 0x75};
// static uint8_t dust14[] = {0xE8, 0x68, 0xE7, 0xC7, 0xFB, 0x56};
// static uint8_t wind15[] = {0x10, 0x06, 0x1C, 0x68, 0x33, 0xA4};
// static uint8_t wind16[] = {0x3C, 0x8A, 0x1F, 0xA3, 0xEF, 0x8C};
// static uint8_t wind17[] = {0x10, 0x06, 0x1C, 0x68, 0x29, 0xC8};
// static uint8_t wind18[] = {0x10, 0x06, 0x1C, 0x68, 0x1B, 0xDC}; 
// static uint8_t valve20[] = {0x1C, 0x69, 0x20, 0x96, 0xD6, 0x60};

const uint8_t RECEIVER_COUNT = 12;

const long utcOffsetInSeconds = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


unsigned long GetTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

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


String data[23];  // Stores data1 to data20

bool sent = true;


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

  if (quickEspNow.readyToSendData() && sent && ((millis() - lastSend) > SEND_MSG_MSEC) && i < RECEIVER_COUNT) {
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
    for (int j = 0; j < 23; j++) {
      output += data[j];
      if (j < 22) output += ",";
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
    if(reset_arr == 3){

      for (int j = 0; j < 23; j++) {
        data[j] = "0";  // Clear for next cycle
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
  if (LittleFS.exists("/DataAll.js")) {
    LittleFS.remove("/DataAll.js");
    Serial.println("DataAll.js dihapus saat boot.");
  }
  if (LittleFS.exists("/data.csv")) {
    LittleFS.remove("/data.csv");
    Serial.println("data.csv dihapus saat boot.");
  }
}

void vSetupWifi(){
  // WiFi.mode(WIFI_AP_STA);
  // WiFi.softAP("Test Fan web", "12345678");
  // Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
  // Serial.println(WiFi.softAPIP());

  WiFi.begin("Pemecah", "12345678");
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());


}

void vSetupEspNow(){
  quickEspNow.begin(10, 0, false);
  quickEspNow.onDataSent(dataSent);
  quickEspNow.onDataRcvd(dataReceived);
}

String safeValue(String val) {
  val.trim();
  if (val.length() == 0 || val == "nan" || val == "-1.00") return "0";
  return val;
}

String vSendData() {
  char data[150];  // Sesuaikan ukuran buffer jika perlu
  sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f", Safety, Watt1, Watt2, Watt3, Watt4, kecepatan1, kecepatan2, kecepatan3, kecepatan4, calM_Wcs1, calM_Wcs2, calM_Wcs3, calM_Wcs4, calB_Wcs1, calB_Wcs2, calB_Wcs3, calB_Wcs4  );
  SData = String(data);
  Serial.print("WebSet = ");
  Serial.println(SData);

  return SData;
}

String vSendDataHeat() {
  char data[150];  // Sesuaikan ukuran buffer jika perlu
  sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f", Safety, Watt1, Watt2, Watt3, Watt4, kecepatan1, kecepatan2, kecepatan3, kecepatan4, calM_Wcs1, calM_Wcs2, calM_Wcs3, calM_Wcs4, calB_Wcs1, calB_Wcs2, calB_Wcs3, calB_Wcs4  );
  SDataHeat = String(data);
  Serial.print("WebSet = ");
  Serial.println(SDataHeat);

  return SDataHeat;
}

void vAsyncWebServer() {
  // server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  // server.serveStatic("/highcharts.js", LittleFS, "/highcharts.js");
  // server.serveStatic("/exporting.js", LittleFS, "/exporting.js");
  // server.serveStatic("/highcharts-more.js", LittleFS, "/highcharts-more.js");
  // server.serveStatic("/data.js", LittleFS, "/data.js");
  // server.serveStatic("/export-data.js", LittleFS, "/export-data.js");
  // server.serveStatic("/highstock.js", LittleFS, "/highstock.js");
  server.serveStatic("/DataAll.js", LittleFS, "/DataAll.js");
  server.serveStatic("/data.csv", LittleFS, "/data.csv");

    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
      String suhu1 = data[0];
      String kec1 = data[1];
      String suhu2 = data[2];
      String kec2 = data[3];
      String suhu3 = data[4];
      String kec3 = data[5];
      String suhu4 = data[6];
      String kec4 = data[7];
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

      for (int j = 0; j < 23; j++) {
        data[j] = "0";  // Clear for next cycle
      }
      reset_arr = 0;
    });

    server.on("/dataDin", HTTP_GET, [](AsyncWebServerRequest *request){
      String Dsuhu1 = data[20];
      String Dsuhu2 = data[21];
      String Dsuhu3 = data[22];

      String json = "{";
      json += "\"Dsuhu1\":" + safeValue(Dsuhu1)+ ",";
      json += "\"Dsuhu2\":" + safeValue(Dsuhu2)+ ",";
      json += "\"Dsuhu3\":" + safeValue(Dsuhu3);

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
      Serial.print("WebSet = ");
      Serial.println(Watt1);
    }

    if (request->hasParam("In_Watt_Max2")) {
      Watt2 = request->getParam("In_Watt_Max2")->value().toInt();
      Serial.print("WebSet = ");
      Serial.println(Watt1);
    }

    if (request->hasParam("In_Watt_Max3")) {
      Watt3 = request->getParam("In_Watt_Max3")->value().toInt();
      Serial.print("WebSet = ");
      Serial.println(Watt1);
    }

    if (request->hasParam("In_Watt_Max4")) {
      Watt4 = request->getParam("In_Watt_Max4")->value().toInt();
      Serial.print("WebSet = ");
      Serial.println(Watt1);
    }

    if (request->hasParam("In_Rref")) {
      Safety = request->getParam("In_Rref")->value().toInt();
      Serial.print("Safety :");
      Serial.println(Safety);
    }
    
    if (request->hasParam("In_CalM_Wcs1")) {
      calM_Wcs1 = request->getParam("In_CalM_Wcs1")->value().toFloat();
      Serial.print("WebSet = ");
      Serial.println(Watt1);
    }
    if (request->hasParam("In_CalB_Wcs1")) {
      calB_Wcs1 = request->getParam("In_CalB_Wcs1")->value().toFloat();
      Serial.print("WebSet = ");
      Serial.println(Watt1);
    }

    if (request->hasParam("In_CalM_Wcs2")) {
      calM_Wcs2 = request->getParam("In_CalM_Wcs2")->value().toFloat();
      Serial.print("WebSet = ");
      Serial.println(Watt1);
    }
    if (request->hasParam("In_CalB_Wcs2")) {
      calB_Wcs2 = request->getParam("In_CalB_Wcs2")->value().toFloat();
      Serial.print("WebSet = ");
      Serial.println(Watt1);
    }

    if (request->hasParam("In_CalM_Wcs3")) {
      calM_Wcs3 = request->getParam("In_CalM_Wcs3")->value().toFloat();
      Serial.print("WebSet = ");
      Serial.println(Watt1);
    }
    if (request->hasParam("In_CalB_Wcs3")) {
      calB_Wcs3 = request->getParam("In_CalB_Wcs3")->value().toFloat();
      Serial.print("WebSet = ");
      Serial.println(Watt1);
    }

    if (request->hasParam("In_CalM_Wcs4")) {
      calM_Wcs4 = request->getParam("In_CalM_Wcs4")->value().toFloat();
      Serial.print("WebSet = ");
      Serial.println(Watt1);
    }
    if (request->hasParam("In_CalB_Wcs4")) {
      calB_Wcs4 = request->getParam("In_CalB_Wcs4")->value().toFloat();
      Serial.print("WebSet = ");
      Serial.println(Watt1);
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

void vTulisDataJS() {
  char buffer[100];
  sprintf(buffer, "[%lu%d00,%s,%s,%s]", UpdateTime, mill, data[14], data[15], data[16]);
  DATAJS = buffer;
}

void vPengechekanMill() {
  if (UpdateTime == Last_UpdateTime) {
    mill += 2;
    if (mill == 10) {
      mill = 0;
    }
  } else {
    mill = 0;
    Last_UpdateTime = UpdateTime;
  }
}

void vTulisDataJSAll() {
  char buffer[300]; // pastikan cukup besar
  int index = 0;

  // Awali dengan waktu
  // index += sprintf(buffer + index, "[%lu%d00", UpdateTime, mill);
  index += sprintf(buffer + index, "[%lu", UpdateTime);

  // Tambahkan semua data[0] sampai data[19]
  for (int i = 0; i < 23; i++) {
    index += sprintf(buffer + index, ",%s", data[i]);
  }

  // Tutup array baris
  index += sprintf(buffer + index, "]");

  // Simpan ke DATAJS
  DATAJS = buffer;
  Serial.print("buffer = ");
  Serial.println(buffer);
}

void vGantiKomaLoopAll() {
  const char* path = "/DataAll.js";
  File file = LittleFS.open(path, "r+");

  // Jika belum ada, buat dan tulis header awal
  if (!file) {
    Serial.println("File belum ada. Membuat baru...");
    file = LittleFS.open(path, "w");
    if (!file) {
      Serial.println("Gagal membuat file!");
      return;
    }
    file.print("var DataAll = [\n");
    file.print(DATAJS);
    file.print("\n];");
    file.close();
    return;
  }

  // File ada → edit di posisi sebelum "];"
  size_t size = file.size();
  if (size < 4) {
    file.seek(size); // jika terlalu kecil, tidak ada "];"
  } else {
    file.seek(size - 3); // tepat sebelum ];\n
    file.print(",\n");   // tambahkan koma untuk pemisah antar data
  }

  file.print(DATAJS);   // tulis data baru
  file.print("\n];\n"); // tutup ulang array
  file.close();
}

void vDataAll(){
  vPengechekanMill();
  // vTulisDataJS();
  vTulisDataJSAll();
  vGantiKomaLoopAll();
}

void vResetData(){
  for (int j = 0; j < 23; j++) {
    data[j] = String(random(0, 10));  // Clear for next cycle
  }
}


void logToJS() {
  static bool headerWritten = false;
  static unsigned long logCount = 0;
  const int MAX_LINES = 45000;
  const int CHECK_INTERVAL = 5000;
  const char* filename = "/DataAll.js";

  unsigned long waktu = millis(); // gunakan millis sebagai timestamp

  // Siapkan buffer dan isi awal dengan waktu
  char buffer[300];
  int index = 0;
  index += sprintf(buffer + index, "[%lu", waktu);

  // Tambahkan data[1] hingga data[23]
  for (int i = 0; i <= 22; i++) {
    index += sprintf(buffer + index, ",%s", data[i]); // asumsikan data[i] adalah String atau char*
  }

  index += sprintf(buffer + index, "]"); // tutup array
  String DATAJS = buffer;

  if (!headerWritten) {
      if (!LittleFS.exists(filename)) {
        File file = LittleFS.open(filename, "w");
        if (file) {
          file.println("var DataAll = [");
          file.print(DATAJS); // baris pertama tanpa newline
          file.println("\n;");
          file.close();
        }
        headerWritten = true;
        return;
      } else {
        headerWritten = true;
      }
  }

  // Tambahkan data baru ke file yang sudah ada
  File file = LittleFS.open(filename, "r+");
  if (!file) {
    Serial.println("Gagal membuka DataAll.js");
    return;
  }

  size_t size = file.size();
  // seek ke akhir sebelum ];
  if (size >= 3) {
    file.seek(size - 3);
    file.print(",\n");  // tambahkan koma SEBELUM baris baru
  } else {
    file.seek(size);
  }

  file.print(DATAJS);
  file.print("\n];\n");
  file.close();

  // Pangkas file jika terlalu banyak baris
  logCount++;
  if (logCount % CHECK_INTERVAL == 0) {
    File file = LittleFS.open(filename, FILE_READ);
    std::vector<String> lines;
    if (file) {
      while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.startsWith("[")) {
          lines.push_back(line);
        }
      }
      file.close();
    }

    if (lines.size() > MAX_LINES + 10) {
      File newFile = LittleFS.open(filename, FILE_WRITE);
      if (newFile) {
        newFile.println("var DataAll = [");
        int start = lines.size() - MAX_LINES;
        for (int i = start; i < lines.size(); ++i) {
          newFile.print(lines[i]);
          if (i < lines.size() - 1) {
            newFile.print(",\n");  // hanya tambah koma jika bukan baris terakhir
          }
        }
        newFile.println("\n];");
        newFile.println("\n];");
        newFile.close();
      }
    }
  }
}

void logToCSV() {
  static bool headerWritten = false;
  static unsigned long logCount = 0;
  const int MAX_LINES = 45000;
  const int CHECK_INTERVAL = 5000;
  const char* filename = "/data.csv";

  // Tulis header jika belum ditulis
  if (!headerWritten) {
    if (!LittleFS.exists(filename)) {
      File file = LittleFS.open(filename, FILE_WRITE);
      if (file) {
        file.print("timestamp");
        for (int i = 0; i <= 23; i++) {
          file.print(",data");
          file.print(i);
        }
        file.println();
        file.close();
      }
    }
    headerWritten = true;
  }

  // Siapkan waktu timestamp
  epochTime = GetTime();  // atau gunakan millis() jika tak perlu epoch
  UpdateTime = epochTime;
  if (UpdateTime == Last_UpdateTime) {
    mill += 2;
    if (mill == 10) mill = 0;
  } else {
    mill = 0;
    Last_UpdateTime = UpdateTime;
  }

  char timeBuffer[30];
  sprintf(timeBuffer, "%lu%d00", UpdateTime, mill); // atau pakai millis()

  // Buat baris CSV
  String line = String(timeBuffer);
  for (int i = 0; i <= 23; i++) {
    line += "," + String(data[i]);  // asumsikan data[i] bertipe String
  }

  // Tulis ke file
  File file = LittleFS.open(filename, FILE_APPEND);
  if (file) {
    file.println(line);
    file.close();
  } else {
    Serial.println("Gagal membuka file untuk append.");
  }

  // Cek dan potong jika lebih dari MAX_LINES
  logCount++;
  if (logCount % CHECK_INTERVAL == 0) {
    File file = LittleFS.open(filename, FILE_READ);
    int lineCount = 0;
    if (file) {
      while (file.available()) {
        file.readStringUntil('\n');
        lineCount++;
      }
      file.close();
    }

    if (lineCount > MAX_LINES + 10) {
      File oldFile = LittleFS.open(filename, FILE_READ);
      std::vector<String> lastLines;
      while (oldFile.available()) {
        String line = oldFile.readStringUntil('\n');
        line.trim();
        if (line.length() > 0) lastLines.push_back(line);
      }
      oldFile.close();

      File newFile = LittleFS.open(filename, FILE_WRITE);
      if (newFile) {
        newFile.print("timestamp");
        for (int i = 0; i <= 23; i++) {
          newFile.print(",data");
          newFile.print(i);
        }
        newFile.println();

        int start = lastLines.size() > MAX_LINES ? lastLines.size() - MAX_LINES : 1;
        for (int i = start; i < lastLines.size(); ++i) {
          newFile.println(lastLines[i]);
        }
        newFile.close();
      }
    }
  }
}




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  vSetupLittlefs();
  vSetupWifi();
  vAsyncWebServer();
  // vSetupEspNow();
  vResetData();

  // timeClient.begin();
  // epochTime = GetTime();
  // Serial.print("epochTime=");
  // Serial.println(epochTime);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  // vLoopESPnow();
  if (millis() - OTAMillis >= IntervalOTAMillis) {
    UpdateTime = millis();
    vResetData();
    // logToJS();
    logToCSV();
    // vDataAll();
    OTAMillis = millis();
  }
}
