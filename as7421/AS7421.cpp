/*
  AS7421.cpp - Implementation for AMS AS7421 64-Channel Hyperspectral NIR Sensor
  
  Based on AS7421 Datasheet DS000667 v2-00
*/

#include "AS7421.h"

// Default SMUX configuration for sequential photodiode mapping
// Each 32-byte CFG_RAM represents 2 photodiodes as 4-bit nibbles
// Nibble values: 0=GND, 1-4=ADC 1-4
const uint8_t DEFAULT_SMUX_CONFIG[] = {
  0x21, 0x21, 0x21, 0x21, 0x43, 0x43, 0x43, 0x43  // Cycle A (16 channels)
};

AS7421::AS7421() : _address(AS7421_I2C_ADDR), _integrationTime(65500) {}

bool AS7421::begin(TwoWire &wire) {
  _wire = &wire;
  _wire->begin();
  
  // Check if device is present
  _wire->beginTransmission(_address);
  if (_wire->endTransmission() != 0) {
    return false;
  }
  
  // Initialize device (required after power-up)
  initDevice();
  delay(10);
  
  // Power on
  powerOn();
  delay(10);
  
  // Configure SMUX for 64-channel operation (using working Python library config)
  configureSMUX();
  
  // Set default configuration
  setMeasurementCycles(4);  // 4 cycles for all 64 channels
  setCLKMOD(4);  // 1 MHz clock
  setIntegrationTime(32750);  // ~32.75ms default
  
  // Configure auto-zero
  configureAutoZero(true, 2);  // Enable with 128µs wait
  
  // Enable LED wait and wait cycle
  uint8_t misc = readRegister(AS7421_CFG_MISC);
  writeRegister(AS7421_CFG_MISC, misc | 0x02);  // WAIT_CYCLE_ON
  
  return true;
}

void AS7421::initDevice() {
  // Required initialization sequence from datasheet
  writeRegister(AS7421_INIT_REG1, 0x44);
  writeRegister(AS7421_INIT_REG2, 0x20);
  writeRegister(AS7421_INIT_REG1, 0x00);
}

void AS7421::reset() {
  // Software reset
  uint8_t val = readRegister(AS7421_CFG_MISC);
  writeRegister(AS7421_CFG_MISC, val | 0x01);
  delay(50);
  initDevice();
}

uint8_t AS7421::getDeviceID() {
  return readRegister(AS7421_STATUS_0) & 0x3F;
}

uint8_t AS7421::getRevisionID() {
  return readRegister(AS7421_STATUS_1) & 0x07;
}

void AS7421::powerOn() {
  uint8_t val = readRegister(AS7421_ENABLE);
  writeRegister(AS7421_ENABLE, val | AS7421_PON);
  delay(5);
}

void AS7421::powerOff() {
  uint8_t val = readRegister(AS7421_ENABLE);
  writeRegister(AS7421_ENABLE, val & ~AS7421_PON);
}

bool AS7421::isPoweredOn() {
  return (readRegister(AS7421_ENABLE) & AS7421_PON) != 0;
}

void AS7421::configureSMUX() {
  // SMUX configuration from working Python library
  // Each byte contains two 4-bit nibbles representing 2 photodiodes
  // Nibble values: 0=GND, 1=ADC1, 2=ADC2, 3=ADC3, 4=ADC4
  // Default pattern: 0x21, 0x21, 0x21, 0x21, 0x43, 0x43, 0x43, 0x43
  
  uint8_t smuxPattern[8] = {0x21, 0x21, 0x21, 0x21, 0x43, 0x43, 0x43, 0x43};
  
  // Zero out all SMUX first
  uint8_t zeroSmux[32] = {0};
  writeRAM(0x0C, zeroSmux, 32);
  writeRAM(0x0D, zeroSmux, 32);
  writeRAM(0x0E, zeroSmux, 32);
  writeRAM(0x0F, zeroSmux, 32);
  
  // Configure SMUX for each cycle with offset
  // Cycle A: offset 0
  writeRAM(0x0C, smuxPattern, 8);
  
  // Cycle B: offset 8
  uint8_t tempBuf[32] = {0};
  for (int i = 0; i < 8; i++) {
    tempBuf[i + 8] = smuxPattern[i];
  }
  writeRAM(0x0D, tempBuf, 32);
  
  // Cycle C: offset 16
  memset(tempBuf, 0, 32);
  for (int i = 0; i < 8; i++) {
    tempBuf[i + 16] = smuxPattern[i];
  }
  writeRAM(0x0E, tempBuf, 32);
  
  // Cycle D: offset 24
  memset(tempBuf, 0, 32);
  for (int i = 0; i < 8; i++) {
    tempBuf[i + 24] = smuxPattern[i];
  }
  writeRAM(0x0F, tempBuf, 32);
  
  // Default gain configuration (gain = 6 = 64x, but changeable)
  uint8_t gainConfig[32];
  for (int i = 0; i < 32; i++) {
    gainConfig[i] = 0x88;  // Both nibbles = 8 (256x gain)
  }
  writeRAM(0x10, gainConfig, 32);  // ASETUP for cycles A/B
  writeRAM(0x11, gainConfig, 32);  // ASETUP for cycles C/D
}

