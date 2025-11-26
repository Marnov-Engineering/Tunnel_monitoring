#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_MAX31865.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "max6675.h"
#include <ESP32Servo.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "MainWebServer.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <QuickEspNow.h>

//////// esp now /////////
static const String msg = "send cmd";
const unsigned int SEND_MSG_MSEC = 50;
unsigned long countToReport;
unsigned long cycleStartTime;

int Suhu_Tunnel;
int Fan_Power;
int Heater_st;

String SDataHeat;

float DindSuhu1;
float DindSuhu2;
float DindSuhu3;

static uint8_t sensorDinding1[] = {0x2C, 0x3A, 0xE8, 0x14, 0x85, 0x8F};
static uint8_t sensorDinding2[] = {0x58, 0xBF, 0x25, 0xC2, 0xFE, 0x9F};
static uint8_t sensorDinding3[] = {0xEC, 0xFA, 0xBC, 0x41, 0x6E, 0xDF};

const uint8_t RECEIVER_COUNT = 3;

AsyncWebServer server(80);

static uint8_t* receivers[RECEIVER_COUNT] = {
  sensorDinding1,sensorDinding2,sensorDinding3
};

String data[3];

bool sent = true;

// Jumlah data yang dikirim setiap node
const uint8_t dataPerNode[RECEIVER_COUNT] = {
  1, 1, 1
};

int thermoDO = 4;
int thermoCS = 2;
int thermoCLK = 15;

static const int servoPin = 26;
static const int SSRPin = 25;
Servo servo1;

String inputString = "";  // Untuk menyimpan input dari Serial
bool inputComplete = false;
int h = 0;
int f = 0;
int counting;

// #define ONE_WIRE_BUS 33
#define PIN_WCS 33
int Raw_WCS;
#define SEALEVELPRESSURE_HPA (1013.25)


// OneWire oneWire(ONE_WIRE_BUS);
// DallasTemperature sensorsDS(&oneWire); // sensor ds 
// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(5, 23, 19, 18);// sensorRTD
Adafruit_BME280 bme; // I2C sensor BME
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);// thermocouple


int RREF = 430 ;//430.0 rtd
#define RNOMINAL  100.0 //rtd

unsigned long millis1S = 1000;
unsigned long now1S = 0;

unsigned long millisSSR = 10;
unsigned long nowSSR = 0;


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

void vLoopESPnow() {
  static time_t lastSend = 0;
  static int i = 0;
  static bool countToReport = false;
  static unsigned long cycleStartTime = 0;

  // Awal siklus pengiriman
  if (i == 0 && !countToReport) {
    cycleStartTime = millis();
  }

  // Kirim data ke setiap node jika waktunya sudah lewat
  if (quickEspNow.readyToSendData() && sent && ((millis() - lastSend) > SEND_MSG_MSEC) && i < RECEIVER_COUNT) {
    lastSend = millis();
    String message = String("MSG ") + String(i);  // contoh pesan
    sent = false;

    quickEspNow.send(receivers[i], (uint8_t*)message.c_str(), message.length());
    i++;

    if (i == RECEIVER_COUNT) {
      countToReport = true;  // Semua node sudah dikirim
    }
  }

  // Setelah semua data dikirim dan tunggu 1.5 detik, kirim hasil
  if (countToReport && (millis() - cycleStartTime >= 1000)) {
    String output = "";
    for (int j = 0; j < RECEIVER_COUNT; j++) {
      output += data[j];
      if (j < RECEIVER_COUNT - 1) output += ",";
      Serial.printf("data[%d] = %s\n", j, data[j].c_str());
    }

    Serial2.println("<" + output + ">");  // kirim ke serial
    Serial.println("Output: <" + output + ">");

    // Reset untuk siklus berikutnya
    for (int j = 0; j < RECEIVER_COUNT; j++) {
      data[j] = "0";
    }

    countToReport = false;
    i = 0;
    sent = true;
  }
}

