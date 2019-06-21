# Timer
Simple timer with LED indication
## Hardware
Display (HX1230 or other), encoder and LED
### Connection
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
Encoder:
```
5V -> 5V
KEY -> D9
S1 -> D3
S2 -> D2
```
LED:
```
short pin -> GND
tall pin -> D6
```
## Software
Required libraries:
```
Arduino.h
U8g2lib.h
Wire.h
SPI.h
```
