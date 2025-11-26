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
  webServer.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  
  // Start server
  webServer.begin();
  Serial.println("Async WebServer started.");
}

void loop() {
  runPeriodicPost();
}


