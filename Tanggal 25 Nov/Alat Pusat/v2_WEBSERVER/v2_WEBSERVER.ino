#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>

// Ganti dengan SSID dan Password WiFi Anda
const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";

Servo servo;
const int servoPin = 13;

AsyncWebServer server(80);

String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Kontrol Servo ESP32</title>
</head>
<body>
  <h2>Masukkan Sudut Servo (0–180)</h2>
  <form action="/servo">
    <input type="number" name="value" min="0" max="180" required>
    <input type="submit" value="Kirim">
  </form>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  servo.attach(servoPin);
  servo.write(0);

  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung ke WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Sajikan halaman web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", htmlPage);
  });

  // Endpoint kontrol servo
  server.on("/servo", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("value")) {
      String value = request->getParam("value")->value();
      int angle = value.toInt();
      angle = constrain(angle, 0, 180);
      servo.write(angle);
      Serial.print("Servo ke sudut: ");
      Serial.println(angle);
      request->send(200, "text/html", "<p>Servo diset ke sudut: " + String(angle) + "°</p><a href='/'>Kembali</a>");
    } else {
      request->send(400, "text/plain", "Parameter tidak valid");
    }
  });

  server.begin();
}

void loop() {
  // Tidak perlu isi karena menggunakan event-driven
}
