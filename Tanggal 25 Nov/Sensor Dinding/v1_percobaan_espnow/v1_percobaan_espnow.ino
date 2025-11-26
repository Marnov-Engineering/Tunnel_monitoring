// my addr -> EC:FA:BC:D5:51:A7

// #define ESP8266
#include <Arduino.h>
#if defined ESP32
#include <WiFi.h>
#include <esp_wifi.h>
#elif defined ESP8266
#include <ESP8266WiFi.h>
#define WIFI_MODE_STA WIFI_STA
#else
#error "Unsupported platform"
#endif // ESP32
#include <QuickEspNow.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */



static uint8_t receiver[] = {0x1C, 0x69, 0x20, 0x96, 0x31, 0xD4};
bool sent = true;
const unsigned int SEND_MSG_MSEC = 2000;
#define INTERVAL 1000 // in milliseconds
unsigned long lastReadTime = 0;

bool ledState = false;
int indicatorLed = 2;

float readMetana(int numberOfSampling)
{
    unsigned long ADCRead = 0;
    for (int i = 0; i < numberOfSampling; i++)
    {
        ADCRead += ads.readADC_SingleEnded(0);
        delay(1);
    }
    return (ADCRead / numberOfSampling);
}

void dataSent(uint8_t *address, uint8_t status)
{
    sent = true;
    Serial.printf("Message sent to " MACSTR ", status: %d\n", MAC2STR(address), status);
}

void dataReceived(uint8_t *address, uint8_t *data, uint8_t len, signed int rssi, bool broadcast)
{
    Serial.print("Received: ");
    Serial.printf("%.*s\n", len, data);
    Serial.printf("RSSI: %d dBm\n", rssi);
    Serial.printf("From: " MACSTR "\n", MAC2STR(address));
    Serial.printf("%s\n", broadcast ? "Broadcast" : "Unicast");

    ledState = !ledState;
    digitalWrite(indicatorLed, ledState ? HIGH : LOW);

    String message = String(readMetana(10));
    // sent = false;
    if (!quickEspNow.send(receiver, (uint8_t *)message.c_str(), message.length()))
    {
        Serial.printf(">>>>>>>>>> Message sent\n");
    }
    else
    {
        Serial.printf(">>>>>>>>>> Message not sent\n");
        sent = true; // In case of error we need to set the flag to true to avoid blocking the loop
    }
}

void setup()
{
    Serial.begin(115200);    
    pinMode(indicatorLed, OUTPUT);

    WiFi.mode(WIFI_MODE_STA);
#if defined ESP32
    WiFi.disconnect(false, true);
#elif defined ESP8266
    WiFi.disconnect(false);
#endif // ESP32

    Serial.printf("Connected to %s in channel %d\n", WiFi.SSID().c_str(), WiFi.channel());
    Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
    quickEspNow.begin(10, 0, false);
    quickEspNow.onDataSent(dataSent);
    quickEspNow.onDataRcvd(dataReceived);

    delay(500);
    if (!ads.begin()) {
        Serial.println("Failed to initialize ADS.");
    while (1);
    
    }
    ads.setGain(GAIN_ONE);
}

void loop()
{
    unsigned long currentMillis = millis();


    if (currentMillis - lastReadTime >= INTERVAL)
    {
        lastReadTime = currentMillis;
        
        // float metana = readMetana(10);
        // Serial.println(raw);
    }
}
