# Climate Czar Pellet Stove Controller
Raspberry Pi Zero W powered pellet stove controller for any single or dual auger pellet or bio-fuel fired stove or boiler.

This system works with any network enabled Raspberry Pi or even clones **_(also tested on a Banana Pi BPI-M2-Zero)_**. Designed using an England's Stove Works 25-PDV pellet stove which has both upper and lower augers where the lower auger runs continuously. In my case, I just run the lower auger motor and combustion blower both on the same relay.

It's actually cheaper to build this unit that it is to buy a new main board for the stove, and this system makes the stove WiFi enabled. The vacuum switches were eliminated since they don't really provide any real level of safety other than shutting down the stove in 5 minutes if you leave the door open. Personally, I have never accidentally left my door open.

The thermistor is replaced by a digital I2C interfaced MLX90614B non-contact infrared temperature sensor **_(the device that's used in laser thermometers)_**. These are just more accurate than a thermistor and have a higher temperature range than a DS18B20 OneWire temperature sensor.

The system uses a solid state relay board for all switching since mechanical relay contacts wear out, which actually happened to me during the initial development. An 8-channel SSR board is used so that if one does happen to die, you can just move its wires to one of the 4 spares to get right back up and running.

The system is so simple to build that a person can do it without any soldering necessary, just plain old breadboard jumper wires is all that's needed. It's controlled by its own Web API so that it is easy to connect to Climate Czar Server or run from the command line of any Windows, Mac, or Linux PC.