void AS7421::configureSMUXCustom(const uint8_t *config, uint8_t length) {
  // Allow custom SMUX configuration
  writeRAM(0x0C, config, length);
}

void AS7421::setGainConfiguration(const uint8_t *gainConfig) {
  // Custom gain configuration for each ADC
  // gainConfig should be 32 bytes
  writeRAM(0x10, gainConfig, 32);  // ASETUP for cycles A/B
  writeRAM(0x11, gainConfig, 32);  // ASETUP for cycles C/D
}

void AS7421::setIntegrationTime(uint32_t timeUs) {
  // Integration time in microseconds
  // t_int = (LTF_ITIME + 1) / f_CLKMOD
  // For 1MHz clock: timeUs = (LTF_ITIME + 1)
  uint32_t ltfItime = timeUs - 1;
  _integrationTime = timeUs;
  write24(AS7421_LTF_ITIME_L, ltfItime);
}

uint32_t AS7421::getIntegrationTime() {
  return _integrationTime;
}

void AS7421::setWaitTime(uint32_t timeUs) {
  uint32_t ltfWtime = timeUs - 1;
  write24(AS7421_LTF_WTIME_L, ltfWtime);
}

void AS7421::enableMeasurement(bool enable) {
  uint8_t val = readRegister(AS7421_ENABLE);
  if (enable) {
    writeRegister(AS7421_ENABLE, val | AS7421_LTF_EN);
  } else {
    writeRegister(AS7421_ENABLE, val & ~AS7421_LTF_EN);
  }
}

bool AS7421::isMeasurementEnabled() {
  return (readRegister(AS7421_ENABLE) & AS7421_LTF_EN) != 0;
}

void AS7421::startSingleMeasurement() {
  writeRegister(AS7421_LTF_ICOUNT, 0x01);
  enableMeasurement(true);
}

void AS7421::startContinuousMeasurement(uint8_t count) {
  writeRegister(AS7421_LTF_ICOUNT, count);
  enableMeasurement(true);
}

bool AS7421::isDataReady() {
  return (readRegister(AS7421_STATUS_7) & AS7421_ADATA) != 0;
}

bool AS7421::isMeasurementBusy() {
  return (readRegister(AS7421_STATUS_6) & 0x10) != 0;
}

uint16_t AS7421::readChannel(uint8_t channel) {
  if (channel >= AS7421_NUM_CHANNELS) return 0;
  
  uint8_t regL = AS7421_CH0_DATA_L + (channel * 2);
  return read16(regL);
}

void AS7421::readAllChannels(uint16_t *data) {
  // Read all 64 channels in burst mode
  uint8_t buffer[128];
  readRegisters(AS7421_CH0_DATA_L, buffer, 128);
  
  for (int i = 0; i < AS7421_NUM_CHANNELS; i++) {
    data[i] = buffer[i*2] | (buffer[i*2 + 1] << 8);
  }
}

bool AS7421::readSpectralData(uint16_t *data) {
  // Read STATUS_7 first (required before reading data)
  clearStatus();
  
  if (!isDataReady()) {
    return false;
  }
  
  readAllChannels(data);
  return true;
}

int16_t AS7421::readTemperature(uint8_t cycle) {
  if (cycle > 3) cycle = 0;
  
  uint8_t regL = AS7421_TEMP0_L + (cycle * 2);
  return (int16_t)read16(regL);
}