void vSetupWifi(){
  // Serial2.begin(115200);
  // WiFi.mode(WIFI_MODE_STA);
  // WiFi.disconnect(false, true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("Test Fan web", "12345678");

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

String safeValue(String val) {
  val.trim();
  if (val.length() == 0 || val == "nan" || val == "-1.00") return "0";
  return val;
}

/////////// WEBSERVER //////////////

void vSetupLittlefs(){
  // Inisialisasi LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Gagal mount LittleFS!");
    return;
  }
}

// String vSendData() {
//   char data[150];  // Sesuaikan ukuran buffer jika perlu
//   sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%d,%d,%d,%d", Safety, Watt1, Watt2, Watt3, Watt4, kecepatan1, kecepatan2, kecepatan3, kecepatan4, calM_Wcs1, calM_Wcs2, calM_Wcs3, calM_Wcs4, calB_Wcs1, calB_Wcs2, calB_Wcs3, calB_Wcs4, Arus1, Arus2, Arus3, Arus4);
//   SData = String(data);
//   Serial.print("WebSet = ");
//   Serial.println(SData);

//   return SData;
// }

String vSendDataHeat() {
  char data[150];  // Sesuaikan ukuran buffer jika perlu
  
  sprintf(data, "%d,%d,%d", Suhu_Tunnel, Heater_st, Fan_Power);
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

    server.on("/dataDin", HTTP_GET, [](AsyncWebServerRequest *request){

      // SuhuDS1 = (CalM_DinD1*data[20].toFloat())+CalB_DinD1;
      // SuhuDS2 = (CalM_DinD2*data[21].toFloat())+CalB_DinD2;
      // SuhuDS3 = (CalM_DinD3*data[22].toFloat())+CalB_DinD3;

      String Dsuhu1 = String(data[0]);
      String Dsuhu2 = String(data[1]);
      String Dsuhu3 = String(data[2]);
      
      String BME280 = String(bme.readTemperature());

      String json = "{";
      json += "\"Dsuhu1\":" + safeValue(Dsuhu1)+ ",";
      json += "\"Dsuhu2\":" + safeValue(Dsuhu2)+ ",";
      json += "\"Dsuhu3\":" + safeValue(Dsuhu3)+ ",";

      json += "\"BME280\":" + safeValue(BME280);

      json += "}";
      request->send(200, "application/json", json);
      Serial.println(json);

    });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // server.on("/controling", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send_P(200, "text/html", controling_html);
  // });

  // server.on("/controling", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send(LittleFS, "/controling.html", "text/html"); });

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {

    // if (request->hasParam("In_Rref")) {
    //   Safety = request->getParam("In_Rref")->value().toInt();
    //   Serial.print("Safety :");
    //   Serial.println(Safety);
    //   if(Safety == 1){
    //     servo1.write(35);
    //     servo2.write(35);
    //     servo3.write(35);
    //     servo4.write(35);
    //   }
    //   else{
    //     servo1.write(0);
    //     servo2.write(0);
    //     servo3.write(0);
    //     servo4.write(0);
    //   }
    // }

    if (request->hasParam("In_Heat_st")) {
      Heater_st = request->getParam("In_Heat_st")->value().toInt();
      Serial.print("Heater_st :");
      Serial.println(Heater_st);
      if(Heater_st == 1){
        servo1.write(35);
      }
      else{
        servo1.write(0);
      }
    }

    if (request->hasParam("In_Fan_Power")) {
      Fan_Power = request->getParam("In_Fan_Power")->value().toInt();
      Serial.print("Fan_Power :");
      Serial.println(Fan_Power);
      if(Fan_Power == 1){
        servo1.write(100);
      }
      else{
        servo1.write(0);
      }
    }

    if (request->hasParam("In_Suhu_Tunnel")) {
      Suhu_Tunnel = request->getParam("In_Suhu_Tunnel")->value().toInt();
      Serial.print("Suhu_Tunnel :");
      Serial.println(Suhu_Tunnel);
      h = Suhu_Tunnel;
      // preferences.putInt("Suhu_Tunnel", Suhu_Tunnel);
    }

    //     if (request->hasParam("In_CalM_DinD1")) {
    //   CalM_DinD1 = request->getParam("In_CalM_DinD1")->value().toFloat();
    //   Serial.print("CalM_DinD1 = ");
    //   Serial.println(CalM_DinD1);
    //   preferences.putFloat("CalM_DinD1", CalM_DinD1);
    // }
    // if (request->hasParam("In_CalB_DinD1")) {
    //   CalB_DinD1 = request->getParam("In_CalB_DinD1")->value().toFloat();
    //   Serial.print("CalB_DinD1 = ");
    //   Serial.println(CalB_DinD1);
    //   preferences.putFloat("CalB_DinD1", CalB_DinD1);
    // }

    // if (request->hasParam("In_CalM_DinD2")) {
    //   CalM_DinD2 = request->getParam("In_CalM_DinD2")->value().toFloat();
    //   Serial.print("CalM_DinD2 = ");
    //   Serial.println(CalM_DinD2);
    //   preferences.putFloat("CalM_DinD2", CalM_DinD2);
    // }
    // if (request->hasParam("In_CalB_DinD2")) {
    //   CalB_DinD2 = request->getParam("In_CalB_DinD2")->value().toFloat();
    //   Serial.print("CalB_DinD2 = ");
    //   Serial.println(CalB_DinD2);
    //   preferences.putFloat("CalB_DinD2", CalB_DinD2);
    // }

    // if (request->hasParam("In_CalM_DinD3")) {
    //   CalM_DinD3 = request->getParam("In_CalM_DinD3")->value().toFloat();
    //   Serial.print("CalM_DinD3 = ");
    //   Serial.println(CalM_DinD3);
    //   preferences.putFloat("CalM_DinD3", CalM_DinD3);
    // }
    // if (request->hasParam("In_CalB_DinD3")) {
    //   CalB_DinD3 = request->getParam("In_CalB_DinD3")->value().toFloat();
    //   Serial.print("CalB_DinD3 = ");
    //   Serial.println(CalB_DinD3);
    //   preferences.putFloat("CalB_DinD3", CalB_DinD3);
    // }

    // ////////////////////////////////////////////////////////////////////
    
    // if (request->hasParam("In_CalM_Wcs1")) {
    //   calM_Wcs1 = request->getParam("In_CalM_Wcs1")->value().toFloat();
    //   Serial.print("calM_Wcs1 = ");
    //   Serial.println(calM_Wcs1);
    //   preferences.putFloat("calM_Wcs1", calM_Wcs1);
    // }
    // if (request->hasParam("In_CalB_Wcs1")) {
    //   calB_Wcs1 = request->getParam("In_CalB_Wcs1")->value().toFloat();
    //   Serial.print("calB_Wcs1 = ");
    //   Serial.println(calB_Wcs1);
    //   preferences.putFloat("calB_Wcs1", calB_Wcs1);
    // }

    // if (request->hasParam("In_CalM_Wcs2")) {
    //   calM_Wcs2 = request->getParam("In_CalM_Wcs2")->value().toFloat();
    //   Serial.print("calM_Wcs2 = ");
    //   Serial.println(calM_Wcs2);
    //   preferences.putFloat("calM_Wcs2", calM_Wcs2);
    // }
    // if (request->hasParam("In_CalB_Wcs2")) {
    //   calB_Wcs2 = request->getParam("In_CalB_Wcs2")->value().toFloat();
    //   Serial.print("calB_Wcs2 = ");
    //   Serial.println(calB_Wcs2);
    //   preferences.putFloat("calB_Wcs2", calB_Wcs2);
    // }

    // if (request->hasParam("In_CalM_Wcs3")) {
    //   calM_Wcs3 = request->getParam("In_CalM_Wcs3")->value().toFloat();
    //   Serial.print("calM_Wcs3 = ");
    //   Serial.println(calM_Wcs3);
    //   preferences.putFloat("calM_Wcs3", calM_Wcs3);
    // }
    // if (request->hasParam("In_CalB_Wcs3")) {
    //   calB_Wcs3 = request->getParam("In_CalB_Wcs3")->value().toFloat();
    //   Serial.print("calB_Wcs3 = ");
    //   Serial.println(calB_Wcs3);
    //   preferences.putFloat("calB_Wcs3", calB_Wcs3);
    // }

    // if (request->hasParam("In_CalM_Wcs4")) {
    //   calM_Wcs4 = request->getParam("In_CalM_Wcs4")->value().toFloat();
    //   Serial.print("calM_Wcs4 = ");
    //   Serial.println(calM_Wcs4);
    //   preferences.putFloat("calM_Wcs4", calM_Wcs4);
    // }
    // if (request->hasParam("In_CalB_Wcs4")) {
    //   calB_Wcs4 = request->getParam("In_CalB_Wcs4")->value().toFloat();
    //   Serial.print("calB_Wcs4 = ");
    //   Serial.println(calB_Wcs4);
    //   preferences.putFloat("calB_Wcs4", calB_Wcs4);
    // }
    
    request->send(204);
  });

  // server.on("/dataSet", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send_P(200, "text/plain", vSendData().c_str());
  // });

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ledcSetup(0, 1, 8); // channel, frekuensi, resolution
  ledcAttachPin(SSRPin, 0);
  vSetupLittlefs();
  
  vSetupWifi();
  vSetupEspNow();
  vAsyncWebServer();
  
  // sensorsDS.begin();
  thermo.begin(MAX31865_3WIRE);
  pinMode(SSRPin, OUTPUT);
  pinMode(PIN_WCS, INPUT);
  // digitalWrite(SSRPin, HIGH);
  // delay(5000);
  // digitalWrite(SSRPin, LOW);
  //
  bool status;
  // default settings
  status = bme.begin(0x76);  
  // You can also pass in a Wire library object like &Wire2
  // status = bme.begin(0x76, &Wire2)
  servo1.attach(servoPin);
  servo1.write(0);
  delay(3000);
}

