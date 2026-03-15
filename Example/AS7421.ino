/*
  AS7421 Complete Example - 64-Channel NIR Spectral Sensor
  
  This example demonstrates proper usage of the AS7421 including:
  - Correct initialization sequence
  - Custom I2C pins (SCL=20, SDA=21)
  - SMUX configuration for all 64 channels
  - LED control for reflectance measurements
  - Reading all spectral channels
  - Temperature monitoring
  
  Hardware Connections:
  - SDA to Pin 21
  - SCL to Pin 20
  - VDD to 3.3V (must be applied first)
  - LEDA to 3.3V (apply after VDD)
  - GND/PGND to GND
  - INT (optional) to digital pin for interrupts
  
  Based on AS7421 Datasheet DS000667 v2-00
*/

#include <Wire.h>
#include "AS7421.h"

// Custom I2C pins
#define I2C_SDA 21
#define I2C_SCL 20

AS7421 sensor;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("AS7421 64-Channel NIR Spectral Sensor");
  Serial.println("======================================");
  Serial.println("Based on Datasheet DS000667 v2-00\n");

  delay(500);

  
  // Initialize I2C with custom pins
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.print("I2C initialized - SDA: Pin ");
  Serial.print(I2C_SDA);
  Serial.print(", SCL: Pin ");
  Serial.println(I2C_SCL);
  
  // Initialize the sensor
  if (!sensor.begin(Wire)) {
    Serial.println("ERROR: Failed to initialize AS7421!");
    Serial.println("Check:");
    Serial.println("  - I2C connections (SDA/SCL)");
    Serial.println("  - Power supply (VDD must be on before LEDA)");
    Serial.println("  - I2C address (0x64)");
    while (1) delay(10);
  }
  
  Serial.println("AS7421 initialized successfully!");
  
  // Print device information
  Serial.print("Device ID: 0x");
  Serial.println(sensor.getDeviceID(), HEX);
  Serial.print("Revision ID: 0x");
  Serial.println(sensor.getRevisionID(), HEX);
  
  // Configure measurement parameters
  Serial.println("\nConfiguring sensor...");
  
  // Set integration time to 20ms (matches Python library default)
  sensor.setIntegrationTime(256000);
  
  // Set wait time
  sensor.setWaitTime(10000);
  
  Serial.println("Configuration complete!");
  Serial.println("\nMeasurement Settings:");
  Serial.println("  Integration Time: 20ms per cycle");
  Serial.println("  Wait Time: 10ms");
  Serial.println("  Total Measurement Time: ~80ms (4 cycles)");
  Serial.println("  Default Gain: 64x");
  Serial.println("  Number of Channels: 64");
  Serial.println("  Wavelength Range: 750-1050nm");
  Serial.println("  SMUX: Using verified configuration from Python library");
  
  delay(1000);
  Serial.println("\n=== Starting Measurements ===\n");
  
  // Enable measurement system
  sensor.enableMeasurement(true);
  Serial.println("Measurement system enabled");
}

