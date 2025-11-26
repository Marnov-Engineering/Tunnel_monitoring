#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_MAX31865.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "max6675.h"
#include <ESP32Servo.h>

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

#define ONE_WIRE_BUS 33
#define SEALEVELPRESSURE_HPA (1013.25)


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensorsDS(&oneWire); // sensor ds 
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ledcSetup(0, 1, 8); // channel, frekuensi, resolution
  ledcAttachPin(SSRPin, 0);
  

  sensorsDS.begin();
  thermo.begin(MAX31865_3WIRE);
  pinMode(SSRPin, OUTPUT);
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

void vSuhuDS(){
  // Serial.println("Suhu DS ==============================");
  // Serial.print("Requesting temperatures...");
  sensorsDS.requestTemperatures(); // Send the command to get temperatures
  // Serial.println("DONE");
  // delay(1500);
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = sensorsDS.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Temperature DS = ");
    Serial.println(tempC);
  }
  else
  {
    Serial.println("Error: Could not read temperature data");
  }
  // Serial.println();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() - now1S > millis1S){
    // vSuhuDS();
    // vSuhuRTD();
    vSuhuBME280();
    Serial.println("===================================================");
    // vSuhuThermocouple();
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
