# Inflation Testing

# Thu 22 May 2025
## Components Required

### Micro Robotics

- `R45`: 18B20 Digital [Temperature Sensor](https://www.robotics.org.za/18B20-2M), 2m cable.
- `R33` [Heater Element](https://www.robotics.org.za/EF03018) 12V 40W.
- `R113`: [ESP32](https://www.robotics.org.za/ESP32-DEV-CH340-C) Dev Board CH340 - USB-C.
- `R310`: XGZ [Pressure Sensor](https://www.robotics.org.za/XGZP61611D3000616BS) 1.6MPA - Analog Out. **Currently out of stock.** Ek kon nie ander kry wat reasonably priced was nie, maar ek sal later weer soek.

## Communica 
- `R60`: BDD [RELAY BOARD](https://www.communica.co.za/products/bdd-relay-board-2ch-3-3v) 2CH 3.3V.

Total Cost = `R561`

Ek het al by albei van die plekke gekoop en dis goeie plekke. Ek kan dit bestel en gaan haal as jy wil. Dan kan ek ook sommer begin uitfigure hoe dit alles werk. 

# Wed 27 Aug 2025
## ESP32 as USB-to-UART Bridge
### Wiring (ESP to ESP-CAM)
- RX -> UOR
- TX -> UOT
- 5v -> 5V
- GND -> GND

### Other important steps
1. Disable the ESP32 by connecting EN to GND.
2. Put ESP-CAM in boot mode by connecting IO0 to GND, and pressing RST.

## Sensor Wiring

To wire your ESP32 for the provided code, follow these steps:

1. Water Pressure Sensor
Signal Pin: Connect the signal output of your water pressure sensor to GPIO 27 on the ESP32.
Power: Connect the power pin of the sensor to the 3.3V pin of the ESP32.
Ground: Connect the ground pin of the sensor to the GND pin of the ESP32.
2. DS18B20 Temperature Sensor
Data Pin: Connect the data pin of the DS18B20 to GPIO 26 on the ESP32.
Pull-up Resistor: Add a 4.7kΩ resistor between the data pin and the 3.3V pin to ensure proper communication.
Power: Connect the VCC pin of the DS18B20 to the 3.3V pin of the ESP32.
Ground: Connect the GND pin of the DS18B20 to the GND pin of the ESP32.
3. Additional Notes
Ensure that the ESP32 is powered properly, either via USB or an external power source.
Double-check the pin numbers in the code (PRESSURE_PIN = 27, ONE_WIRE_BUS = 26) and ensure they match your wiring.
If you're using a breadboard, make sure all connections are secure to avoid intermittent issues.