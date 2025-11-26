#include <ESP32Servo.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;
static const int servoPin = 25;
Servo servo1;
int posDegrees = 0;

String inputString = "";  // Untuk menyimpan input dari Serial
bool inputComplete = false;

unsigned long ADSMillis;
unsigned long IntervalADSMillis = 1000;

#define MAX_SIGNAL 2000
#define MIN_SIGNAL 1000  // 700 ?

float arus;

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
  // Arus1 = adc0;
  // Arus2 = adc1;
  // Arus3 = adc2;
  // Arus4 = adc3;
  // arus = adc3 * 0.004351 - 85.004;
  // Serial.print("arus = ");
  // Serial.println(arus);
}

void keywait()
{
  Serial.println(" So press any key.");

  while (!Serial.available())
    ;
  Serial.read();
}

void setup() {
  // put your setup code here, to run once:

  vSetupADS();
  ads.setGain(GAIN_ONE);
  Serial.begin(115200);
  
  servo1.attach(servoPin,700,2000);
  // servo1.attach(servoPin);
  servo1.write(0);

  Serial.println("Start (again) the calibrate dance for ESC.");


  Serial.println("Turn OFF power source."); keywait();

  Serial.println("Now writing maximum output.");
  servo1.writeMicroseconds(2000);
  Serial.println("Turn ON power source."); keywait();
  
  Serial.println("Now to write minimum output"); keywait();
  servo1.writeMicroseconds(1000);

  Serial.println("Masukkan nilai derajat antara 0 hingga 180:");

}

void loop() {
  // Baca input dari Serial
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      inputComplete = true;
    } else {
      inputString += inChar;
    }
  }

  if(millis() - ADSMillis >= IntervalADSMillis){
    vPembacaanADS();
    ADSMillis = millis();
  }

  // Proses input jika lengkap
  if (inputComplete) {
    posDegrees = inputString.toInt(); // Konversi ke integer
    // posDegrees = posDegrees + pertambahan;

    if (posDegrees >= 0 && posDegrees <= 180) {
      servo1.write(posDegrees);
      Serial.print("Servo diputar ke: ");
      Serial.println(posDegrees);
    } else {
      Serial.println("Masukkan angka antara 0 hingga 180.");
    }
    inputString = "";
    inputComplete = false;
  }
}
