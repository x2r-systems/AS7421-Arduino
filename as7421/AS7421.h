/*
  AS7421.h - Library for AMS AS7421 64-Channel Hyperspectral NIR Sensor
  
  Based on AS7421 Datasheet DS000667 v2-00
  Includes proper SMUX configuration and register mapping
*/

#ifndef AS7421_H
#define AS7421_H

#include <Arduino.h>
#include <Wire.h>

// I2C Address
#define AS7421_I2C_ADDR 0x64

// Configuration Registers
#define AS7421_CFG_MISC         0x38
#define AS7421_CFG_LED_MULT     0x39
#define AS7421_TEMP_COMPDAC     0x3C
#define AS7421_LED_WAIT         0x3D
#define AS7421_CFG_PINMAP       0x3E

// RAM Configuration (32 bytes)
#define AS7421_CFG_RAM_0        0x40
#define AS7421_CFG_RAM_31       0x5F

// Enable Register
#define AS7421_ENABLE           0x60

// Configuration Registers
#define AS7421_LTF_ITIME_L      0x61
#define AS7421_LTF_ITIME_M      0x62
#define AS7421_LTF_ITIME_H      0x63
#define AS7421_LTF_WTIME_L      0x64
#define AS7421_LTF_WTIME_M      0x65
#define AS7421_LTF_WTIME_H      0x66
#define AS7421_CFG_LTF          0x67
#define AS7421_CFG_LED          0x68
#define AS7421_LTF_ICOUNT       0x69
#define AS7421_CFG_RAM          0x6A
#define AS7421_CFG_GPIO         0x6B
#define AS7421_INT_ENABLE       0x6C
#define AS7421_CFG_AZ           0x6D

// Special initialization registers
#define AS7421_INIT_REG1        0x6F
#define AS7421_INIT_REG2        0x6E

// Status Registers
#define AS7421_STATUS_0         0x70
#define AS7421_STATUS_1         0x71
#define AS7421_STATUS_2         0x72
#define AS7421_STATUS_3         0x73
#define AS7421_STATUS_6         0x76
#define AS7421_STATUS_7         0x77

// Temperature Registers
#define AS7421_TEMP0_L          0x78
#define AS7421_TEMP0_H          0x79
#define AS7421_TEMP1_L          0x7A
#define AS7421_TEMP1_H          0x7B
#define AS7421_TEMP2_L          0x7C
#define AS7421_TEMP2_H          0x7D
#define AS7421_TEMP3_L          0x7E
#define AS7421_TEMP3_H          0x7F

// Channel Data Registers (0x80 - 0xFF, 64 channels x 2 bytes)
#define AS7421_CH0_DATA_L       0x80

// ENABLE Register Bits
#define AS7421_PON              0x01
#define AS7421_LTF_EN           0x02
#define AS7421_TSD_EN           0x04
#define AS7421_SYNC_EN          0x08

// STATUS_7 Register Bits
#define AS7421_ADATA            0x01
#define AS7421_ASAT             0x08
#define AS7421_DSAT             0x10
#define AS7421_DLOST            0x20

// CFG_LTF Register Values
#define AS7421_LTF_CYCLE_1      0x00
#define AS7421_LTF_CYCLE_2      0x08
#define AS7421_LTF_CYCLE_3      0x10
#define AS7421_LTF_CYCLE_4      0x18  // All 64 channels

// LED Configuration
#define AS7421_LED_CURRENT_50MA 0x00
#define AS7421_LED_CURRENT_75MA 0x01

// Number of channels
#define AS7421_NUM_CHANNELS     64
#define AS7421_NUM_SPECTRAL_CH  61  // 61 spectral + 3 reference

// Photodiode to wavelength mapping (from datasheet Figure 14)
struct PhotodiodeMap {
  uint8_t pd_index;
  uint16_t wavelength;
};

class AS7421 {
public:
  AS7421();
  
  // Initialization
  bool begin(TwoWire &wire = Wire);
  void reset();
  uint8_t getDeviceID();
  uint8_t getRevisionID();
  
  // Power Management
  void powerOn();
  void powerOff();
  bool isPoweredOn();
  
  // SMUX Configuration (multiplexer setup)
  void configureSMUX();
  void configureSMUXCustom(const uint8_t *config, uint8_t length);
  void setGainConfiguration(const uint8_t *gainConfig);
  
  // Integration Time (in microseconds)
  void setIntegrationTime(uint32_t timeUs);
  uint32_t getIntegrationTime();
  
  // Wait Time between measurements
  void setWaitTime(uint32_t timeUs);
  
  // Measurement Control
  void enableMeasurement(bool enable = true);
  bool isMeasurementEnabled();
  void startSingleMeasurement();
  void startContinuousMeasurement(uint8_t count = 0xFF);
  bool isDataReady();
  bool isMeasurementBusy();
  
  // Data Reading
  uint16_t readChannel(uint8_t channel);
  void readAllChannels(uint16_t *data);
  bool readSpectralData(uint16_t *data);
  
  // Temperature Reading (4 temperature values, one per integration cycle)
  int16_t readTemperature(uint8_t cycle = 0);
  float getTemperatureCelsius(uint8_t cycle = 0);
  
  // LED Control (4 integrated NIR LEDs)
  void configureLED(uint8_t offset, uint8_t mult, uint8_t current);
  void enableLED(bool enable = true);
  void setLEDCurrent(uint8_t current);  // 0=50mA, 1=75mA
  void setLEDMultiplexer(uint8_t mult); // 0x1F = all LEDs on
  void setLEDWaitTime(uint8_t waitTime);
  void configureLEDForAllCycles();
  
  // Auto LED control
  void setLEDAutoMode(uint8_t mode);  // 0=OFF, 1=first OFF/second ON, 2=first ON/second OFF, 3=ON
  
  // Status and Interrupts
  uint8_t getStatus();
  void clearStatus();
  bool isAnalogSaturated();
  bool isDigitalSaturated();
  bool isDataLost();
  void enableInterrupt(uint8_t mask);
  
  // Auto-Zero Configuration
  void configureAutoZero(bool enable, uint8_t waitTime = 0);
  void performAutoZero();
  
  // GPIO Configuration
  void configureGPIO(bool output, bool invert = false);
  void setGPIO(bool high);
  bool readGPIO();
  
  // Synchronization
  void enableSync(bool enable = true);
  
  // Wavelength mapping
  uint16_t getWavelength(uint8_t channel);
  void getWavelengthArray(uint16_t *wavelengths);
  
  // Advanced Configuration
  void setMeasurementCycles(uint8_t cycles);  // 1-4 cycles for 16, 32, 48, or 64 channels
  void setCLKMOD(uint8_t mode);
  
private:
  TwoWire *_wire;
  uint8_t _address;
  uint32_t _integrationTime;
  
  // I2C Operations
  void writeRegister(uint8_t reg, uint8_t value);
  uint8_t readRegister(uint8_t reg);
  void readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len);
  uint16_t read16(uint8_t reg);
  void write24(uint8_t reg, uint32_t value);
  uint32_t read24(uint8_t reg);
  
  // RAM Configuration
  void writeRAM(uint8_t offset, const uint8_t *data, uint8_t len);
  void readRAM(uint8_t offset, uint8_t *data, uint8_t len);
  
  // Helper functions
  void initDevice();
  uint16_t mapPhotodiodetToWavelength(uint8_t pd);
};

#endif