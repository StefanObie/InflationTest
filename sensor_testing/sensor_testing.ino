#include <OneWire.h>
#include <DallasTemperature.h>

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

void setup() {
  Serial.begin(115200);
  delay(1000);

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

  delay(200); // Update every 200ms
}
