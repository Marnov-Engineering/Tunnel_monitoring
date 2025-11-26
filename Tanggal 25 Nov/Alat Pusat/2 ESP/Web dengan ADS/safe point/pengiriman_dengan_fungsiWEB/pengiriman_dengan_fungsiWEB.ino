/*
  ESP32 Async WebServer (WiFi STA) with HTTPClient POST helper

  - Connects ESP32 to an existing WiFi network (Station mode)
  - Serves a simple HTML page at '/'
  - Provides reusable 'postData' function using HTTPClient
  - Periodically performs a POST in loop()

  Required libraries (install via Library Manager if missing):
    - ESP Async WebServer by me-no-dev
    - AsyncTCP by me-no-dev (ESP32)
*/

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <LittleFS.h>

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

float out_pid1 = random(0, 10000) / 100.0;
float out_pid2 = random(0, 10000) / 100.0;
float out_pid3 = random(0, 10000) / 100.0;

int Heater_st_1 = random(0, 2);
int Heater_st_2 = random(0, 2);
int Heater_st_3 = random(0, 2);

int Fan_Power_1 = random(0, 101);
int Fan_Power_2 = random(0, 101);
int Fan_Power_3 = random(0, 101);

float setpoint1 = random(2000, 8000) / 100.0;
float setpoint2 = random(2000, 8000) / 100.0;
float setpoint3 = random(2000, 8000) / 100.0;

int WCSstate1 = random(0, 2);
int WCSstate2 = random(0, 2);
int WCSstate3 = random(0, 2);

String SData;


// ---------- WiFi Credentials ----------
// Replace with your WiFi network SSID and password
const char* WIFI_SSID = "Web Tunnel Monitoring";
const char* WIFI_PASSWORD = "12345678";

// ---------- Server ----------
AsyncWebServer webServer(80);

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

// ---------- Periodic POST in loop ----------
// Adjust interval and target as needed
const uint32_t POST_INTERVAL_MS = 30000; // 60 seconds
unsigned long lastPostMs = 0;
unsigned long waktuUpdate = 1760618825;

void runPeriodicPost() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  const unsigned long nowMs = millis();
  if (nowMs - lastPostMs < POST_INTERVAL_MS) {
    return;
  }
  lastPostMs = nowMs;
  Serial.println("Mulai pengiriman");

  const char* url = "http://ptsv3.com/gas-monitor"; // Change to your server URL
  const char* contentType = "application/json";
  const char* payload = "{\"hello\":\"from-esp32\"}";

  static char responseBody[512];
  int httpCode = 0;
  bool ok = postData(url, payload, contentType, responseBody, sizeof(responseBody), httpCode);
  Serial.printf("POST %s -> status=%d, ok=%s\n", url, httpCode, ok ? "true" : "false");
  Serial.println("Selesai pengiriman");
}

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

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("Booting...");

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

  // Routes
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed");
  } else {
    Serial.println("LittleFS mounted");
  }

  // Serve static files from LittleFS root; default file is index.html
  webServer.serveStatic("/", LittleFS, "/").setDefaultFile("user.html");
  webServer.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
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
      json += "\"time\":" + String(waktuUpdate + 25200) + "000" + ",";

      json += "\"suhu1\":" + String(random(2000, 8000) / 100.0) + ",";
      json += "\"suhu2\":" + String(random(2000, 8000) / 100.0) + ",";
      json += "\"suhu3\":" + String(random(2000, 8000) / 100.0) + ",";
      json += "\"suhu4\":" + String(random(2000, 8000) / 100.0) + ",";
      json += "\"suhu5\":" + String(random(2000, 8000) / 100.0) + ",";
      json += "\"suhu6\":" + String(random(2000, 8000) / 100.0) + ",";
      json += "\"suhu7\":" + String(random(2000, 8000) / 100.0) + ",";

      json += "\"Dsuhu1\":" + String(random(100, 500) / 10.0) + ",";
      json += "\"Dsuhu2\":" + String(random(100, 500) / 10.0) + ",";
      json += "\"Dsuhu3\":" + String(random(100, 500) / 10.0) + ",";

      json += "\"Psuhu1\":" + String(random(900, 1100) / 10.0) + ",";
      json += "\"Psuhu2\":" + String(random(900, 1100) / 10.0) + ",";
      json += "\"Psuhu3\":" + String(random(900, 1100) / 10.0) + ",";

      json += "\"Humid1\":" + String(random(3000, 9000) / 100.0) + ",";
      json += "\"Humid2\":" + String(random(3000, 9000) / 100.0) + ",";
      json += "\"Humid3\":" + String(random(3000, 9000) / 100.0) + ",";
      json += "\"Humid4\":" + String(random(3000, 9000) / 100.0) + ",";
      json += "\"Humid5\":" + String(random(3000, 9000) / 100.0) + ",";
      json += "\"Humid6\":" + String(random(3000, 9000) / 100.0) + ",";
      json += "\"Humid7\":" + String(random(3000, 9000) / 100.0) + ",";
      json += "\"UHumid1\":" + String(random(3000, 9000) / 100.0) + ",";
      json += "\"UHumid2\":" + String(random(3000, 9000) / 100.0) + ",";

      json += "\"kec1\":" + String(random(0, 2000) / 100.0) + ",";
      json += "\"kec2\":" + String(random(0, 2000) / 100.0) + ",";
      json += "\"kec3\":" + String(random(0, 2000) / 100.0) + ",";
      json += "\"kec4\":" + String(random(0, 2000) / 100.0) + ",";
      json += "\"kec5\":" + String(random(0, 2000) / 100.0) + ",";
      json += "\"kec6\":" + String(random(0, 2000) / 100.0) + ",";
      json += "\"kec7\":" + String(random(0, 2000) / 100.0) + ",";

      json += "\"Usuhu1\":" + String(random(2000, 8000) / 100.0) + ",";
      json += "\"Usuhu2\":" + String(random(2000, 8000) / 100.0) + ",";
      json += "\"Ukec1\":" + String(random(0, 2000) / 100.0) + ",";
      json += "\"Ukec2\":" + String(random(0, 2000) / 100.0) + ",";
      json += "\"USDP1\":" + String(random(100, 1000) / 10.0) + ",";
      json += "\"USDP2\":" + String(random(100, 1000) / 10.0);

      json += "}";
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

void loop() {
  runPeriodicPost();
}


