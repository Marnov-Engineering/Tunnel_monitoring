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
int safety = 0;

int currentPos = 0;     // posisi sekarang servo
int targetPos  = 0;     // posisi tujuan
unsigned long lastUpdate = 0;
const int stepDelay = 20;  // jeda antar langkah (ms)
const int stepSize  = 1;   // langkah per update

String SDataHeat;
String perintah;
String order = "Kirimkan Suhu";

bool balikan = 0;
int countGagal = 0;
bool waitFeedback = 0;

float DindSuhu1;
float DindSuhu2;
float DindSuhu3;

static uint8_t macDinding3[] = {0x2C, 0x3A, 0xE8, 0x14, 0x85, 0x8F};
static uint8_t macDinding2[] = {0x58, 0xBF, 0x25, 0xC2, 0xFE, 0x9F};
static uint8_t macDinding1[] = {0xEC, 0xFA, 0xBC, 0x41, 0x6E, 0xDF};

// MAC target masing-masing
static uint8_t target1[] = {0x1C, 0x69, 0x20, 0x96, 0x77, 0x60};
static uint8_t target2[] = {0xEC, 0xE3, 0x34, 0xD7, 0x70, 0x60};
static uint8_t target3[] = {0x1C, 0x69, 0x20, 0x96, 0xEC, 0xC8};

static uint8_t AlatPusat[] = {0x1C, 0x69, 0x20, 0x96, 0x31, 0xD4};

uint8_t *sensorDinding = nullptr;

const uint8_t RECEIVER_COUNT = 2;

AsyncWebServer server(80);

static uint8_t* receivers[RECEIVER_COUNT] = {
  sensorDinding,AlatPusat
};

String data[3];
String BME280;

bool sent = true;

