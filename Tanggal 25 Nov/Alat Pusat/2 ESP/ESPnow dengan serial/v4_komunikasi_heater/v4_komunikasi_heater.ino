#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <QuickEspNow.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "MainWebServer.h"
#include "ControlingWebserver.h"
#include <Adafruit_ADS1X15.h>

#define RX_PIN 16
#define TX_PIN 17

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
// Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

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


static const String msg = "send cmd";
const unsigned int SEND_MSG_MSEC = 50;

unsigned long countToReport;
unsigned long cycleStartTime;

static uint8_t receiver[] = {0x1C, 0x69, 0x20, 0x96, 0x77, 0x60}; // MAC address tujuan


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

// 2C:F4:32:2F:F4:F9
// 58:BF:25:C2:FE:9F
// EC:FA:BC:41:6E:DF
static uint8_t sensorDinding1[] = {0x2C, 0x3A, 0xE8, 0x14, 0x85, 0x8F};
static uint8_t sensorDinding2[] = {0x58, 0xBF, 0x25, 0xC2, 0xFE, 0x9F};
static uint8_t sensorDinding3[] = {0xEC, 0xFA, 0xBC, 0x41, 0x6E, 0xDF};

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


String data[23];  // Stores data1 to data20
String dataHeater[23];

bool kirimHeaterFlag = false;
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
  // Serial.println("masuk sini>> ");
  //   String output = "";
  //   for (int j = 0; j < 23; j++) {
  //     output += data[j];
  //     if (j < 22) output += ",";
  //     Serial.print("data[");
  //     Serial.print(j);
  //     Serial.print("] = ");
  //     Serial.println(data[j]);  // Cetak nilai masing-masing
  //     // data[j] = "";  // Clear for next cycle
  //   }
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
    Serial2.println("<" + output + ">");  // format dengan pembuka & penutup

    for (int j = 0; j < 23; j++) {
        data[j] = "0";  // Clear for next cycle
      }
      
    // Serial.println("reset_arr>> " + reset_arr);
    // Serial2.print("<" + output + ">");

    countToReport = false;
    i = 0;
    sent = true;
    // if(reset_arr == 3){

    //   for (int j = 0; j < 23; j++) {
    //     data[j] = "0";  // Clear for next cycle
    //   }
    //   reset_arr = 0;
    // }
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
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect(false, true);
  // WiFi.mode(WIFI_AP_STA);
  // WiFi.softAP("Test Fan web", "12345678");

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

      // for (int j = 0; j < 22; j++) {
      //   data[j] = "";  // Clear for next cycle
      // }
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

// void vPembacaanSerial(){
//   static String serialBuffer = "";

//   while (Serial2.available()) {
//     char c = Serial2.read();

//     if (c == '<') {
//       serialBuffer = "";
//     } else if (c == '>') {
//       if (serialBuffer == "KIRIM HEATER") {
//         kirimHeaterFlag = true;
//         Serial.println("Perintah KIRIM HEATER diterima!");
//       } else {
//         parseSerialData(serialBuffer); // false = bukan heater
//       }
//       serialBuffer = "";
//     } else {
//       serialBuffer += c;
//     }
//   }
// }

void vPembacaanSerial() {
  static String serialBuffer = "";

  while (Serial2.available()) {
    char c = Serial2.read();

    if (c == '<') {
      serialBuffer = "";
    } else if (c == '>') {
      String lastSerialData = serialBuffer; // Ambil data antara tanda < >
      Serial.println("Data dari Serial2:");
      Serial.println(lastSerialData);

      // Kirim data melalui ESP-NOW
      if (!quickEspNow.send(receiver, (uint8_t *)lastSerialData.c_str(), lastSerialData.length())) {
        Serial.println(">>>>>>>>>> Message sent");
      } else {
        Serial.println(">>>>>>>>>> Message not sent");
      }

      serialBuffer = ""; // Reset buffer setelah dikirim
    } else {
      serialBuffer += c;
    }
  }
}


void parseSerialData(String input) {
  int index = 0;
  int start = 0;
  for (int i = 0; i < input.length(); i++) {
    if (input.charAt(i) == ',') {
      dataHeater[index++] = input.substring(start, i);
      start = i + 1;
    }
  }
  dataHeater[index] = input.substring(start); // elemen terakhir

  // Cetak semua nilai yang diterima
  for (int i = 0; i <= index; i++) {
    Serial.printf("dataHeater[%d] = %s\n", i, dataHeater[i].c_str());

  }

  // SuhuDS1 = (CalM_DinD1*data[20].toFloat())+CalB_DinD1;
  // SuhuDS2 = (CalM_DinD2*data[21].toFloat())+CalB_DinD2;
  // SuhuDS3 = (CalM_DinD3*data[22].toFloat())+CalB_DinD3;
  
  Serial.println("selesai mendapatkan data");
  // reset_arr bisa diatur di sini juga kalau diperlukan
  reset_arr = 0;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // vSetupLittlefs();
  vSetupWifi();
  vSetupEspNow();
  // Pengirim dan penerima harus punya inisialisasi yang sama
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); 
  
  // vAsyncWebServer();
  
  // vSetupADS();
}

void loop() {
  static unsigned long cycleStartTime = 0;

    if (millis() - cycleStartTime >= 5000){
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
      Serial.println("now>> " + output);
      Serial2.println("<" + output + ">");  // format dengan pembuka & penutup

      for (int j = 0; j < 23; j++) {
          data[j] = "0";  // Clear for next cycle
        }
      Serial.println("RESET");
      cycleStartTime = millis();
    }
    
  vPembacaanSerial();


  // put your main code here, to run repeatedly:
  // vLoopESPnow();
}
