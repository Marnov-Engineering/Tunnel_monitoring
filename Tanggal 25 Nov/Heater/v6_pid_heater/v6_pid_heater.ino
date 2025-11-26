#include <Arduino.h>
/*************************************************************
   PID Relay Output Example
   Same as basic example, except that this time, the output
   is going to a digital pin which (we presume) is controlling
   a relay.  The pid is designed to Output an analog value,
   but the relay can only be On/Off.

   To connect them together we use "time proportioning
   control", essentially a really slow version of PWM.
   First we decide on a window size (5000mS for example).
   We then set the pid to adjust its output between 0 and that
   window size. Lastly, we add some logic that translates the
   PID output into "Relay On Time" with the remainder of the
   window being "Relay Off Time". The minWindow setting is a
   floor (minimum time) the relay would be on.
 *************************************************************/

#include "QuickPID.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

#define PIN_INPUT 0
#define SSRPin 25

Adafruit_BME280 bme; // I2C

//Define Variables we'll be connecting to
float Setpoint, Input, Output;

unsigned long millis1S = 1000;
unsigned long now1S = 0;

//Specify the links and initial tuning parameters
float Kp = 2, Ki = 5, Kd = 1;
float POn = 1.0;   // proportional on Error to Measurement ratio (0.0-1.0), default = 1.0
float DOn = 0.0;   // derivative on Error to Measurement ratio (0.0-1.0), default = 0.0

QuickPID myQuickPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, POn, DOn, QuickPID::DIRECT);

unsigned int WindowSize = 1000;
unsigned int minWindow = 10;
unsigned long windowStartTime;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("BME280 test"));

  bool status;

  status = bme.begin(0x76);

  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  pinMode(SSRPin, OUTPUT);
  windowStartTime = millis();

  //initialize the variables we're linked to
  Setpoint = 40;

  //tell the PID to range between 0 and the full window size
  myQuickPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  myQuickPID.SetMode(QuickPID::AUTOMATIC);
}

void printValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}

void loop()
{
  Input = bme.readTemperature();
  if(millis() - now1S > millis1S){
    printValues();
    Serial.println("===================================================");
    now1S = millis();
  }

  /************************************************
     turn the output pin on/off based on pid output
   ************************************************/
  if (millis() - windowStartTime >= WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize; 
    myQuickPID.Compute();
  }
  if (((unsigned int)Output > minWindow) && ((unsigned int)Output < (millis() - windowStartTime))) digitalWrite(SSRPin, HIGH);
  else digitalWrite(SSRPin, LOW);
}

