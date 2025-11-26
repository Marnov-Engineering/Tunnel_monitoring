#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <QuickEspNow.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Adafruit_ADS1X15.h>
#include <Preferences.h>


Preferences preferences;

#define RX_PIN 16
#define TX_PIN 17

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
// Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

AsyncWebServer server(80);
int8_t max_tx_power = 80; // 20 dBm

unsigned long DuaDetik = 2000;
unsigned long millisPengiriman = 0;

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



static uint8_t targetALL[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t newMACAddress[] = {0x1C, 0x69, 0x20, 0x96, 0xFC, 0x60};


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

static uint8_t heater1[] = {0x1C, 0x69, 0x20, 0x96, 0x77, 0x60};
static uint8_t heater2[] = {0xEC, 0xE3, 0x34, 0xD7, 0x70, 0x60};
static uint8_t heater3[] = {0x1C, 0x69, 0x20, 0x96, 0xEC, 0xC8};


// static uint8_t sensorDinding1[] = {0x2C, 0x3A, 0xE8, 0x14, 0x85, 0x8F};
// static uint8_t sensorDinding2[] = {0x58, 0xBF, 0x25, 0xC2, 0xFE, 0x9F};
// static uint8_t sensorDinding3[] = {0xEC, 0xFA, 0xBC, 0x41, 0x6E, 0xDF};

static uint8_t sensorDinding1[6] = { 0x2C, 0x3A, 0xE8, 0x14, 0x85, 0x8F };
static uint8_t sensorDinding2[6] = { 0x58, 0xBF, 0x25, 0xC2, 0xFE, 0x9F };
static uint8_t sensorDinding3[6] = { 0x4C, 0x11, 0xAE, 0x03, 0x64, 0xF1 };

static uint8_t sensorDinding4[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
static uint8_t sensorDinding5[] = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB};
static uint8_t sensorDinding6[] = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC};
// static uint8_t wind16[] = {0xCC, 0x8A, 0x1F, 0xA3, 0xEF, 0x8C};
// static uint8_t wind17[] = {0x10, 0x06, 0x1C, 0x68, 0x29, 0xC8};
// static uint8_t wind18[] = {0x10, 0x06, 0x1C, 0x68, 0x1B, 0xDC}; 
// static uint8_t valve20[] = {0x1C, 0x69, 0x20, 0x96, 0xD6, 0x60};

const uint8_t RECEIVER_COUNT = 15;

static uint8_t* receivers[RECEIVER_COUNT] = {
  sensorDalam1, sensorDalam2, sensorDalam3, sensorDalam4, sensorDalam5,
  sensorDalam6, sensorDalam7,
  sensorUjung1, sensorUjung2,
  sensorDinding4,sensorDinding5,sensorDinding6,
  heater1,heater2,heater3
};

// daftar target
uint8_t *targets[] = {heater1, heater2, heater3, sensorDalam1, sensorDalam2, sensorDalam3,
                      sensorDalam4, sensorDalam5, sensorDalam6, sensorDalam7,
                      sensorUjung1, sensorUjung2};
const int targetCount = sizeof(targets) / sizeof(targets[0]);

// bool needRetry[targetCount];   // flag untuk retry

bool sentDone[targetCount];    // sudah pernah dicoba kirim

// Jumlah data yang dikirim setiap node
const uint8_t dataPerNode[RECEIVER_COUNT] = {
  3, 3, 3, 3, 3, 3, 3,   // 7 sensorDalam → 2 data per node
  5, 5, 1, 1, 1, 14, 14, 14                   // 2 sensorUjung → 3 data per node
};


String data[76];        // buffer input baru
String lastData[76];    // buffer nilai terakhir yang valid
int zeroCount[76] = {0}; // penghitung 0 berturut-turut

bool alreadySent = false;

// MAC khusus
static uint8_t macKhusus[6] = {0x1C, 0x69, 0x20, 0x96, 0x31, 0xD4};