// Jumlah data yang dikirim setiap node
const uint8_t dataPerNode[RECEIVER_COUNT] = {
  1, 1
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
bool Raw_WCS;
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

unsigned long millis100S = 100;
unsigned long prevMillis = 0;

unsigned long millisSSR = 10;
unsigned long nowSSR = 0;

/////////////// ESP NOW
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

bool isMacMatch(const uint8_t *macArray) {
  for (int i = 0; i < 6; i++) {
    if (macArray[i] != WiFi.macAddress()[i]) {
      return false;
    }
  }
  return true;
}

void dataReceived(uint8_t* address, uint8_t* dataRaw, uint8_t len, signed int rssi, bool broadcast) {
  String incoming = "";
  for (int i = 0; i < len; i++) incoming += (char)dataRaw[i];

  // Cek apakah data berasal dari MAC AlatPusat
  if (macMatch(address, AlatPusat)) {
    Serial.print("Dari AlatPusat: ");
    Serial.println(incoming);

    // Cek apakah pesan diawali "KIRIM HEATER"
    if (incoming.startsWith(perintah)) {
      // Cari posisi tanda '='
      int eqIndex = incoming.indexOf('=');
      if (eqIndex > 0) {
        String varName = incoming.substring(0, eqIndex);
        String varValue = incoming.substring(eqIndex + 1);

        varName.trim();
        varValue.trim();

        // Kalau mau ambil Heat_st
        if (varName.indexOf("Heat_st") != -1) {
          Heater_st = varValue.toInt();
          Serial.print("Heat_st diterima: ");
          Serial.println(Heater_st);

          // Misal mau langsung eksekusi servo
          if (Heater_st == 1) {
            servo1.write(35);
            safety = 1;
          } else {
            servo1.write(0);
            safety = 0;
          }
        }

        if (varName.indexOf("Fan_Power") != -1) {
          Fan_Power = varValue.toInt();
          Serial.print("Fan_Power diterima: ");
          Serial.println(Fan_Power);

          if(Fan_Power == 1){
            // servo1.write(100);
            targetPos = 100;
          }
          else if(safety){
            // servo1.write(35);
            targetPos = 35;
          }
          else{
            // servo1.write(0);
            targetPos = 0;
          }
        }

        if (varName.indexOf("Power_Tunnel") != -1) {
          Suhu_Tunnel = varValue.toInt();
          Serial.print("Power_Tunnel diterima: ");
          Serial.println(Suhu_Tunnel);
          h = Suhu_Tunnel;
        }
      }
    }

    // vKirimPusat();

    // String message = String(Heater_st) + "," + String(Fan_Power) + "," + String(Suhu_Tunnel) + "," + String(Raw_WCS) + "," + safeValue(data[0]);
    // // sent = false;
    // if (!quickEspNow.send(AlatPusat, (uint8_t *)message.c_str(), message.length()))
    // {
    //     Serial.printf(">>>>>>>>>> Message sent\n");
    // }
    // else
    // {
    //     Serial.printf(">>>>>>>>>> Message not sent\n");
    //     sent = true; // In case of error we need to set the flag to true to avoid blocking the loop
    // }

    return; // stop di sini biar gak masuk ke parsing node lain
  }

  // // Kalau bukan dari AlatPusat → proses parsing data biasa
  // for (int node = 0, dataIndex = 0; node < RECEIVER_COUNT; node++) {
  //   if (macMatch(address, receivers[node])) {
  //     int count = dataPerNode[node];
  //     int idx = 0;
  //     for (int j = 0; j < count; j++) {
  //       int nextComma = incoming.indexOf(',', idx);
  //       String val;
  //       if (nextComma != -1) {
  //         val = incoming.substring(idx, nextComma);
  //         idx = nextComma + 1;
  //       } else {
  //         val = incoming.substring(idx);
  //         idx = incoming.length();
  //       }
  //       data[dataIndex + j] = val;
  //     }
  //     break;
  //   }
  //   dataIndex += dataPerNode[node];
  // }

  // Kalau bukan dari AlatPusat → proses parsing data biasa
  if (sensorDinding != nullptr && macMatch(address, sensorDinding)) {
    int idx = 0;
    int nextComma = incoming.indexOf(',', idx);
    String val;
    if (nextComma != -1) {
      val = incoming.substring(idx, nextComma);
    } else {
      val = incoming.substring(idx);
    }
    data[0] = val;
    Serial.println("Dapat nilai sensor");
    Serial.println(val);

    balikan = 1;
    
    countGagal = 0;
    if(waitFeedback){
      vKirimPusat();
      waitFeedback = 0;
    }
    
    
  }

}

// esp now
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
  // Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
  // Serial.println(WiFi.softAPIP());

  uint8_t mac[6];
  WiFi.macAddress(mac);
  Serial.printf("MAC ESP ini: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  // Pencocokan MAC
  if (memcmp(mac, target1, 6) == 0) {
    sensorDinding = macDinding1;
    perintah = "KIRIM HEATER 1";
    Serial.println("ESP ini = 0x1C, 0x69, 0x20, 0x96, 0x77, 0x60");
  }
  else if (memcmp(mac, target2, 6) == 0) {
    sensorDinding = macDinding2;
    perintah = "KIRIM HEATER 2";
    Serial.println("ESP ini = 0xEC, 0xE3, 0x34, 0xD7, 0x70, 0x60");
  }
  else if (memcmp(mac, target3, 6) == 0) {
    sensorDinding = macDinding3;
    perintah = "KIRIM HEATER 3";
    Serial.println("ESP ini = 0x1C, 0x69, 0x20, 0x96, 0xEC, 0xC8");
  } else {
    Serial.println("MAC tidak terdaftar → tidak kirim");
  }
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
  
  sprintf(data, "%d,%d,%d,%d", Suhu_Tunnel, Heater_st, Fan_Power, Raw_WCS);
  SDataHeat = String(data);
  Serial.print("SDataHeat = ");
  Serial.println(SDataHeat);

  return SDataHeat;
}

void vAsyncWebServer() {
  // server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });
  
  server.serveStatic("/highcharts.js", LittleFS, "/highcharts.js");
  server.serveStatic("/exporting.js", LittleFS, "/exporting.js");
  server.serveStatic("/highcharts-more.js", LittleFS, "/highcharts-more.js");

    server.on("/dataDin", HTTP_GET, [](AsyncWebServerRequest *request){

      // SuhuDS1 = (CalM_DinD1*data[20].toFloat())+CalB_DinD1;
      // SuhuDS2 = (CalM_DinD2*data[21].toFloat())+CalB_DinD2;
      // SuhuDS3 = (CalM_DinD3*data[22].toFloat())+CalB_DinD3;

      String Dsuhu1 = String(data[0]);
      // String Dsuhu2 = String(data[1]);
      // String Dsuhu3 = String(data[2]);
      
      BME280 = String(bme.readTemperature());

      String json = "{";
      json += "\"Dsuhu1\":" + safeValue(Dsuhu1)+ ",";
      // json += "\"Dsuhu2\":" + safeValue(Dsuhu2)+ ",";
      // json += "\"Dsuhu3\":" + safeValue(Dsuhu3)+ ",";

      json += "\"BME280\":" + safeValue(BME280);

      json += "}";
      request->send(200, "application/json", json);
      Serial.println(json);

    });

  

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {

    if (request->hasParam("In_Heat_st")) {
      Heater_st = request->getParam("In_Heat_st")->value().toInt();
      Serial.print("Heater_st :");
      Serial.println(Heater_st);
      if(Heater_st == 1){
        // servo1.write(35);
        targetPos = 35;

        safety = 1;
      }
      else{
        // servo1.write(0);
        targetPos = 0;

        safety = 0;
      }
    }

    if (request->hasParam("In_Fan_Power")) {
      Fan_Power = request->getParam("In_Fan_Power")->value().toInt();
      Serial.print("Fan_Power :");
      Serial.println(Fan_Power);
      if(Fan_Power == 1){
        // servo1.write(100);
        targetPos = 100;

      }
      else if(safety){
        // servo1.write(35);
        targetPos = 35;

      }
      else{
        // servo1.write(0);
        targetPos = 0;

      }
    }

    if (request->hasParam("In_Suhu_Tunnel")) {
      Suhu_Tunnel = request->getParam("In_Suhu_Tunnel")->value().toInt();
      Serial.print("Suhu_Tunnel :");
      Serial.println(Suhu_Tunnel);
      h = Suhu_Tunnel;
    }
    
    request->send(204);
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

  server.begin();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // ledcSetup(0, 1, 8); // channel, frekuensi, resolution
  // ledcAttachPin(SSRPin, 0);
  vSetupLittlefs();
  
  vSetupWifi();
  vSetupEspNow();
  vAsyncWebServer();
  
  // sensorsDS.begin();
  // thermo.begin(MAX31865_3WIRE);
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

void vKirimPusat(){
    BME280 = String(bme.readTemperature());
    String message = String(Heater_st) + "," + String(Fan_Power) + "," + String(Suhu_Tunnel) + "," + String(Raw_WCS) + "," + safeValue(data[0]) + "," + BME280;
    // sent = false;
    if (!quickEspNow.send(AlatPusat, (uint8_t *)message.c_str(), message.length()))
    {
        Serial.printf(">>>>>>>>>> Message sent\n");
        Serial.println(message);
        data[0] = "";  // Clear for next cycle
    }
    else
    {
        Serial.printf(">>>>>>>>>> Message not sent\n");
        sent = true; // In case of error we need to set the flag to true to avoid blocking the loop
    }
}

void vKirimSensor(){
  if (!quickEspNow.send(sensorDinding, (uint8_t *)order.c_str(), order.length()))
    {
        Serial.printf(">>>>>>>>>> Message sent\n");
        Serial.println(order);
        // data[0] = "";  // Clear for next cycle
    }
    else
    {
        Serial.printf(">>>>>>>>>> Message not sent\n");
        sent = true; // In case of error we need to set the flag to true to avoid blocking the loop
    }
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
  unsigned long now = millis();
  if (now - lastUpdate >= stepDelay) {
    lastUpdate = now;

    if (currentPos < targetPos) {
      currentPos += stepSize;
      if (currentPos > targetPos) currentPos = targetPos;
      servo1.write(currentPos);
    } 
    else if (currentPos > targetPos) {
      currentPos -= stepSize;
      if (currentPos < targetPos) currentPos = targetPos;
      servo1.write(currentPos);
    }
  }
  // put your main code here, to run repeatedly:
  if(millis() - now1S > millis1S){
    // vSuhuDS();
    // vSuhuRTD();
    String output = "";
      for (int j = 0; j < 1; j++) {
        output += data[j];
        if (j < 1) output += ",";
        Serial.print("Suhu Dinding ");
        Serial.print(j);
        Serial.print(" = ");
        Serial.println(data[j]);  // Cetak nilai masing-masing
        // data[j] = "";  // Clear for next cycle
      }
    // Serial.println("now>> " + output);
    vSuhuBME280();
    Raw_WCS = digitalRead(PIN_WCS);
    Serial.print("Raw WCS = ");
    Serial.println(Raw_WCS);
    if(Raw_WCS){
      h = 0;
      Suhu_Tunnel = 0;
    }
    Serial.println("===================================================");

    waitFeedback = 1;
    vKirimSensor();
    

    
    

    Serial.println("===================================================");
    


    now1S = millis();
  }


  if(waitFeedback){
    if(millis() - prevMillis > millis100S){
      countGagal++;
      Serial.println(countGagal);
      if (countGagal > 3) 
      {
        vKirimSensor();
      }
      prevMillis = millis();
    }

    

    if (countGagal == 6) 
    {
      countGagal = 0;
      vKirimPusat();
      Serial.println("dari countGagal");
      waitFeedback = 0;
    }
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
