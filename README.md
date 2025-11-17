# athena
Plays back random audio samples using an esp32, MAX98357A amp and a speaker.

BOM

NodeMcu ESP32 WROOM-32 Type C CH340C Development Board
MAX98357A 




The MAX98357A pins are LRC-BCLK-DIN-GAIN-SD-GND-V1N

GPT suggests 

| ESP32 pin | MAX pin    | Purpose                     |
| --------- | ---------- | --------------------------- |
| GPIO25    | (1)LRC     | I²S Word Select             |
| GPIO26    | (2)BCLK    | I²S Bit Clock               |
| GPIO27    | (3)DIN     | I²S Data Out                |
| GPIO14    | (4)GAIN    | leave unconnected or ignore |
| GPIO13    | (5)SD      | Enable/Shutdown control     |
| GND       | (6)GND     | Ground                      |
| VIN       | (7)VIN     | 5V to power amp             |
