#include <OneWire.h>
#include <DallasTemperature.h>
#include <TM1637Display.h>

// ---------------------------
// Water Pressure Sensor Setup
// ---------------------------
#define PRESSURE_PIN  27       // GPIO connected to water pressure sensor
#define NUM_CALIB_SAMPLES 100  // Number of samples for zero-offset calibration

float V;      // Measured voltage
float P;      // Calculated pressure in KPa
int adcOffset = 0;  // ADC value corresponding to zero pressure

// Add: track whether a DS18B20 was found
bool tempSensorDetected = false;

// ---------------------------
// DS18B20 Temperature Sensor Setup
// ---------------------------
#define ONE_WIRE_BUS  26      // GPIO connected to DS18B20 data line
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ---------------------------
// TM1637 Display Setup
// ---------------------------
#define CLK 18
#define DIO 19
TM1637Display display(CLK, DIO);

// ---------------------------
// Relay Control Setup
// ---------------------------
#define PUMP_RELAY_PIN 33   // GPIO connected to pump relay
#define HEATER_RELAY_PIN 25 // GPIO connected to heater relay

// Display timing variables
unsigned long lastDisplayUpdate = 0;
bool showingPressure = true;
const unsigned long DISPLAY_CYCLE_TIME = 3000; // 3 seconds

void setup() {
  Serial.begin(115200);
  delay(1000);

  // --------------------------
  // Initialize TM1637 Display
  // --------------------------
  display.setBrightness(0x0f); // Set brightness (0x00 to 0x0f)

  // --------------------------
  // Water pressure sensor calibration
  // --------------------------
  Serial.println("Calibrating water pressure sensor... Make sure sensor is not under pressure.");
  int adcMin = 4095; // initialize to max ADC
  for (int i = 0; i < NUM_CALIB_SAMPLES; i++) {
    int val = analogRead(PRESSURE_PIN);
    if (val < adcMin) adcMin = val;
    delay(10);
  }
  adcOffset = adcMin;
  Serial.print("Calibration complete. ADC Offset = ");
  Serial.println(adcOffset);

  // --------------------------
  // Start DS18B20
  // --------------------------
  sensors.begin();

  // Add: report detected DS18B20 devices and addresses
  {
    int deviceCount = sensors.getDeviceCount();
    Serial.print("DS18B20 device count: ");
    Serial.println(deviceCount);
    if (deviceCount <= 0) {
      Serial.println("Warning: No DS18B20 devices detected. Check wiring and pull-up resistor (4.7k).");
      tempSensorDetected = false;
    } else {
      tempSensorDetected = true;
      for (int i = 0; i < deviceCount; i++) {
        DeviceAddress addr;
        if (sensors.getAddress(addr, i)) {
          Serial.print("Device ");
          Serial.print(i);
          Serial.print(" address: ");
          for (uint8_t b = 0; b < 8; b++) {
            if (addr[b] < 16) Serial.print("0");
            Serial.print(addr[b], HEX);
          }
          Serial.println();
        } else {
          Serial.print("Device ");
          Serial.print(i);
          Serial.println(" address not available.");
        }
      }
    }
  }

  // --------------------------
  // Initialize Relays
  // --------------------------
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);  // Initialize pump relay off
  pinMode(HEATER_RELAY_PIN, OUTPUT);
  digitalWrite(HEATER_RELAY_PIN, LOW);  // Initialize heater relay off

  // --------------------------
  // Initialize GPIO Pin 14 as output HIGH for 3.3V
  // --------------------------
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);

  // --------------------------
  // Header for Serial Plotter
  // --------------------------
  Serial.println("ADC,Pressure(KPa),Voltage(V),Temp(C)");
}

