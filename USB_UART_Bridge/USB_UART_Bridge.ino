// #define ESP32CAM_GPIO0 2  // Connect to ESP32-CAM's GPIO0
// #define ESP32CAM_RST 4    // Connect to ESP32-CAM's RST

void setup() {
  // pinMode(ESP32CAM_GPIO0, OUTPUT);
  // pinMode(ESP32CAM_RST, OUTPUT);
  
  // Put ESP32-CAM in programming mode
  // digitalWrite(ESP32CAM_GPIO0, LOW);
  // digitalWrite(ESP32CAM_RST, LOW);
  // delay(100);
  // digitalWrite(ESP32CAM_RST, HIGH);
  
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
}

void loop() {
  if (Serial.available()) {
    Serial2.write(Serial.read());
  }

  if (Serial2.available()) {
    Serial.write(Serial2.read());
  }
}