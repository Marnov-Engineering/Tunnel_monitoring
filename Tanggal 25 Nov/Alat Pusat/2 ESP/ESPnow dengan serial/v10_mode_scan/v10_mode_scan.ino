#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <QuickEspNow.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Adafruit_ADS1X15.h>

#define RX_PIN 16
#define TX_PIN 17

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
// Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

AsyncWebServer server(80);
int8_t max_tx_power = 80; // 20 dBm

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

bool ModeScan = 0;

String SData;
String SDataHeat;


static const String msg = "send cmd";
const unsigned int SEND_MSG_MSEC = 50;

unsigned long countToReport;
unsigned long cycleStartTime;

// static uint8_t receiver[] = {0x1C, 0x69, 0x20, 0x96, 0x77, 0x60}; // MAC address tujuan

static uint8_t target1[] = {0x1C, 0x69, 0x20, 0x96, 0x77, 0x60};
static uint8_t target2[] = {0xEC, 0xE3, 0x34, 0xD7, 0x70, 0x60};
static uint8_t target3[] = {0x1C, 0x69, 0x20, 0x96, 0xEC, 0xC8};

static uint8_t targetALL[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t newMACAddress[] = {0x1C, 0x69, 0x20, 0x96, 0x31, 0xD4};


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

// --- Deklarasi global array MAC ---
String macSensorDalam[7];   
String macUjungSensor[2];   
String macHeater[3];        
String macDinding[3]; 

int countSensorDalam = 0; 
int countUjungSensor = 0; 
int countHeater = 0;
int countDinding = 0;

// static uint8_t dust13[] = {0xFC, 0xF5, 0xC4, 0xA6, 0xEA, 0x75};
// static uint8_t dust14[] = {0xE8, 0x68, 0xE7, 0xC7, 0xFB, 0x56};
// static uint8_t wind15[] = {0x10, 0x06, 0x1C, 0x68, 0x33, 0xA4};
// static uint8_t wind16[] = {0x3C, 0x8A, 0x1F, 0xA3, 0xEF, 0x8C};
// static uint8_t wind17[] = {0x10, 0x06, 0x1C, 0x68, 0x29, 0xC8};
// static uint8_t wind18[] = {0x10, 0x06, 0x1C, 0x68, 0x1B, 0xDC}; 
// static uint8_t valve20[] = {0x1C, 0x69, 0x20, 0x96, 0xD6, 0x60};

const uint8_t RECEIVER_COUNT = 15;

static uint8_t* receivers[RECEIVER_COUNT] = {
  sensorDalam1, sensorDalam2, sensorDalam3, sensorDalam4, sensorDalam5,
  sensorDalam6, sensorDalam7,
  sensorUjung1, sensorUjung2,
  sensorDinding1,sensorDinding2,sensorDinding3,
  target1,target2,target3
};

// daftar target
uint8_t *targets[] = {target1, target2, target3, sensorDalam1, sensorDalam2, sensorDalam3,
                      sensorDalam4, sensorDalam5, sensorDalam6, sensorDalam7,
                      sensorUjung1, sensorUjung2};
const int targetCount = sizeof(targets) / sizeof(targets[0]);

bool needRetry[targetCount];   // flag untuk retry
bool sentDone[targetCount];    // sudah pernah dicoba kirim

// Jumlah data yang dikirim setiap node
const uint8_t dataPerNode[RECEIVER_COUNT] = {
  2, 2, 2, 2, 2, 2, 2,   // 7 sensorDalam → 2 data per node
  3, 3, 1, 1, 1, 13, 13, 13                   // 2 sensorUjung → 3 data per node
};


String data[62];        // buffer input baru
String lastData[62];    // buffer nilai terakhir yang valid
int zeroCount[62] = {0}; // penghitung 0 berturut-turut

bool alreadySent = false;



// helper: cek apakah index perlu pakai filter
bool useFilter(int idx) {
  if (idx >= 0 && idx <= 19) return true;
  if (idx == 27 || idx == 28 || idx == 40 || idx == 41 || idx == 53 || idx == 54) return true;
  return false;
}


String dataHeater[62];

bool kirimHeaterFlag = false;
bool sent = true;


// void dataSent(uint8_t* address, uint8_t status) {
//   sent = true;
//   Serial.printf("Message sent to " MACSTR ", status: %d\n", MAC2STR(address), status);
// }

// callback esp-now
void dataSent(uint8_t* address, uint8_t status) {
  sent = true;
  Serial.printf("Message sent to " MACSTR ", status: %d\n", MAC2STR(address), status);

  // cocokkan address ke daftar target
  for (int i = 0; i < targetCount; i++) {
    if (memcmp(address, targets[i], 6) == 0) {
      sentDone[i] = true;
      needRetry[i] = (status != 0); // retry kalau status != 0
      break;
    }
  }
}

void kirimPesanSemua(const String &message) {
  // reset flag
  // for (int i = 0; i < targetCount; i++) {
  //   needRetry[i] = false;
  //   sentDone[i] = false;
  // }

  // // kirim percobaan pertama
  // for (int i = 0; i < targetCount; i++) {
  //   quickEspNow.send(targets[i], (uint8_t*)message.c_str(), message.length());
  //   delay(50); // jeda antar kirim
  // }
  quickEspNow.send(targetALL, (uint8_t*)message.c_str(), message.length());
    // delay(50); // jeda antar kirim
}

void retryFailed(const String &message, unsigned long startTime) {
  // cek yang gagal
  for (int i = 0; i < targetCount; i++) {
    if (needRetry[i] && (millis() - startTime < 5000)) {
      Serial.printf("Retry kirim ke target %d\n", i);
      quickEspNow.send(targets[i], (uint8_t*)message.c_str(), message.length());
      delay(50);
      needRetry[i] = false; // hanya coba sekali lagi
    }
  }
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

    if (incoming.startsWith("HASIL SCAN ALAT TUNNEL")) {
      int idx = incoming.indexOf("MAC");
      if (idx >= 0) { // >=0 bukan >0, supaya kalau "MAC" ada di index 0 tetap terbaca
        String macStr = incoming.substring(idx + 3); // ambil setelah "MAC"
        macStr.trim();

        Serial.printf("[DEBUG] Pesan: %s\n", incoming.c_str());
        Serial.printf("[DEBUG] Hasil macStr: %s\n", macStr.c_str());

        if (incoming.indexOf("SENSOR DALAM") > 0) {
          if (countSensorDalam < 7) {
            macSensorDalam[countSensorDalam++] = macStr;
            Serial.printf(">> Sensor Dalam ke-%d: %s\n", countSensorDalam, macStr.c_str());
          }
        } 
        else if (incoming.indexOf("UJUNG SENSOR") > 0) {
          if (countUjungSensor < 2) {
            macUjungSensor[countUjungSensor++] = macStr;
            Serial.printf(">> Ujung Sensor ke-%d: %s\n", countUjungSensor, macStr.c_str());
          }
        } 
        else if (incoming.indexOf("HEATER") > 0) {
          // cari nomor heater
          int numIdx = incoming.indexOf("HEATER") + 6; // setelah kata "HEATER "
          int nomor = incoming.substring(numIdx, numIdx + 1).toInt(); // ambil angka 1/2/3

          if (nomor >= 1 && nomor <= 3) {
            int idxHeater = nomor - 1;
            macHeater[idxHeater] = macStr;
            Serial.printf(">> Heater %d diset ke %s\n", nomor, macStr.c_str());

            // pastikan counter minimal sebesar jumlah heater yang sudah terisi
            if (countHeater < nomor) countHeater = nomor;
          } else {
            Serial.println(">> Heater: nomor tidak valid");
          }
        }

        else if (incoming.indexOf("DINDING") > 0) {
          if (countDinding < 3) {
            macDinding[countDinding++] = macStr;
            Serial.printf(">> Dinding ke-%d: %s\n", countDinding, macStr.c_str());
          }
        }
      }
      return;
    }


  // ==== PARSING SENSOR (program lama) ====
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
    dataIndex += dataPerNode[node]; // Menambahkan offset jumlah data yang dilewati
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
    for (int j = 0; j < 62; j++) {
      output += data[j];
      if (j < 61) output += ",";
      Serial.print("data[");
      Serial.print(j);
      Serial.print("] = ");
      Serial.println(data[j]);  // Cetak nilai masing-masing
      // data[j] = "";  // Clear for next cycle
    }

    
    reset_arr++;
    Serial.println("now>> " + output);
    Serial2.println("<" + output + ">");  // format dengan pembuka & penutup

    for (int j = 0; j < 62; j++) {
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
  // esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  // if (err == ESP_OK) {
  //   Serial.println("Success changing Mac Address");
  // }
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

      // ==== CEK PERINTAH MODE SCAN ====
      if (lastSerialData.equalsIgnoreCase("MODE SCAN 1")) {
        ModeScan = true;
        Serial.println(">>>>> MODE SCAN AKTIF <<<<<");
      } else if (lastSerialData.equalsIgnoreCase("MODE SCAN 0")) {
        ModeScan = false;
        Serial.println(">>>>> MODE SCAN NONAKTIF <<<<<");
      } else {
        // ==== Kirim data melalui ESP-NOW (jika bukan perintah MODE SCAN) ====
        if (!quickEspNow.send(target1, (uint8_t *)lastSerialData.c_str(), lastSerialData.length())) {
          Serial.println(">>>>>>>>>> Message sent");
        } else {
          Serial.println(">>>>>>>>>> Message not sent");
        }

        if (!quickEspNow.send(target2, (uint8_t *)lastSerialData.c_str(), lastSerialData.length())) {
          Serial.println(">>>>>>>>>> Message sent");
        } else {
          Serial.println(">>>>>>>>>> Message not sent");
        }

        if (!quickEspNow.send(target3, (uint8_t *)lastSerialData.c_str(), lastSerialData.length())) {
          Serial.println(">>>>>>>>>> Message sent");
        } else {
          Serial.println(">>>>>>>>>> Message not sent");
        }
      }

      serialBuffer = ""; // Reset buffer setelah diproses
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
  esp_wifi_set_max_tx_power(max_tx_power);
  vSetupEspNow();
  // Pengirim dan penerima harus punya inisialisasi yang sama
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); 
  
  // vAsyncWebServer();
  
  // vSetupADS();
}

void loop() {
  static unsigned long cycleStartTime = 0;

  unsigned long elapsed = millis() - cycleStartTime;

  // --- DETIK KE-3 : Kirim ESP-NOW ---
  if (elapsed >= 2500 && elapsed < 5000) {
    
    // if (!alreadySent) {
    //   String message = "KIRIM SEMUA DATA";

    //   if (!quickEspNow.send(target1, (uint8_t *)message.c_str(), message.length())) {
    //     Serial.println(">>>>>>>>>> Message sent to target1");
    //   } else {
    //     Serial.println(">>>>>>>>>> Message not sent to target1");
    //   }
    //   delay(50);

    //   if (!quickEspNow.send(target2, (uint8_t *)message.c_str(), message.length())) {
    //     Serial.println(">>>>>>>>>> Message sent to target2");
    //   } else {
    //     Serial.println(">>>>>>>>>> Message not sent to target2");
    //   }
    //   delay(50);

    //   if (!quickEspNow.send(target3, (uint8_t *)message.c_str(), message.length())) {
    //     Serial.println(">>>>>>>>>> Message sent to target3");
    //   } else {
    //     Serial.println(">>>>>>>>>> Message not sent to target3");
    //   }
    //   delay(50);

    //   if (!quickEspNow.send(sensorDalam1, (uint8_t *)message.c_str(), message.length())) {
    //     Serial.println(">>>>>>>>>> Message sent to sensorDalam1");
    //   } else {
    //     Serial.println(">>>>>>>>>> Message not sent to sensorDalam1");
    //   }
    //   delay(50);

    //   if (!quickEspNow.send(sensorDalam2, (uint8_t *)message.c_str(), message.length())) {
    //     Serial.println(">>>>>>>>>> Message sent to sensorDalam2");
    //   } else {
    //     Serial.println(">>>>>>>>>> Message not sent to sensorDalam2");
    //   }
    //   delay(50);

    //   if (!quickEspNow.send(sensorDalam3, (uint8_t *)message.c_str(), message.length())) {
    //     Serial.println(">>>>>>>>>> Message sent to sensorDalam3");
    //   } else {
    //     Serial.println(">>>>>>>>>> Message not sent to sensorDalam3");
    //   }
    //   delay(50);

    //   if (!quickEspNow.send(sensorDalam4, (uint8_t *)message.c_str(), message.length())) {
    //     Serial.println(">>>>>>>>>> Message sent to sensorDalam4");
    //   } else {
    //     Serial.println(">>>>>>>>>> Message not sent to sensorDalam4");
    //   }
    //   delay(50);

    //   if (!quickEspNow.send(sensorDalam5, (uint8_t *)message.c_str(), message.length())) {
    //     Serial.println(">>>>>>>>>> Message sent to sensorDalam5");
    //   } else {
    //     Serial.println(">>>>>>>>>> Message not sent to sensorDalam5");
    //   }
    //   delay(50);

    //   if (!quickEspNow.send(sensorDalam6, (uint8_t *)message.c_str(), message.length())) {
    //     Serial.println(">>>>>>>>>> Message sent to sensorDalam6");
    //   } else {
    //     Serial.println(">>>>>>>>>> Message not sent to sensorDalam6");
    //   }
    //   delay(50);

    //   if (!quickEspNow.send(sensorDalam7, (uint8_t *)message.c_str(), message.length())) {
    //     Serial.println(">>>>>>>>>> Message sent to sensorDalam7");
    //   } else {
    //     Serial.println(">>>>>>>>>> Message not sent to sensorDalam7");
    //   }
    //   delay(50);

    //   if (!quickEspNow.send(sensorUjung1, (uint8_t *)message.c_str(), message.length())) {
    //     Serial.println(">>>>>>>>>> Message sent to sensorUjung1");
    //   } else {
    //     Serial.println(">>>>>>>>>> Message not sent to sensorUjung1");
    //   }
    //   delay(50);

    //   if (!quickEspNow.send(sensorUjung2, (uint8_t *)message.c_str(), message.length())) {
    //     Serial.println(">>>>>>>>>> Message sent to sensorUjung2");
    //   } else {
    //     Serial.println(">>>>>>>>>> Message not sent to sensorUjung2");
    //   }
    //   delay(50);

    //   alreadySent = true; // biar tidak spam terus
    // }
    if(!ModeScan){
      if (!alreadySent) {
        String message = "KIRIM SEMUA DATA";
        kirimPesanSemua(message);
        alreadySent = true;
      } else {
        // retry selama masih < 5 detik
        String message = "KIRIM SEMUA DATA";
        // retryFailed(message, cycleStartTime);
      }
    }

    else{
      if(!alreadySent){
        String message = "SCAN ALAT TUNNEL";
        kirimPesanSemua(message);
        alreadySent = true;
      }
    }
  }

  // --- DETIK KE-5 : Kirim Serial Data ---
  if (elapsed >= 5000) {
    if(!ModeScan){
      String output = "";
      for (int j = 0; j < 62; j++) {
        String val = data[j];
        val.trim();

        if (useFilter(j)) {
          // ---- LOGIKA FILTER ----
          if (lastData[j].length() == 0 && val == "0") {
            lastData[j] = "0";
            zeroCount[j] = 0;
          } else if (val == "0") {
            if (zeroCount[j] >= 2) {
              val = "0";
              lastData[j] = "0";
              zeroCount[j] = 0;
            } else {
              val = lastData[j];
              zeroCount[j]++;
            }
          } else {
            lastData[j] = val;
            zeroCount[j] = 0;
          }
        } else {
          lastData[j] = val;
          zeroCount[j] = 0;
        }

        // rakit output
        output += val;
        if (j < 61) output += ",";

        // debug
        Serial.print("data[");
        Serial.print(j);
        Serial.print("] = ");
        Serial.print(data[j]);
        Serial.print(" | kirim = ");
        Serial.println(val);
      }

      Serial.println("now>> " + output);
      Serial2.println("<" + output + ">");

      // reset data[] untuk siklus berikutnya
      for (int j = 0; j < 62; j++) {
        data[j] = "0";
      }
      Serial.println("RESET");

      // reset cycle
      cycleStartTime = millis();
      // reset flag untuk kirim esp-now di siklus berikutnya
      alreadySent = false;
    }
    else{
        // kirimkan semua mac hasil scan
        Serial.println("PENGIRIMAN MAC");
        String output = "";

        // kirim sensor dalam
        for (int i = 0; i < countSensorDalam; i++) {
          output = "HASIL SCAN ALAT TUNNEL SENSOR DALAM " + String(i + 1) + " MAC " + macSensorDalam[i];
          Serial2.println("<" + output + ">");
          Serial.println(output);
          delay(20);
        }

        // kirim ujung sensor
        for (int i = 0; i < countUjungSensor; i++) {
          output = "HASIL SCAN ALAT TUNNEL UJUNG SENSOR " + String(i + 1) + " MAC " + macUjungSensor[i];
          Serial2.println("<" + output + ">");
          Serial.println(output);
          delay(20);
        }

        // kirim heater
        for (int i = 0; i < countHeater; i++) {
          output = "HASIL SCAN ALAT TUNNEL HEATER " + String(i + 1) + " MAC " + macHeater[i];
          Serial2.println("<" + output + ">");
          Serial.println(output);
          delay(20);
        }

        // kirim dinding
        for (int i = 0; i < countDinding; i++) {
          output = "HASIL SCAN ALAT TUNNEL DINDING " + String(i + 1) + " MAC " + macDinding[i];
          Serial2.println("<" + output + ">");
          Serial.println(output);
          delay(20);
        }
      cycleStartTime = millis();
      // reset flag untuk kirim esp-now di siklus berikutnya
      alreadySent = false;
    }
  }

  // tetap jalankan fungsi rutin lain
  vPembacaanSerial();
  // vLoopESPnow();
}