void loop() {
  // --------------------------
  // Read water pressure sensor
  // --------------------------
  int adcValue = analogRead(PRESSURE_PIN);
  V = adcValue * (3.3 / 4095.0); // Convert ADC to voltage
  P = (V - (adcOffset * (3.3 / 4095.0))) * (1000.0 / 4.0); // Pressure in KPa

  // --------------------------
  // Read DS18B20 temperature in Celsius
  // --------------------------
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  // Handle disconnected sensor case (DallasTemperature defines DEVICE_DISCONNECTED_C, commonly -127)
  bool tempValid = true;
  if (tempC == DEVICE_DISCONNECTED_C) { // No Temp
    tempValid = false;
    Serial.println("Error: Could not read temperature (DEVICE_DISCONNECTED_C). Check wiring, pull-up resistor, and sensor power.");
    
  } else { // Valid Temperature
    if (tempC < 37.5) {
      digitalWrite(PUMP_RELAY_PIN, HIGH);
      digitalWrite(HEATER_RELAY_PIN, HIGH);
    } else {
      digitalWrite(PUMP_RELAY_PIN, HIGH);
      digitalWrite(HEATER_RELAY_PIN, HIGH);
    }
  }

  // --------------------------
  // Print all data for Serial Plotter
  // --------------------------
  Serial.print(adcValue);
  Serial.print(",");
  Serial.print(P, 1);
  Serial.print(",");
  Serial.print(V, 3);
  if (tempValid) {
    Serial.print(",");
    Serial.println(tempC, 2);
  } else {
    Serial.println(",ERROR");
  }

  // --------------------------
  // Update TM1637 Display (cycle every 3 seconds)
  // --------------------------
  unsigned long currentTime = millis();
  if (currentTime - lastDisplayUpdate >= DISPLAY_CYCLE_TIME) {
    lastDisplayUpdate = currentTime;
    showingPressure = !showingPressure;
  }

  if (showingPressure) {
    displayPressure(P);
  } else {
    if (tempValid) {
      displayTemperature(tempC);
    } else {
      displayTemperatureError(); // show error on display when temp invalid
    }
  }

  // --------------------------
  // Relay test: toggle relays on/off every second
  // --------------------------
  // {
  //   static unsigned long lastToggle = 0;
  //   static bool relaysOn = false;
  //   const unsigned long TOGGLE_INTERVAL = 5000; // ms

  //   if (currentTime - lastToggle >= TOGGLE_INTERVAL) {
  //     lastToggle = currentTime;
  //     relaysOn = !relaysOn;
  //     digitalWrite(PUMP_RELAY_PIN, relaysOn ? HIGH : LOW);
  //     digitalWrite(HEATER_RELAY_PIN, relaysOn ? HIGH : LOW);
  //     Serial.print("Relay test - state: ");
  //     Serial.println(relaysOn ? "ON" : "OFF");
  //   }
  // }

  delay(200); // Update every 200ms
}

// --------------------------
// Display Functions
// --------------------------
void displayPressure(float pressure) {
  // Convert KPa to MPa and display as PXX.X format
  float pressureMPa = pressure / 1000.0; // Convert KPa to MPa
  int pressureInt = (int)(pressureMPa * 10); // Get integer representation of XX.X
  
  uint8_t data[4];
  data[0] = 0x73; // "P"
  
  // Format as XX.X
  if (pressureInt >= 100) {
    // For pressure >= 10.0 MPa, show PXX.X
    data[1] = display.encodeDigit((pressureInt / 100) % 10);
    data[2] = display.encodeDigit((pressureInt / 10) % 10) | 0x80; // Add decimal point
    data[3] = display.encodeDigit(pressureInt % 10);
  } else if (pressureInt >= 10) {
    // For pressure >= 1.0 MPa, show PX.X
    data[1] = 0x00; // blank
    data[2] = display.encodeDigit((pressureInt / 10) % 10) | 0x80; // Add decimal point
    data[3] = display.encodeDigit(pressureInt % 10);
  } else {
    // For pressure < 1.0 MPa, show P0.X
    data[1] = 0x00; // blank
    data[2] = display.encodeDigit(0) | 0x80; // "0" with decimal point
    data[3] = display.encodeDigit(pressureInt % 10);
  }
  
  display.setSegments(data);
}

void displayTemperature(float temperature) {
  // Display temperature as CXX.X format
  int tempInt = (int)(temperature * 10); // Get temperature * 10 for one decimal place
  
  uint8_t data[4];
  data[0] = 0x39; // "C"
  
  // Format as XX.X
  if (tempInt >= 1000) {
    // For temperatures >= 100.0C, show CXX (no decimal due to space)
    data[1] = display.encodeDigit((tempInt / 1000) % 10);
    data[2] = display.encodeDigit((tempInt / 100) % 10);
    data[3] = display.encodeDigit((tempInt / 10) % 10);
  } else if (tempInt >= 100) {
    // For temperatures >= 10.0C, show CXX.X
    data[1] = display.encodeDigit((tempInt / 100) % 10);
    data[2] = display.encodeDigit((tempInt / 10) % 10) | 0x80; // Add decimal point
    data[3] = display.encodeDigit(tempInt % 10);
  } else {
    // For temperatures < 10.0C, show CX.X
    data[1] = 0x00; // blank
    data[2] = display.encodeDigit((tempInt / 10) % 10) | 0x80; // Add decimal point
    data[3] = display.encodeDigit(tempInt % 10);
  }
  
  display.setSegments(data);
}

// Add: display an obvious error state for temperature (shows "C---")
void displayTemperatureError() {
  uint8_t data[4];
  data[0] = 0x39; // "C"
  const uint8_t SEG_MINUS = 0x40; // common segment for minus sign (may vary by TM1637 mapping)
  data[1] = SEG_MINUS;
  data[2] = SEG_MINUS;
  data[3] = SEG_MINUS;
  display.setSegments(data);
}