void loop() {
  // Start a single measurement
  Serial.println("Starting measurement...");
  sensor.startSingleMeasurement();
  
  // Wait for measurement to complete with timeout
  unsigned long startTime = millis();
  unsigned long timeout = 5000; // 5 second timeout
  
  while (!sensor.isDataReady()) {
    if (millis() - startTime > timeout) {
      Serial.println("ERROR: Measurement timeout!");
      Serial.print("Status register: 0x");
      Serial.println(sensor.getStatus(), HEX);
      Serial.print("Busy flag: ");
      Serial.println(sensor.isMeasurementBusy());
      delay(5000);
      return;
    }
    delay(50);
  }
  
  Serial.println("Data ready!");
  
  // Check for any issues
  if (sensor.isAnalogSaturated()) {
    Serial.println("WARNING: Analog saturation detected!");
  }
  if (sensor.isDigitalSaturated()) {
    Serial.println("WARNING: Digital saturation detected!");
  }
  if (sensor.isDataLost()) {
    Serial.println("WARNING: Data lost!");
  }
  
  // Read all 64 channels
  uint16_t channels[64];
  Serial.println("Reading spectral data...");
  
  // Read status first (required by datasheet)
  sensor.clearStatus();
  
  // Read all channels directly
  sensor.readAllChannels(channels);
  
  Serial.println("=== Spectral Measurement ===");
    
    // Display data organized by wavelength
    Serial.println("\nCH\tWavelength(nm)\tCount");
    //Serial.println("--\t--------------\t-----");
    
   /* for (int i = 0; i < 64; i++) {
      uint16_t wavelength = sensor.getWavelength(i);
      
      //Serial.print(i);
      //Serial.print("\t");
      //Serial.print(wavelength);
      //Serial.print("\t");
      Serial.println(channels[i]);
      
      // Add spacing every 8 channels
      //if ((i + 1) % 8 == 0 && i < 63) {
       // Serial.println();
      //}
    }*/

Serial.print(sensor.getWavelength(1)); Serial.print("\t"); Serial.println(channels[1]);
Serial.print(sensor.getWavelength(48)); Serial.print("\t"); Serial.println(channels[48]);
Serial.print(sensor.getWavelength(2)); Serial.print("\t"); Serial.println(channels[2]);
Serial.print(sensor.getWavelength(34)); Serial.print("\t"); Serial.println(channels[34]);
Serial.print(sensor.getWavelength(16)); Serial.print("\t"); Serial.println(channels[16]);
Serial.print(sensor.getWavelength(32)); Serial.print("\t"); Serial.println(channels[32]);
Serial.print(sensor.getWavelength(18)); Serial.print("\t"); Serial.println(channels[18]);
Serial.print(sensor.getWavelength(51)); Serial.print("\t"); Serial.println(channels[51]);
Serial.print(sensor.getWavelength(4)); Serial.print("\t"); Serial.println(channels[4]);
Serial.print(sensor.getWavelength(49)); Serial.print("\t"); Serial.println(channels[49]);
Serial.print(sensor.getWavelength(3)); Serial.print("\t"); Serial.println(channels[3]);
Serial.print(sensor.getWavelength(35)); Serial.print("\t"); Serial.println(channels[35]);
Serial.print(sensor.getWavelength(17)); Serial.print("\t"); Serial.println(channels[17]);
Serial.print(sensor.getWavelength(33)); Serial.print("\t"); Serial.println(channels[33]);
Serial.print(sensor.getWavelength(19)); Serial.print("\t"); Serial.println(channels[19]);
Serial.print(sensor.getWavelength(54)); Serial.print("\t"); Serial.println(channels[54]);
Serial.print(sensor.getWavelength(0)); Serial.print("\t"); Serial.println(channels[0]);
Serial.print(sensor.getWavelength(52)); Serial.print("\t"); Serial.println(channels[52]);
Serial.print(sensor.getWavelength(6)); Serial.print("\t"); Serial.println(channels[6]);
Serial.print(sensor.getWavelength(38)); Serial.print("\t"); Serial.println(channels[38]);
Serial.print(sensor.getWavelength(20)); Serial.print("\t"); Serial.println(channels[20]);
Serial.print(sensor.getWavelength(36)); Serial.print("\t"); Serial.println(channels[36]);
Serial.print(sensor.getWavelength(22)); Serial.print("\t"); Serial.println(channels[22]);
Serial.print(sensor.getWavelength(55)); Serial.print("\t"); Serial.println(channels[55]);
Serial.print(sensor.getWavelength(5)); Serial.print("\t"); Serial.println(channels[5]);
Serial.print(sensor.getWavelength(53)); Serial.print("\t"); Serial.println(channels[53]);
Serial.print(sensor.getWavelength(7)); Serial.print("\t"); Serial.println(channels[7]);
Serial.print(sensor.getWavelength(39)); Serial.print("\t"); Serial.println(channels[39]);
Serial.print(sensor.getWavelength(21)); Serial.print("\t"); Serial.println(channels[21]);
Serial.print(sensor.getWavelength(37)); Serial.print("\t"); Serial.println(channels[37]);
Serial.print(sensor.getWavelength(23)); Serial.print("\t"); Serial.println(channels[23]);
Serial.print(sensor.getWavelength(40)); Serial.print("\t"); Serial.println(channels[40]);
Serial.print(sensor.getWavelength(26)); Serial.print("\t"); Serial.println(channels[26]);
Serial.print(sensor.getWavelength(42)); Serial.print("\t"); Serial.println(channels[42]);
Serial.print(sensor.getWavelength(24)); Serial.print("\t"); Serial.println(channels[24]);
Serial.print(sensor.getWavelength(56)); Serial.print("\t"); Serial.println(channels[56]);
Serial.print(sensor.getWavelength(10)); Serial.print("\t"); Serial.println(channels[10]);
Serial.print(sensor.getWavelength(58)); Serial.print("\t"); Serial.println(channels[58]);
Serial.print(sensor.getWavelength(8)); Serial.print("\t"); Serial.println(channels[8]);
Serial.print(sensor.getWavelength(41)); Serial.print("\t"); Serial.println(channels[41]);
Serial.print(sensor.getWavelength(27)); Serial.print("\t"); Serial.println(channels[27]);
Serial.print(sensor.getWavelength(43)); Serial.print("\t"); Serial.println(channels[43]);
Serial.print(sensor.getWavelength(25)); Serial.print("\t"); Serial.println(channels[25]);
Serial.print(sensor.getWavelength(57)); Serial.print("\t"); Serial.println(channels[57]);
Serial.print(sensor.getWavelength(11)); Serial.print("\t"); Serial.println(channels[11]);
Serial.print(sensor.getWavelength(59)); Serial.print("\t"); Serial.println(channels[59]);
Serial.print(sensor.getWavelength(9)); Serial.print("\t"); Serial.println(channels[9]);
Serial.print(sensor.getWavelength(44)); Serial.print("\t"); Serial.println(channels[44]);
Serial.print(sensor.getWavelength(30)); Serial.print("\t"); Serial.println(channels[30]);
Serial.print(sensor.getWavelength(46)); Serial.print("\t"); Serial.println(channels[46]);
Serial.print(sensor.getWavelength(28)); Serial.print("\t"); Serial.println(channels[28]);
Serial.print(sensor.getWavelength(60)); Serial.print("\t"); Serial.println(channels[60]);
Serial.print(sensor.getWavelength(14)); Serial.print("\t"); Serial.println(channels[14]);
Serial.print(sensor.getWavelength(62)); Serial.print("\t"); Serial.println(channels[62]);
Serial.print(sensor.getWavelength(12)); Serial.print("\t"); Serial.println(channels[12]);
Serial.print(sensor.getWavelength(15)); Serial.print("\t"); Serial.println(channels[15]);
Serial.print(sensor.getWavelength(29)); Serial.print("\t"); Serial.println(channels[29]);
Serial.print(sensor.getWavelength(31)); Serial.print("\t"); Serial.println(channels[31]);
Serial.print(sensor.getWavelength(45)); Serial.print("\t"); Serial.println(channels[45]);
Serial.print(sensor.getWavelength(47)); Serial.print("\t"); Serial.println(channels[47]);
Serial.print(sensor.getWavelength(61)); Serial.print("\t"); Serial.println(channels[61]);
    
/*    // Read and display temperature from each cycle
    Serial.println("\nTemperature Readings:");
    for (int cycle = 0; cycle < 4; cycle++) {
      float temp = sensor.getTemperatureCelsius(cycle);
      Serial.print("  Cycle ");
      Serial.print(cycle);
      Serial.print(": ");
      Serial.print(temp, 2);
      Serial.println(" °C");
    }*/
    
    // Calculate some statistics
    uint32_t sum = 0;
    uint16_t minVal = 65535;
    uint16_t maxVal = 0;
    uint8_t minCh = 0, maxCh = 0;
    uint8_t saturatedCount = 0;
    uint8_t zeroCount = 0;
    uint8_t validCount = 0;
    
    for (int i = 0; i < 64; i++) {
      if (channels[i] >= 65500) {
        saturatedCount++;
      } else if (channels[i] == 0) {
        zeroCount++;
      } else {
        validCount++;
        sum += channels[i];
      }
      
      if (channels[i] < minVal) {
        minVal = channels[i];
        minCh = i;
      }
      if (channels[i] > maxVal) {
        maxVal = channels[i];
        maxCh = i;
      }
    }
    /*
    Serial.println("\nStatistics:");
    Serial.print("  Valid channels: ");
    Serial.println(validCount);
    Serial.print("  Saturated channels: ");
    Serial.println(saturatedCount);
    Serial.print("  Zero/dark channels: ");
    Serial.println(zeroCount);
    
    if (validCount > 0) {
      Serial.print("  Average (valid only): ");
      Serial.println(sum / validCount);
    }
    
    Serial.print("  Min: ");
    Serial.print(minVal);
    Serial.print(" (CH");
    Serial.print(minCh);
    Serial.print(" @ ");
    Serial.print(sensor.getWavelength(minCh));
    Serial.println("nm)");
    Serial.print("  Max: ");
    Serial.print(maxVal);
    Serial.print(" (CH");
    Serial.print(maxCh);
    Serial.print(" @ ");
    Serial.print(sensor.getWavelength(maxCh));
    Serial.println("nm)");
    
    if (saturatedCount > 10) {
      Serial.println("\n*** WARNING: Many saturated channels! ***");
      Serial.println("    Consider:");
      Serial.println("    - Reducing integration time");
      Serial.println("    - Reducing gain configuration");
      Serial.println("    - Reducing light intensity");
    }
    Serial.println("\n" + String('-', 60) + "\n");
  */
  delay(8000);  // Wait 1 second between measurements
}

