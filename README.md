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


# WIRING

| ESP32 pin | Module Pin | Module Type | Cable Colour | Purpose                     |
| --------- | ---------- | ----------- | ------------ | --------------------------- |
| Not used  | (1)GND     | DS3231      | BROWN        | Common Ground for RTC       |
| Not used  | (2)VCC     | DS3231      | RED          | Common Voltage+ for RTC     |
| GPIO21    | SDA        | DS3231      | BLUE         | I²C data line               |
| GPIO22    | SCL        | DS3231      | PURPLE       | I²C clock line              | 
| GPIO25    | (1)LRC     | MAX98357A   | GREY         | I²S Word Select             |
| GPIO26    | (2)BCLK    | MAX98357A   | WHITE        | I²S Bit Clock               |
| GPIO27    | (3)DIN     | MAX98357A   | BLACK        | I²S Data Out                |
| GPIO14    | (4)GAIN    | MAX98357A   | BROWN        | leave unconnected or ignore |
| GPIO13    | (5)SD      | MAX98357A   | RED          | Enable/Shutdown control     |
| Not used  | (6)GND     | MAX98357A   | ORANGE       | Common Ground for Amp       |
| Not used  | (7)VIN     | MAX98357A   | YELLOW       | 5V to power amp             |
| VIN       | OUT+       | LM2596      | WHITE        | Variable + output           |
| GND       | OUT-       | LM2596      | BLACK        | Common Ground for ESP32     |
| Not used  | IN+        | LM2596      | RED          | Bat +                       |
| Not used  | IN-        | LM2596      | BROWN        | Bat -                       |

The ESP VIN pin can be bypassed and instead use the 3v3 pin as long as the ESP and other components are all fed 3.3v by the LM2596.
This should be more power efficent. 

# Unused pins info on DS3231

| DS3231 pin | TBC | Colour | Purpose                                      |
|------------|-----|--------|----------------------------------------------|
| SQW        | --- | TBC    | Square-wave / interrupt output               |
| 32K        | --- | TBC    | 32.768kHz temperature-compensated clock out  |

