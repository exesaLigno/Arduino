# Meteostation
Meteostation on BME280 sensor
## Hardware
Display HX1230 (or other), BME280 sensor, encoder.
### Connecting
Display:
```
GND -> GND
BL -> 3V3
VCC -> 5V
CLK -> D4
DIN -> D5
N/C is not connected
CE -> D7
RST -> D8
```
BME280:
```
VCC -> 3V3
GND -> GND
SCL -> D13
SDA -> D11
SCB -> D10
SDO -> D12
```
Encoder:
```
5V -> 5V
KEY -> D9
S1 -> D3
S2 -> D2
```
## Software
Libraries below required:
```
Arduino.h
U8g2lib.h (search actual version on Git)
Wire.h
SPI.h
Adafruit_Sensor.h
Adafruit_BME280.h
```