/*
  ==========================================
  ADDITIONAL EXAMPLE CODE
  ==========================================
  
  // Example 1: Continuous measurements
  sensor.startContinuousMeasurement(10);  // Take 10 measurements
  // or
  sensor.startContinuousMeasurement(0xFF);  // Continuous mode
  
  // Example 2: Custom integration time
  sensor.setIntegrationTime(131000);  // 131ms
  sensor.setIntegrationTime(32750);   // 32.75ms (faster but less sensitive)
  
  // Example 3: LED control
  sensor.setLEDCurrent(AS7421_LED_CURRENT_75MA);  // Higher power
  sensor.enableLED(true);   // Manually enable
  sensor.enableLED(false);  // Manually disable
  
  // Example 4: Auto LED mode
  sensor.setLEDAutoMode(0);  // LEDs always OFF
  sensor.setLEDAutoMode(1);  // First measurement OFF, second ON
  sensor.setLEDAutoMode(2);  // First measurement ON, second OFF
  sensor.setLEDAutoMode(3);  // LEDs always ON
  
  // Example 5: Custom SMUX configuration
  uint8_t customSMUX[8] = {0x21, 0x21, 0x21, 0x21, 0x43, 0x43, 0x43, 0x43};
  sensor.configureSMUXCustom(customSMUX, 8);
  
  // Example 6: Custom gain per ADC
  uint8_t gainConfig[32];
  for (int i = 0; i < 32; i++) {
    gainConfig[i] = 0x77;  // Both nibbles = 7 (128x gain)
  }
  sensor.setGainConfiguration(gainConfig);
  
  // Example 7: Use GPIO for sync
  sensor.configureGPIO(false, false);  // Input mode
  sensor.enableSync(true);  // Wait for GPIO trigger
  
  // Example 8: Interrupt-driven measurement
  sensor.enableInterrupt(0x01);  // Enable ADATA interrupt
  // In your ISR or main loop:
  if (sensor.isDataReady()) {
    uint16_t data[64];
    sensor.readSpectralData(data);
  }
  
  // Example 9: Read specific wavelength region
  // Find channels near 850nm
  for (int i = 0; i < 64; i++) {
    uint16_t wl = sensor.getWavelength(i);
    if (wl >= 840 && wl <= 860) {
      uint16_t value = sensor.readChannel(i);
      Serial.print(wl);
      Serial.print("nm: ");
      Serial.println(value);
    }
  }
  
  // Example 10: Temperature compensation
  float temp = sensor.getTemperatureCelsius(0);
  if (temp > 50.0) {
    Serial.println("WARNING: High temperature detected!");
  }
  
  // Example 11: Set measurement cycles
  sensor.setMeasurementCycles(1);  // Only cycle A (16 channels)
  sensor.setMeasurementCycles(2);  // Cycles A+B (32 channels)
  sensor.setMeasurementCycles(3);  // Cycles A+B+C (48 channels)
  sensor.setMeasurementCycles(4);  // All cycles (64 channels)
  
  // Example 12: Auto-zero calibration
  sensor.configureAutoZero(true, 2);  // Enable with 128µs wait
  sensor.performAutoZero();  // Manual trigger
  
  // Example 13: Material identification workflow
  // 1. Take dark reference (LEDs off)
  sensor.enableLED(false);
  sensor.startSingleMeasurement();
  while (!sensor.isDataReady()) delay(10);
  uint16_t dark[64];
  sensor.readSpectralData(dark);
  
  // 2. Take light reference (LEDs on, white standard)
  sensor.enableLED(true);
  sensor.startSingleMeasurement();
  while (!sensor.isDataReady()) delay(10);
  uint16_t reference[64];
  sensor.readSpectralData(reference);
  
  // 3. Measure sample
  sensor.startSingleMeasurement();
  while (!sensor.isDataReady()) delay(10);
  uint16_t sample[64];
  sensor.readSpectralData(sample);
  
  // 4. Calculate reflectance spectrum
  float reflectance[64];
  for (int i = 0; i < 64; i++) {
    reflectance[i] = (float)(sample[i] - dark[i]) / (reference[i] - dark[i]);
  }
*/
