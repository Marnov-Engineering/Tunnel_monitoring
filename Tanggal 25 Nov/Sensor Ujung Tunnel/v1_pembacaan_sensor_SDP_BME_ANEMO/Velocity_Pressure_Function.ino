#include <Arduino.h>
#include <stdint.h>

int8_t i2c_read(uint8_t addr, uint8_t* data, uint16_t count, int Port) {
  if (Port == 1){
    Wire.requestFrom(addr, count);
    if (Wire.available() != count) {
        return -1;
    }
    for (int i = 0; i < count; ++i) {
        data[i] = Wire.read();
    }
    return 0;
  }
  else if (Port == 2){
    Wire1.requestFrom(addr, count);
    if (Wire1.available() != count) {
        return -1;
    }
    for (int i = 0; i < count; ++i) {
        data[i] = Wire1.read();
    }
    return 0;
  }
}

int8_t i2c_write(uint8_t addr, const uint8_t* data, uint16_t count, bool appendCrc, int Port){
  if (Port == 1){
    Wire.beginTransmission(addr);
    for (int i = 0; i < count; ++i) {
      if (Wire.write(data[i]) != 1) {
          return 1;
      }
    }
    if (appendCrc) {
      uint8_t crc = crc8(data, count);
      if (Wire.write(crc) != 1) {
          return 2;
      }
    }
    if (Wire.endTransmission() != 0) {
        return 3;
    }
    return 0;
  }
  else if (Port == 2) {
    Wire1.beginTransmission(addr);
    for (int i = 0; i < count; ++i) {
      if (Wire1.write(data[i]) != 1) {
          return 1;
      }
    }
    if (appendCrc) {
      uint8_t crc = crc8(data, count);
      if (Wire1.write(crc) != 1) {
          return 2;
      }
    }
    if (Wire1.endTransmission() != 0) {
        return 3;
    }
    return 0;
  }
}

uint8_t crc8(const uint8_t* data, uint8_t len) {
  uint8_t crc = 0xff;
  uint8_t byteCtr;
  for (byteCtr = 0; byteCtr < len; ++byteCtr) {
    crc ^= (data[byteCtr]);
    for (uint8_t bit = 8; bit > 0; --bit) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x31;
      } else {
        crc = (crc << 1);
      }
    }
  }
  return crc;
}
int iInitSensorSPD810(int Port){
  // try to read product id
  const uint8_t CMD_LEN = 2;
  uint8_t cmd0[CMD_LEN] = { 0x36, 0x7C };
  uint8_t cmd1[CMD_LEN] = { 0xE1, 0x02 };
  const uint8_t DATA_LEN = 18;
  uint8_t data[DATA_LEN] = { 0 };
  uint8_t ret = i2c_write(0x25, cmd0, CMD_LEN, false, Port);
  if (ret != 0) {
    return 1;
  }
  ret = i2c_write(0x25, cmd1, CMD_LEN, false, Port);
  if (ret != 0) {
    return 2;
  }
  ret = i2c_read(0x25, data, DATA_LEN, Port);
  if (ret != 0) {
    return 3;
  }
  // at this point, we don't really care about the data just yet, but
  // we may use that in the future. Either way, the sensor responds, and
  return 0;
}
int readSample(int Port){
  const uint8_t CMD_LEN = 2;
  uint8_t cmd[CMD_LEN] = { 0x36, 0x2F };
  const uint8_t DATA_LEN = 9;
  uint8_t data[DATA_LEN] = { 0 };
  if (i2c_write(0x25, cmd, CMD_LEN, false, Port) != 0) {
    return 1;
  }
  delay(100); // theoretically 45ms
  if (i2c_read(0x25, data, DATA_LEN, Port) != 0) {
    return 1;
  }
  // TODO: check CRC
  int16_t dp_raw   = (int16_t)data[0] << 8 | data[1];
  int16_t temp_raw = (int16_t)data[3] << 8 | data[4];
  int8_t dp_scale  = (int16_t)data[7] << 8 | data[7];
  if (Port == 1){
    mDifferentialPressure = dp_raw / (float)dp_scale;
    mTemperature = temp_raw / 200.0;
  }
  else if (Port == 2){
    mDifferentialPressure2 = dp_raw / (float)dp_scale;
    mTemperature2 = temp_raw / 200.0;
  }
  return 0;
}