// Fungsi pembanding MAC
bool macEquals(const uint8_t *a, const uint8_t *b) {
  for (int i = 0; i < 6; i++) if (a[i] != b[i]) return false;
  return true;
}

bool macCompare(const uint8_t *a, const uint8_t *b, const char* tag = "") {

  Serial.print("Membandingkan MAC ");
  Serial.print(tag);
  Serial.println(":");

  Serial.print("  A = ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", a[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  Serial.print("  B = ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", b[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  // Cek byte per byte
  for (int i = 0; i < 6; i++) {
    if (a[i] != b[i]) {
      Serial.printf("  → Byte %d tidak cocok: %02X != %02X\n", i, a[i], b[i]);
      Serial.println("  → Hasil: TIDAK SAMA\n");
      return false;
    }
  }

  Serial.println("  → Semua byte cocok! MAC SAMA\n");
  return true;
}




// helper: cek apakah index perlu pakai filter
bool useFilter(int idx) {
  if (idx >= 0 && idx <= 30) return true;
  if (idx == 38 || idx == 39 || idx == 53 || idx == 52 || idx == 66 || idx == 67) return true;
  return false;
}


String dataHeater[76];
String dataMAC[500];

bool kirimHeaterFlag = false;
bool sent = true;

struct MacEntry {
  const char *name;
  uint8_t *addr;
};

MacEntry macList[] = {
  {"sensorDalam1", sensorDalam1},
  {"sensorDalam2", sensorDalam2},
  {"sensorDalam3", sensorDalam3},
  {"sensorDalam4", sensorDalam4},
  {"sensorDalam5", sensorDalam5},
  {"sensorDalam6", sensorDalam6},
  {"sensorDalam7", sensorDalam7},
  {"sensorUjung1", sensorUjung1},
  {"sensorUjung2", sensorUjung2},
  {"heater1",     heater1},
  {"heater2",     heater2},
  {"heater3",     heater3},
  {"sensorDinding1", sensorDinding1},
  {"sensorDinding2", sensorDinding2},
  {"sensorDinding3", sensorDinding3}
};

// --- Mapping untuk EEPROM ---
struct MacPref {
  const char *key;
  uint8_t *addr;
};

MacPref macPrefs[] = {
  {"SensDlm1", sensorDalam1},
  {"SensDlm2", sensorDalam2},
  {"SensDlm3", sensorDalam3},
  {"SensDlm4", sensorDalam4},
  {"SensDlm5", sensorDalam5},
  {"SensDlm6", sensorDalam6},
  {"SensDlm7", sensorDalam7},

  {"SensUjn1", sensorUjung1},
  {"SensUjn2", sensorUjung2},

  {"Heater1", heater1},
  {"Heater2", heater2},
  {"Heater3", heater3},

  {"Dindng1", sensorDinding1},
  {"Dindng2", sensorDinding2},
  {"Dindng3", sensorDinding3},
};


// void dataSent(uint8_t* address, uint8_t status) {
//   sent = true;
//   Serial.printf("Message sent to " MACSTR ", status: %d\n", MAC2STR(address), status);
// }
bool needRetry[sizeof(macPrefs)/sizeof(macPrefs[0])] = {false};
// callback esp-now
void dataSent(uint8_t* address, uint8_t status) {
  sent = true;
  Serial.printf("Message sent to " MACSTR ", status: %d\n", MAC2STR(address), status);
  // cocokkan dengan macPrefs
  for (int i = 0; i < (int)(sizeof(macPrefs)/sizeof(macPrefs[0])); i++) {
    if (memcmp(address, macPrefs[i].addr, 6) == 0) {
      if (status == ESP_NOW_SEND_SUCCESS) {
        needRetry[i] = false;  // sukses → clear retry
      } else {
        needRetry[i] = true;   // gagal → retry di loop berikutnya
      }
    }
  }
}

// void dataSent(const uint8_t *mac_addr, uint8_t status) {
//   String macStr = macToString((uint8_t*)mac_addr);
//   Serial.printf("Message sent to %s, status: %d\n", macStr.c_str(), status);

//   // cocokkan dengan macPrefs
//   for (int i = 0; i < (int)(sizeof(macPrefs)/sizeof(macPrefs[0])); i++) {
//     if (memcmp(mac_addr, macPrefs[i].addr, 6) == 0) {
//       if (status == ESP_NOW_SEND_SUCCESS) {
//         needRetry[i] = false;  // sukses → clear retry
//       } else {
//         needRetry[i] = true;   // gagal → retry di loop berikutnya
//       }
//     }
//   }
// }


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

void kirimPesanKeHeater(const String &message) {

  quickEspNow.send(heater1, (uint8_t*)message.c_str(), message.length());
  quickEspNow.send(heater2, (uint8_t*)message.c_str(), message.length());
  quickEspNow.send(heater3, (uint8_t*)message.c_str(), message.length());
}

// void retryFailed(const String &message, unsigned long startTime) {
//   // cek yang gagal
//   for (int i = 0; i < targetCount; i++) {
//     if (needRetry[i] && (millis() - startTime < 5000)) {
//       Serial.printf("Retry kirim ke target %d\n", i);
//       quickEspNow.send(targets[i], (uint8_t*)message.c_str(), message.length());
//       delay(50);
//       needRetry[i] = false; // hanya coba sekali lagi
//     }
//   }
// }

bool macMatch(uint8_t* a, uint8_t* b) {
  for (int i = 0; i < 6; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

// void dataReceived(uint8_t* address, uint8_t* dataRaw, uint8_t len, signed int rssi, bool broadcast) {
  
//   String incoming = "";
//   for (int i = 0; i < len; i++) incoming += (char)dataRaw[i];

//   for (int node = 0, dataIndex = 0; node < RECEIVER_COUNT; node++) {
//     if (macMatch(address, receivers[node])) {
//       int count = dataPerNode[node];
//       int idx = 0;
//       for (int j = 0; j < count; j++) {
//         int nextComma = incoming.indexOf(',', idx);
//         String val;
//         if (nextComma != -1) {
//           val = incoming.substring(idx, nextComma);
//           idx = nextComma + 1;
//         } else {
//           val = incoming.substring(idx);
//           idx = incoming.length();  // force exit
//         }

//         data[dataIndex + j] = val;
//       }
//       break;
//     }
//     // Menambahkan offset jumlah data yang dilewati
//     dataIndex += dataPerNode[node];
//   }
//   // Serial.println("masuk sini>> ");
//   //   String output = "";
//   //   for (int j = 0; j < 23; j++) {
//   //     output += data[j];
//   //     if (j < 22) output += ",";
//   //     Serial.print("data[");
//   //     Serial.print(j);
//   //     Serial.print("] = ");
//   //     Serial.println(data[j]);  // Cetak nilai masing-masing
//   //     // data[j] = "";  // Clear for next cycle
//   //   }
// }

// void dataReceived(uint8_t* address, uint8_t* dataRaw, uint8_t len, signed int rssi, bool broadcast) {

//   // ---- Ambil data string ----
//   String incoming = "";
//   for (int i = 0; i < len; i++) incoming += (char)dataRaw[i];

//   // ----------------------------
//   // 1. JIKA DARI MAC KHUSUS
//   // ----------------------------
//   if (macEquals(address, macKhusus)) {

//     if (incoming == "KIRIM SEMUA DATA") {

//         // === AKSI KHUSUS DI SINI ===
//         Serial.println("PERINTAH DARI MAC KHUSUS: KIRIM SEMUA DATA");
//         String message = "KIRIM SEMUA DATA";
//         kirimPesanKeHeater(message);

//         return; // selesai, jangan parsing
//     }

//     int commaPos = incoming.indexOf(',');
//     if (commaPos == -1) return; // format salah, skip

//     String macText = incoming.substring(0, commaPos);
//     String payload = incoming.substring(commaPos + 1);

//     // konversi MAC text → byte array
//     uint8_t macReal[6];
//     sscanf(macText.c_str(), "%hhX:%hhX:%hhX:%hhX:%hhX:%hhX",
//            &macReal[0], &macReal[1], &macReal[2],
//            &macReal[3], &macReal[4], &macReal[5]);

//     // PARSING sesuai macReal
//     if (macEquals(macReal, sensorDinding1)){
//       // ganti data 38 dengan data payload
//     }
//     else if (macEquals(macReal, sensorDinding2)){
//       // ganti data 52 dengan data payload
//     }
//     else if (macEquals(macReal, sensorDinding3)){
//       // ganti data 66 dengan data payload
//     }
//     else{
//       for (int node = 0, dataIndex = 0; node < RECEIVER_COUNT; node++) {

//         if (macMatch(macReal, receivers[node])) {

//           int count = dataPerNode[node];
//           int idx = 0;

//           for (int j = 0; j < count; j++) {
//             int nextComma = payload.indexOf(',', idx);
//             String val;

//             if (nextComma != -1) {
//               val = payload.substring(idx, nextComma);
//               idx = nextComma + 1;
//             } else {
//               val = payload.substring(idx);
//               idx = payload.length();
//             }

//             data[dataIndex + j] = val;
//           }

//           return;  // selesai
//         }

//         dataIndex += dataPerNode[node];
//       }
//     }

//     return; // selesai MAC khusus
//   }
//   // ----------------------------
//   // 2. JIKA BUKAN MAC KHUSUS → lakukan seperti program asli kamu
//   // ----------------------------
//   for (int node = 0, dataIndex = 0; node < RECEIVER_COUNT; node++) {

//     if (macMatch(address, receivers[node])) {
//       int count = dataPerNode[node];
//       int idx = 0;

//       for (int j = 0; j < count; j++) {
//         int nextComma = incoming.indexOf(',', idx);
//         String val;

//         if (nextComma != -1) {
//           val = incoming.substring(idx, nextComma);
//           idx = nextComma + 1;
//         } else {
//           val = incoming.substring(idx);
//           idx = incoming.length();
//         }

//         data[dataIndex + j] = val;
//       }
//       return;
//     }

//     dataIndex += dataPerNode[node];
//   }
// }

void dataReceived(uint8_t* address, uint8_t* dataRaw, uint8_t len, signed int rssi, bool broadcast) {

  // ======================
  // AMBIL RAW STRING
  // ======================
  String incoming = "";
  for (int i = 0; i < len; i++) incoming += (char)dataRaw[i];

  // ======================
  // JIKA DARI MAC KHUSUS
  // ======================
  if (macEquals(address, macKhusus)) {

    // --- Perintah spesial ---
    if (incoming == "KIRIM SEMUA DATA") {
      Serial.println("PERINTAH DARI MAC KHUSUS: KIRIM SEMUA DATA");
      kirimPesanKeHeater("KIRIM SEMUA DATA");
      return;
    }

    // ===========================
    // Ambil MAC Real di dalam pesan
    // Format: "AA:BB:CC:DD:EE:FF,DATA"
    // ===========================
    int commaPos = incoming.indexOf(',');
    if (commaPos == -1) return;

    String macText = incoming.substring(0, commaPos);
    String payload = incoming.substring(commaPos + 1);

    macText.trim();
    macText.toUpperCase();

    // ===========================
    // Konversi ke byte array
    // ===========================
    uint8_t macReal[6];
    int parsed = sscanf(
      macText.c_str(),
      " %hhX:%hhX:%hhX:%hhX:%hhX:%hhX",
      &macReal[0], &macReal[1], &macReal[2],
      &macReal[3], &macReal[4], &macReal[5]
    );

    if (parsed != 6) {
      // Serial.println("ERROR: MAC FORMAT SALAH");
      return;
    }

    // Debug cek MAC REAL
    // Serial.print("macReal = ");
    // for (int i = 0; i < 6; i++) {
    //   Serial.printf("%02X", macReal[i]);
    //   if (i < 5) Serial.print(":");
    // }
    // Serial.println();

    // ===========================
    // 1. SENSOR DINDING 1
    // ===========================
    if (macEquals(macReal, sensorDinding1)) {
      Serial.print("MATCH → sensorDinding1 = ");
      Serial.println(payload);
      data[38] = payload;
      return;
    }

    // ===========================
    // 2. SENSOR DINDING 2
    // ===========================
    if (macEquals(macReal, sensorDinding2)) {
      Serial.print("MATCH → sensorDinding2 = ");
      Serial.println(payload);
      data[52] = payload;
      return;
    }

    // ===========================
    // 3. SENSOR DINDING 3
    // ===========================
    if (macEquals(macReal, sensorDinding3)) {
      Serial.print("MATCH → sensorDinding3 = ");
      Serial.println(payload);
      data[66] = payload;
      return;
    }

    // ===========================
    // 4. PARSING NORMAL NODE
    // ===========================
    // Serial.println("MATCH → TIDAK ADA YANG MATCh");
    for (int node = 0, dataIndex = 0; node < RECEIVER_COUNT; node++) {

      if (macMatch(macReal, receivers[node])) {

        int count = dataPerNode[node];
        int idx = 0;

        for (int j = 0; j < count; j++) {
          int nextComma = payload.indexOf(',', idx);
          String val;

          if (nextComma != -1) {
            val = payload.substring(idx, nextComma);
            idx = nextComma + 1;
          } else {
            val = payload.substring(idx);
            idx = payload.length();
          }

          int targetIndex = dataIndex + j;

          // --- SKIP posisi 38, 52, 66 ---
          if (targetIndex == 38 || targetIndex == 52 || targetIndex == 66) {
            Serial.printf("SKIP index %d, biarkan data lama: %s\n",
                          targetIndex, data[targetIndex].c_str());
            continue;  // jangan overwrite data
          }

          // Simpan data
          data[targetIndex] = val;
        }

        return;
      }

      dataIndex += dataPerNode[node];
    }


    return; // selesai MAC khusus
  }

  // ===========================
  // JIKA BUKAN MAC KHUSUS
  // ===========================
  for (int node = 0, dataIndex = 0; node < RECEIVER_COUNT; node++) {

    if (macMatch(address, receivers[node])) {

      int count = dataPerNode[node];
      int idx = 0;

      for (int j = 0; j < count; j++) {

        // Ambil nilai dari incoming
        int nextComma = incoming.indexOf(',', idx);
        String val;

        if (nextComma != -1) {
          val = incoming.substring(idx, nextComma);
          idx = nextComma + 1;
        } else {
          val = incoming.substring(idx);
          idx = incoming.length();
        }

        // Hitung index sesungguhnya di array data[]
        int targetIndex = dataIndex + j;

        // ---- SKIP posisi 38, 52, 66 ----
        if (targetIndex == 38 || targetIndex == 52 || targetIndex == 66) {
          Serial.printf("SKIP index %d (biarkan data lama: %s)\n",
                        targetIndex, data[targetIndex].c_str());
          continue;  // Jangan overwrite
        }

        // ---- Simpan data ----
        data[targetIndex] = val;
      }

      return;
    }

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
    for (int j = 0; j < 76; j++) {
      output += data[j];
      if (j < 75) output += ",";
      Serial.print("data[");
      Serial.print(j);
      Serial.print("] = ");
      Serial.println(data[j]);  // Cetak nilai masing-masing
      // data[j] = "";  // Clear for next cycle
    }

    
    reset_arr++;
    Serial.println("now>> " + output);
    Serial2.println(output);  // format dengan pembuka & penutup

    for (int j = 0; j < 76; j++) {
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
  esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  if (err == ESP_OK) {
    Serial.println("Success changing Mac Address");
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

// void vPembacaanSerial() {
//   static String serialBuffer = "";

//   while (Serial2.available()) {
//     char c = Serial2.read();

//     if (c == '<') {
//       serialBuffer = "";
//     } else if (c == '>') {
//       String lastSerialData = serialBuffer; // Ambil data antara tanda < >
//       Serial.println("Data dari Serial2:");
//       Serial.println(lastSerialData);

//       // Kirim data melalui ESP-NOW
//       if (!quickEspNow.send(heater1, (uint8_t *)lastSerialData.c_str(), lastSerialData.length())) {
//         Serial.println(">>>>>>>>>> Message sent");
//       } else {
//         Serial.println(">>>>>>>>>> Message not sent");
//       }

//       if (!quickEspNow.send(heater2, (uint8_t *)lastSerialData.c_str(), lastSerialData.length())) {
//         Serial.println(">>>>>>>>>> Message sent");
//       } else {
//         Serial.println(">>>>>>>>>> Message not sent");
//       }

//       if (!quickEspNow.send(heater3, (uint8_t *)lastSerialData.c_str(), lastSerialData.length())) {
//         Serial.println(">>>>>>>>>> Message sent");
//       } else {
//         Serial.println(">>>>>>>>>> Message not sent");
//       }

//       serialBuffer = ""; // Reset buffer setelah dikirim
//     } 

//     if (c == '*') {
//       serialBuffer = "";
//     } else if (c == '*') {
//       String lastSerialData = serialBuffer; // Ambil data antara tanda < >
//       Serial.println("Data dari Serial2:");
//       Serial.println(lastSerialData);

//       int index = 0;
//       int start = 0;
//       for (int i = 0; i < lastSerialData.length(); i++) {
//         if (lastSerialData.charAt(i) == ',') {
//           dataMAC[index++] = lastSerialData.substring(start, i);
//           start = i + 1;
//         }
//       }
//       dataMAC[index] = lastSerialData.substring(start); // elemen terakhir
//       // Cetak semua nilai yang diterima

//       for (int i = 0; i <= index; i++) {
//         Serial.printf("MAC[%d] = %s\n", i, dataMAC[i].c_str());
//       }

//       serialBuffer = ""; // Reset buffer setelah dikirim
//     } else {
//       serialBuffer += c;
//     }


//   }
// }

// ubah string "AA:BB:CC:DD:EE:FF" → array uint8_t[6]
bool stringToMac(const String &macStr, uint8_t *macOut) {
  int values[6];
  if (sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x",
             &values[0], &values[1], &values[2],
             &values[3], &values[4], &values[5]) == 6) {
    for (int i = 0; i < 6; i++) {
      macOut[i] = (uint8_t)values[i];
    }
    return true;
  }
  return false;
}

// helper ubah array mac ke string
String macToString(const uint8_t *mac) {
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}


void vSetupEEPROM(){
  preferences.begin("my-app", false);
  for (int i = 0; i < sizeof(macPrefs) / sizeof(macPrefs[0]); i++) {
    preferences.getBytes(macPrefs[i].key, macPrefs[i].addr, 6);
  }

}

void saveMac(const char *key, uint8_t *addr) {
  // preferences.begin("macstore", false);
  preferences.putBytes(key, addr, 6);
  // preferences.end();
}



void vPembacaanSerial() {
  static String serialBuffer = "";
  static char startChar = 0; // pembuka aktif (‘<’ atau ‘^’)

  while (Serial2.available()) {
    char c = Serial2.read();

    if (c == '<' || c == '^') {
      // Awal paket
      serialBuffer = "";
      startChar = c;
    } 
    else if ((c == '>' && startChar == '<') || 
             (c == '*' && startChar == '^')) {
      // Akhir paket
      String lastSerialData = serialBuffer;
      Serial.println("Data dari Serial2:");
      Serial.println(lastSerialData);

      // if (startChar == '<') {
      //   // === kirim data ESP-NOW ===
      //   if (!quickEspNow.send(heater1, (uint8_t*)lastSerialData.c_str(), lastSerialData.length()))
      //     Serial.println(">>> sent to heater1");
      //   if (!quickEspNow.send(heater2, (uint8_t*)lastSerialData.c_str(), lastSerialData.length()))
      //     Serial.println(">>> sent to heater2");
      //   if (!quickEspNow.send(heater3, (uint8_t*)lastSerialData.c_str(), lastSerialData.length()))
      //     Serial.println(">>> sent to heater3");
      // }
      if (startChar == '<') {
        if (lastSerialData.startsWith("KIRIM HEATER")) {
          int heaterNum = -1;

          // cari kata "HEATER"
          int idxHeater = lastSerialData.indexOf("HEATER");
          if (idxHeater >= 0) {
            // ambil setelah kata "HEATER"
            String afterHeater = lastSerialData.substring(idxHeater + 6);
            afterHeater.trim();  // buang spasi

            // ambil angka pertama (sebelum spasi lagi)
            int spasiLagi = afterHeater.indexOf(' ');
            String numStr = (spasiLagi > 0) ? afterHeater.substring(0, spasiLagi) : afterHeater;

            heaterNum = numStr.toInt(); // konversi ke integer
          }

          Serial.printf("Perintah: %s (heaterNum=%d)\n", lastSerialData.c_str(), heaterNum);

          if (heaterNum == 1) {
            if (!quickEspNow.send(heater1, (uint8_t*)lastSerialData.c_str(), lastSerialData.length()))
              Serial.println(">>> sent to heater1");
          }
          else if (heaterNum == 2) {
            if (!quickEspNow.send(heater2, (uint8_t*)lastSerialData.c_str(), lastSerialData.length()))
              Serial.println(">>> sent to heater2");
          }
          else if (heaterNum == 3) {
            if (!quickEspNow.send(heater3, (uint8_t*)lastSerialData.c_str(), lastSerialData.length()))
              Serial.println(">>> sent to heater3");
          }
          else {
            Serial.println("Heater number tidak valid!");
          }
        }
        else {
          // fallback: broadcast ke semua heater
          if (!quickEspNow.send(heater1, (uint8_t*)lastSerialData.c_str(), lastSerialData.length()))
            Serial.println(">>> sent to heater1");
          if (!quickEspNow.send(heater2, (uint8_t*)lastSerialData.c_str(), lastSerialData.length()))
            Serial.println(">>> sent to heater2");
          if (!quickEspNow.send(heater3, (uint8_t*)lastSerialData.c_str(), lastSerialData.length()))
            Serial.println(">>> sent to heater3");
        }
      }


      else if (startChar == '^') {
        int index = 0, start = 0;
        for (int i = 0; i < lastSerialData.length(); i++) {
          if (lastSerialData.charAt(i) == ',') {
            dataMAC[index++] = lastSerialData.substring(start, i);
            start = i + 1;
          }
        }
        dataMAC[index] = lastSerialData.substring(start); // terakhir

        int total = min(index + 1, (int)(sizeof(macPrefs) / sizeof(macPrefs[0])));

        for (int i = 0; i < total; i++) {
          String macStr = dataMAC[i];
          macStr.toUpperCase();

          String current = macToString(macPrefs[i].addr);

          // kondisi: beda MAC atau masih perlu retry
          if (macStr != current || needRetry[i]) {
            if (macStr != current) {
              Serial.printf("[%s] beda! lama=%s baru=%s -> simpan EEPROM\n",
                            macPrefs[i].key, current.c_str(), macStr.c_str());

              // update memory
              stringToMac(macStr, macPrefs[i].addr);
              saveMac(macPrefs[i].key, macPrefs[i].addr);
            } else {
              Serial.printf("[%s] sama (%s), tapi masih needRetry -> kirim ulang\n",
                            macPrefs[i].key, current.c_str());
            }

            // === Kirim payload ke MAC (baru atau retry) ===
            String payload = "MAC " + String(macPrefs[i].key);
            esp_err_t result = quickEspNow.send(macPrefs[i].addr,
                                                (uint8_t*)payload.c_str(),
                                                payload.length());

            if (result == ESP_OK) {
              Serial.printf(">>> sent payload ke %s (%s)\n", macPrefs[i].key, macStr.c_str());
              needRetry[i] = false; // sukses, clear retry
            } else {
              Serial.printf(">>> gagal kirim ke %s (%s)\n", macPrefs[i].key, macStr.c_str());
              needRetry[i] = true;  // tetap tandai untuk retry berikutnya
            }
          } else {
            Serial.printf("[%s] sama (%s), tidak diubah & tidak perlu retry\n",
                          macPrefs[i].key, current.c_str());
          }
        }
      }




      serialBuffer = "";
      startChar = 0;
    } 
    else {
      // karakter payload
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
  
  Serial.println("selesai mendapatkan data");
  // reset_arr bisa diatur di sini juga kalau diperlukan
  reset_arr = 0;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // vSetupLittlefs();
  vSetupEEPROM();
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
    if(ModeScan){
      String message = "SCAN ALAT TUNNEL";
      kirimPesanSemua(message);
    }
  }

  // if(millis() - millisPengiriman > DuaDetik){
  //   millisPengiriman = millis();
  //   String message = "KIRIM SEMUA DATA";
  //   kirimPesanKeHeater(message);
  // }

  // --- DETIK KE-5 : Kirim Serial Data ---
  /////////////////////////////////// MODE 3X 0 MAKA RESET KE 0
  if (elapsed >= 5000) {
    if(!ModeScan){
      String output = "";
      for (int j = 0; j < 76; j++) {
        String val = data[j];
        val.trim();
        // if (useFilter(j)) {
        //   // ---- LOGIKA FILTER ----
        //   if (lastData[j].length() == 0 && val == "0") {
        //     lastData[j] = "0";
        //     zeroCount[j] = 0;
        //   } else if (val == "0") {
        //     if (zeroCount[j] >= 2) {
        //       val = "0";
        //       lastData[j] = "0";
        //       zeroCount[j] = 0;
        //     } else {
        //       val = lastData[j];
        //       zeroCount[j]++;
        //     }
        //   } else {
        //     lastData[j] = val;
        //     zeroCount[j] = 0;
        //   }
        // } else {
        //   lastData[j] = val;
        //   zeroCount[j] = 0;
        // }

        if (useFilter(j)) {
          // ---- LOGIKA FILTER ----
          if (lastData[j].length() == 0 && val == "0") {
            lastData[j] = "0";
            zeroCount[j] = 0;
          } else if (val == "0") {
              val = lastData[j];
              zeroCount[j]++;
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
        if (j < 75) output += ",";

        // debug
        Serial.print("data[");
        Serial.print(j);
        Serial.print("] = ");
        Serial.print(data[j]);
        Serial.print(" | kirim = ");
        Serial.println(val);
      }

      Serial.println("now>> " + output);
      
      Serial2.println(output);

      // reset data[] untuk siklus berikutnya
      for (int j = 0; j < 76; j++) {
        data[j] = "0";
      }
      Serial.println("RESET");

      // reset cycle
      cycleStartTime = millis();
      // reset flag untuk kirim esp-now di siklus berikutnya
      alreadySent = false;
    }
    else{
      String message = "SCAN ALAT TUNNEL";
      kirimPesanSemua(message);
    }
    millisPengiriman = millis();
  }

  /////////////////////////////////// MODE TANPA RESET 0 SAMA SEKALI
  


  // tetap jalankan fungsi rutin lain
  vPembacaanSerial();
  // vLoopESPnow();
}



