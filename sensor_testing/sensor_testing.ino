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

  // --------------------------
  // Print all data for Serial Plotter
  // --------------------------
  Serial.print(adcValue);
  Serial.print(",");
  Serial.print(P, 1);
  Serial.print(",");
  Serial.print(V, 3);
  Serial.print(",");
  Serial.println(tempC, 2);

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
    displayTemperature(tempC);
  }

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