float AS7421::getTemperatureCelsius(uint8_t cycle) {
  int16_t raw = readTemperature(cycle);
  // Temperature conversion (typical for ams sensors)
  return raw / 100.0;
}

void AS7421::configureLED(uint8_t offset, uint8_t mult, uint8_t current) {
  // Set LED offset (which integration cycle)
  uint8_t ledCfg = readRegister(AS7421_CFG_LED);
  ledCfg = (ledCfg & 0xE7) | ((offset & 0x03) << 3);
  ledCfg = (ledCfg & 0xF8) | (current & 0x07);
  writeRegister(AS7421_CFG_LED, ledCfg);
  
  // Set LED multiplexer
  writeRegister(AS7421_CFG_LED_MULT, mult & 0x1F);
}

void AS7421::enableLED(bool enable) {
  uint8_t val = readRegister(AS7421_CFG_LED);
  if (enable) {
    writeRegister(AS7421_CFG_LED, val | 0x80);
  } else {
    writeRegister(AS7421_CFG_LED, val & ~0x80);
  }
}

void AS7421::setLEDCurrent(uint8_t current) {
  uint8_t val = readRegister(AS7421_CFG_LED);
  val = (val & 0xF8) | (current & 0x07);
  writeRegister(AS7421_CFG_LED, val);
}

void AS7421::setLEDMultiplexer(uint8_t mult) {
  writeRegister(AS7421_CFG_LED_MULT, mult & 0x1F);
}

void AS7421::setLEDWaitTime(uint8_t waitTime) {
  writeRegister(AS7421_LED_WAIT, waitTime);
}

void AS7421::configureLEDForAllCycles() {
  // Configure LEDs to be on for all 4 integration cycles
  // Cycle A
  configureLED(0, 0x1F, AS7421_LED_CURRENT_50MA);
  // Cycle B
  configureLED(1, 0x1F, AS7421_LED_CURRENT_50MA);
  // Cycle C
  configureLED(2, 0x1F, AS7421_LED_CURRENT_50MA);
  // Cycle D
  configureLED(3, 0x1F, AS7421_LED_CURRENT_50MA);
}

void AS7421::setLEDAutoMode(uint8_t mode) {
  uint8_t val = readRegister(AS7421_ENABLE);
  val = (val & 0xCF) | ((mode & 0x03) << 4);
  writeRegister(AS7421_ENABLE, val);
}

uint8_t AS7421::getStatus() {
  return readRegister(AS7421_STATUS_7);
}

void AS7421::clearStatus() {
  readRegister(AS7421_STATUS_7);
}

bool AS7421::isAnalogSaturated() {
  return (readRegister(AS7421_STATUS_7) & AS7421_ASAT) != 0;
}

bool AS7421::isDigitalSaturated() {
  return (readRegister(AS7421_STATUS_7) & AS7421_DSAT) != 0;
}

bool AS7421::isDataLost() {
  return (readRegister(AS7421_STATUS_7) & AS7421_DLOST) != 0;
}

void AS7421::enableInterrupt(uint8_t mask) {
  writeRegister(AS7421_INT_ENABLE, mask);
}

void AS7421::configureAutoZero(bool enable, uint8_t waitTime) {
  uint8_t val = (enable ? 0x10 : 0x00) | ((waitTime & 0x03) << 5);
  writeRegister(AS7421_CFG_AZ, val);
}

void AS7421::performAutoZero() {
  writeRegister(AS7421_CFG_AZ, 0x80);
}

void AS7421::configureGPIO(bool output, bool invert) {
  uint8_t val = 0;
  if (output) val |= 0x04;
  if (invert) val |= 0x08;
  writeRegister(AS7421_CFG_GPIO, val);
}

void AS7421::setGPIO(bool high) {
  uint8_t val = readRegister(AS7421_CFG_GPIO);
  if (high) {
    writeRegister(AS7421_CFG_GPIO, val | 0x02);
  } else {
    writeRegister(AS7421_CFG_GPIO, val & ~0x02);
  }
}

bool AS7421::readGPIO() {
  return (readRegister(AS7421_CFG_GPIO) & 0x01) != 0;
}

void AS7421::enableSync(bool enable) {
  uint8_t val = readRegister(AS7421_ENABLE);
  if (enable) {
    writeRegister(AS7421_ENABLE, val | AS7421_SYNC_EN);
  } else {
    writeRegister(AS7421_ENABLE, val & ~AS7421_SYNC_EN);
  }
}

