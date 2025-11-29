# Athena
Intended to play back audio samples periodically using an esp32, MAX98357A amp and a small speaker.
Currently powered by an LM2596 set to output 5v. (input power 6x AA NiMh battery pack)

Wav data is encoded into binary and stored in the ESP's memory, rather than adding SD card. 

Uses captive portal wifi to take the time from the wifi client's device and set the RTC clock up. The client seamlessly disconnects by disabling the esp32's wifi when this step completes. 

# Requrirements 

A single battery between the various boards must be shared, and rechargeable via USB-C, ie no coin cell for the RTC. 

# BOM

| Part                                 | Purpose                                         |
| ------------------------------------ | ----------------------------------------------- |
| NodeMcu ESP32 WROOM-32 Type C CH340C | ESP32 brains of the operation development Board |
| MAX98357A                            | Amplifier to drive speaker                      |
| LM2596                               | DC-DC Buck converter for Battery to 5v          |
| DS3231 RTC                           | Real Time Clock                                 |
| 6x Energiser Recharge AA 1300mAh     | Power                                           |
| 40mm 4ohm 3W speaker                 | Produce output                                  |


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


| DS3231 pin | TBC | Colour | Purpose                                      |
|------------|-----|--------|----------------------------------------------|
| GND        | --- | TBC    | Ground                                       |
| VCC        | --- | TBC    | Power to board                               |
| SDA        | --- | TBC    | I²C data line                                |
| SCL        | --- | TBC    | I²C clock line                               |
| SQW        | --- | TBC    | Square-wave / interrupt output               |
| 32K        | --- | TBC    | 32.768kHz temperature-compensated clock out  |

