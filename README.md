# athena
Plays back random audio samples using an esp32, MAX98357A amp and a 4 ohm, 3w speaker.
Currently powered by an LM2596 set to output 5v. (input power 6x AA NiMh battery pack)

BOM

| Part                                 | Purpose                                         |
| ------------------------------------ | ----------------------------------------------- |
| NodeMcu ESP32 WROOM-32 Type C CH340C | ESP32 brains of the operation development Board |
| MAX98357A                            | Amplifier to drive speaker                      |
| LM2596                               | DC-DC Buck converter for Battery to 5v          |


The MAX98357A pins are LRC-BCLK-DIN-GAIN-SD-GND-V1N

| ESP32 pin | MAX pin    | Colour | Purpose                     |
| --------- | ---------- | ------ | --------------------------- |
| GPIO25    | (1)LRC     | GREY   | I²S Word Select             |
| GPIO26    | (2)BCLK    | WHITE  | I²S Bit Clock               |
| GPIO27    | (3)DIN     | BLACK  | I²S Data Out                |
| GPIO14    | (4)GAIN    | BROWN  | leave unconnected or ignore |
| GPIO13    | (5)SD      | RED    | Enable/Shutdown control     |
| GND       | (6)GND     | ORANGE | Ground                      |
| VIN       | (7)VIN     | YELLOW | 5V to power amp             |
