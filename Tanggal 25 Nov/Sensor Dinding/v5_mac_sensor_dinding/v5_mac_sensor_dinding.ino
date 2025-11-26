// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#define WIFI_MODE_STA WIFI_STA
#include <QuickEspNow.h>

// MAC ESP masing-masing sensor
static uint8_t sensorDinding3[] = {0x2C, 0x3A, 0xE8, 0x14, 0x85, 0x8F}; // Kirim ke target1
// static uint8_t sensorDinding1[] = {0x58, 0xBF, 0x25, 0xC2, 0xFE, 0x9F}; // Kirim ke target2
static uint8_t sensorDinding1[] = { 0x4C, 0x11, 0xAE, 0x03, 0x64, 0xF1 };
static uint8_t sensorDinding2[] = {0xEC, 0xFA, 0xBC, 0x41, 0x6E, 0xDF}; // Kirim ke target3

// MAC target masing-masing
static uint8_t target2[] = {0x1C, 0x69, 0x20, 0x96, 0x77, 0x60};
static uint8_t target1[] = {0xEC, 0xE3, 0x34, 0xD7, 0x70, 0x60};
static uint8_t target3[] = {0x1C, 0x69, 0x20, 0x96, 0xEC, 0xC8};

// Pointer penerima yang dipilih sesuai MAC ESP
uint8_t *receiver = nullptr;

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer;

// static uint8_t receiver[] = {0x1C, 0x69, 0x20, 0x96, 0x31, 0xD4};
// 1c:69:20:96:77:60
// static uint8_t receiver[] = {0x1C, 0x69, 0x20, 0x96, 0x77, 0x60};

bool sent = true;
const unsigned int SEND_MSG_MSEC = 2000;
#define INTERVAL 1000 // in milliseconds
unsigned long lastReadTime = 0;
String message;

bool isMacMatch(const uint8_t *macArray) {
  for (int i = 0; i < 6; i++) {
    if (macArray[i] != WiFi.macAddress()[i]) {
      return false;
    }
  }
  return true;
}

void dataSent(uint8_t *address, uint8_t status)
{    sent = true;
    Serial.printf("Message sent to " MACSTR ", status: %d\n", MAC2STR(address), status);
}

void dataReceived(uint8_t *address, uint8_t *data, uint8_t len, signed int rssi, bool broadcast)
{
    Serial.print("Received: ");
    Serial.printf("%.*s\n", len, data);
    Serial.printf("RSSI: %d dBm\n", rssi);
    Serial.printf("From: " MACSTR "\n", MAC2STR(address));
    Serial.printf("%s\n", broadcast ? "Broadcast" : "Unicast");

    // ledState = !ledState;
    // digitalWrite(indicatorLed, ledState ? HIGH : LOW);
    float tempC = sensors.getTempC(insideThermometer);
    if (tempC == DEVICE_DISCONNECTED_C)
    {
      Serial.println("Error: Could not read temperature data");
      return;
    }
    Serial.print("Temp C: ");
    Serial.print(tempC);
    String message = String(tempC);
    // sent = false;
    // =================================================================================>>>>>>>>>>>>>>>>>>     pembacaan suhu disini
    if (!quickEspNow.send(receiver, (uint8_t *)message.c_str(), message.length()))
    {
        Serial.printf(">>>>>>>>>> Message sent hasil dari feedback\n");
    }
    else
    {
        Serial.printf(">>>>>>>>>> Message not sent hasil dari feedback\n");
        sent = true; // In case of error we need to set the flag to true to avoid blocking the loop
    }
}

void vKirimDataSuhu(){
  float tempC = sensors.getTempC(insideThermometer);
    if (tempC == DEVICE_DISCONNECTED_C)
    {
      Serial.println("Error: Could not read temperature data");
      return;
    }
    Serial.print("Temp C: ");
    Serial.print(tempC);
    String message = String(tempC);
    // sent = false;
    // =================================================================================>>>>>>>>>>>>>>>>>>     pembacaan suhu disini
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


 
/*
 * Setup function. Here we do the basics
 */
void setup(void)
{
  // start serial port
  Serial.begin(115200);
  Serial.println("Dallas Temperature IC Control Library Demo");

  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect(false);

  uint8_t mac[6];
  WiFi.macAddress(mac);

  Serial.printf("MAC ESP ini: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  Serial.printf("Connected to %s in channel %d\n", WiFi.SSID().c_str(), WiFi.channel());
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());

  // Pencocokan MAC
  if (memcmp(mac, sensorDinding1, 6) == 0) {
    receiver = target1;
    Serial.println("ESP ini = sensorDinding1 → kirim ke target1");
  }
  else if (memcmp(mac, sensorDinding2, 6) == 0) {
    receiver = target2;
    Serial.println("ESP ini = sensorDinding2 → kirim ke target2");
  }
  else if (memcmp(mac, sensorDinding3, 6) == 0) {
    receiver = target3;
    Serial.println("ESP ini = sensorDinding3 → kirim ke target3");
  } else {
    Serial.println("MAC tidak terdaftar → tidak kirim");
  }


  quickEspNow.begin(10, 0, false);
  quickEspNow.onDataSent(dataSent);
  quickEspNow.onDataRcvd(dataReceived);


  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // Assign address manually. The addresses below will need to be changed
  // to valid device addresses on your bus. Device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  // Note that you will need to use your specific address here
  //insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };

  // Method 1:
  // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then
  // use those addresses and manually assign them (see above) once you know
  // the devices on your bus (and assuming they don't change).
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");

  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices,
  // or you have already retrieved all of them. It might be a good idea to
  // check the CRC to make sure you didn't get garbage. The order is
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC);
  Serial.println();
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  // method 1 - slower
  //Serial.print("Temp C: ");
  //Serial.print(sensors.getTempC(deviceAddress));
  //Serial.print(" Temp F: ");
  //Serial.print(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
  Serial.print(" //////////////////////////////////////////////////////////////////////////");
}
/*
 * Main function. It will request the tempC from the sensors and display on Serial.
 */
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  // Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // Serial.printf("Connected to %s in channel %d\n", WiFi.SSID().c_str(), WiFi.channel());
  // Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
  // Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
  delay(1000);
  // vKirimDataSuhu();

  // It responds almost immediately. Let's print out the data
  printTemperature(insideThermometer); // Use a simple function to print out the data
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