void vSuhuThermocouple(){
  // Serial.println("Suhu ThermoCouple ==============================");
  Serial.print("C = "); 
  Serial.println(thermocouple.readCelsius());
  // Serial.print("F = ");
  // Serial.println(thermocouple.readFahrenheit());
  // Serial.println("===================================================");
}

void vSuhuBME280(){
  // Serial.println("Suhu BME280 ==============================");
  Serial.print("Temperature BME280 = ");
  Serial.print(bme.readTemperature());
  Serial.println(" °C");

  // Serial.print("Pressure = ");

  // Serial.print(bme.readPressure() / 100.0F);
  // Serial.println(" hPa");

  // Serial.print("Approx. Altitude = ");
  // Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  // Serial.println(" m");

  // Serial.print("Humidity = ");
  // Serial.print(bme.readHumidity());
  // Serial.println(" %");

  // Serial.println("===================================================");
  // Serial.println();

}

void vSuhuRTD(){
  // Serial.println("Suhu RTD ==============================");
  uint16_t rtd = thermo.readRTD();

  // Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;
  // Serial.print("Ratio = "); Serial.println(ratio,8);
  // Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
  Serial.print("Temperature RTD = "); Serial.println(thermo.temperature(RNOMINAL, RREF));
  // Serial.println();
  // Serial.println("===================================================");
}