uint16_t AS7421::getWavelength(uint8_t channel) {
  return mapPhotodiodetToWavelength(channel);
}

void AS7421::getWavelengthArray(uint16_t *wavelengths) {
  for (int i = 0; i < AS7421_NUM_CHANNELS; i++) {
    wavelengths[i] = mapPhotodiodetToWavelength(i);
  }
}

void AS7421::setMeasurementCycles(uint8_t cycles) {
  uint8_t val = readRegister(AS7421_CFG_LTF);
  val = (val & 0xE7) | (((cycles - 1) & 0x03) << 3);
  writeRegister(AS7421_CFG_LTF, val);
}

void AS7421::setCLKMOD(uint8_t mode) {
  uint8_t val = readRegister(AS7421_CFG_LTF);
  val = (val & 0xF8) | (mode & 0x07);
  writeRegister(AS7421_CFG_LTF, val);
}

// Private I2C Methods
void AS7421::writeRegister(uint8_t reg, uint8_t value) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(value);
  _wire->endTransmission();
}

uint8_t AS7421::readRegister(uint8_t reg) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->endTransmission(false);
  
  _wire->requestFrom(_address, (uint8_t)1);
  return _wire->read();
}

void AS7421::readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->endTransmission(false);
  
  _wire->requestFrom(_address, len);
  for (uint8_t i = 0; i < len; i++) {
    buffer[i] = _wire->read();
  }
}

uint16_t AS7421::read16(uint8_t reg) {
  uint8_t buffer[2];
  readRegisters(reg, buffer, 2);
  return buffer[0] | (buffer[1] << 8);
}

void AS7421::write24(uint8_t reg, uint32_t value) {
  writeRegister(reg, value & 0xFF);
  writeRegister(reg + 1, (value >> 8) & 0xFF);
  writeRegister(reg + 2, (value >> 16) & 0xFF);
}

uint32_t AS7421::read24(uint8_t reg) {
  uint8_t buffer[3];
  readRegisters(reg, buffer, 3);
  return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16);
}

void AS7421::writeRAM(uint8_t offset, const uint8_t *data, uint8_t len) {
  // Set RAM offset
  writeRegister(AS7421_CFG_RAM, offset & 0x1F);
  
  // Write data to RAM
  for (uint8_t i = 0; i < len; i++) {
    writeRegister(AS7421_CFG_RAM_0 + i, data[i]);
  }
}

void AS7421::readRAM(uint8_t offset, uint8_t *data, uint8_t len) {
  // Set RAM offset
  writeRegister(AS7421_CFG_RAM, offset & 0x1F);
  
  // Read data from RAM
  readRegisters(AS7421_CFG_RAM_0, data, len);
}

uint16_t AS7421::mapPhotodiodetToWavelength(uint8_t pd) {
  // Photodiode wavelength mapping from datasheet Figure 14
  // 8x8 array with specific wavelength per photodiode
  const uint16_t wavelengthMap[64] = {
    /*
    // Row 0
    830, 750, 790, 870, 940, 980, 1020, 830,
    // Row 1
    760, 800, 840, 880, 930, 970, 1010, 1050,
    // Row 2
    770, 810, 850, 890, 920, 960, 1000, 1040,
    // Row 3
    780, 820, 860, 900, 910, 950, 990, 1030,
    // Row 4
    775, 815, 855, 895, 905, 945, 985, 1025,
    // Row 5
    765, 805, 845, 885, 915, 955, 995, 1035,
    // Row 6
    755, 795, 835, 875, 925, 965, 1005, 1045,
    // Row 7
    830, 785, 825, 865, 935, 975, 1015, 830*/
    830, 750, 760, 800, 790, 870, 840, 
    880, 940, 980, 930, 970, 1020, 831, 
    1010, 1025, 770, 810, 780, 820, 850, 
    890, 860, 900, 920, 960, 910, 950, 
    1000, 1030, 990, 1035, 775, 815, 765, 
    805, 855, 895, 845, 885, 905, 945, 
    915, 955, 985, 1040, 995, 1045, 755, 
    795, 830, 785, 835, 875, 825, 865, 
    925, 965, 935, 975, 1005, 1050, 1015, 832
  };
  
  if (pd < 64) {
    return wavelengthMap[pd];
  }
  return 0;
}