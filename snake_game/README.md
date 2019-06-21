# Snake game
Snake game like in oldschool Nokia
## Hardware
Display (HX1230 or other) and touch keyboard (HW-136), but you can use stick or other controller with simple additions in code.
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
Keyboard:
```
GND -> GND
VCC -> 5V
SCL -> D2
SDO -> D3
```
## Software
Required libraries:
```
TTP229.h
Arduino.h
U8g2lib.h
Wire.h
SPI.h
```