// void vSuhuDS(){
//   // Serial.println("Suhu DS ==============================");
//   // Serial.print("Requesting temperatures...");
//   sensorsDS.requestTemperatures(); // Send the command to get temperatures
//   // Serial.println("DONE");
//   // delay(1500);
//   // After we got the temperatures, we can print them here.
//   // We use the function ByIndex, and as an example get the temperature from the first sensor only.
//   float tempC = sensorsDS.getTempCByIndex(0);

//   // Check if reading was successful
//   if (tempC != DEVICE_DISCONNECTED_C)
//   {
//     Serial.print("Temperature DS = ");
//     Serial.println(tempC);
//   }
//   else
//   {
//     Serial.println("Error: Could not read temperature data");
//   }
//   // Serial.println();
  
// }

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() - now1S > millis1S){
    // vSuhuDS();
    // vSuhuRTD();
    String output = "";
      for (int j = 0; j < 3; j++) {
        output += data[j];
        if (j < 22) output += ",";
        Serial.print("Suhu Dinding ");
        Serial.print(j);
        Serial.print(" = ");
        Serial.println(data[j]);  // Cetak nilai masing-masing
        // data[j] = "";  // Clear for next cycle
      }
    // Serial.println("now>> " + output);
    vSuhuBME280();
    Raw_WCS = analogRead(PIN_WCS);
    Serial.print("Raw WCS = ");
    Serial.println(Raw_WCS);
    Serial.println("===================================================");
    // vSuhuThermocouple();
    // for (int j = 0; j < 3; j++) {
    //       data[j] = "0";  // Clear for next cycle
    //     }
    now1S = millis();
  }
  // while (Serial.available()) {
  //   char inChar = (char)Serial.read();
  //   if (inChar == '\n' || inChar == '\r') {
  //     inputComplete = true;
  //   } else {
  //     inputString += inChar;
  //   }
  // }

  // // Proses input jika lengkap
  // if (inputComplete) {
  //   int posDegrees = inputString.toInt(); // Konversi ke integer
  //   if (posDegrees >= 0 && posDegrees <= 180) {
  //     servo1.write(posDegrees);
  //     Serial.print("Servo diputar ke: ");
  //     Serial.println(posDegrees);
  //   } else {
  //     Serial.println("Masukkan angka antara 0 hingga 180.");
  //   }
  //   inputString = "";
  //   inputComplete = false;
  // }
  // Baca data dari Serial
        if (millis() - nowSSR > millisSSR){
          counting = counting + 1;
          // if(counting > 100){
          //   counting = 0;
          // }
          nowSSR = millis();
          // Serial.println(counting);
        }
        if(counting  < h){
          digitalWrite(SSRPin, HIGH);
          // Serial.print("SSR Nyala ");
        }
        else{
          digitalWrite(SSRPin, LOW);
          // Serial.print("SSR MATI ");
        }

        if(counting == 100){
          counting = 0;
        }



  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      inputComplete = true;
    } else {
      inputString += inChar;
    }
  }

  // Proses input jika lengkap
  if (inputComplete) {
    inputString.trim(); // Hilangkan spasi di awal/akhir

    // Cek apakah formatnya variabel = nilai
    int eqIndex = inputString.indexOf('=');
    if (eqIndex > 0) {
      String varName = inputString.substring(0, eqIndex);
      String varValue = inputString.substring(eqIndex + 1);

      varName.trim();
      varValue.trim();
      int value = varValue.toInt();

      if (varName.equalsIgnoreCase("h")) {
        h = value;
        Serial.print("h diatur ke: ");
        Serial.println(h);

        float persen;
        persen = (h/100)*255;

        // ledcWrite(0, int(h));
        Serial.print("Heater diputar ke : ");
        Serial.print(h);
        Serial.println(" %");
        // int trgSSR;
        // trgSSR = h * 10
        

      } else if (varName.equalsIgnoreCase("f")) {
        f = value;
        Serial.print("f diatur ke: ");
        Serial.println(f);
        if (f >= 0 && f <= 180){
          servo1.write(f);
          Serial.print("Servo diputar ke: ");
          Serial.println(f);
        }
        else  {
          Serial.println("Masukkan angka antara 0 hingga 180.");
        }
        
      } else {
        Serial.println("Variabel tidak dikenal.");
      }
    } 
    else {
      // Jika bukan format variabel=nilai, gunakan parsing servo
      int posDegrees = inputString.toInt();
      if (posDegrees >= 0 && posDegrees <= 180) {
        servo1.write(posDegrees);
        Serial.print("Servo diputar ke: ");
        Serial.println(posDegrees);
      } else {
        Serial.println("Masukkan angka antara 0 hingga 180.");
      }
    }

    // Reset string
    inputString = "";
    inputComplete = false;
  }
}
