# Climate Czar Pellet Stove Controller
Raspberry Pi Zero W powered pellet stove controller for any single or dual auger pellet or bio-fuel fired stove or boiler.

This system works with any network enabled Raspberry Pi or even clones **_(also tested on a Banana Pi BPI-M2-Zero)_**. Designed using an England's Stove Works 25-PDV pellet stove which has both upper and lower augers where the lower auger runs continuously. In my case, I just run the lower auger motor and combustion blower both on the same relay.

It's actually cheaper to build this unit that it is to buy a new main board for the stove, and this system makes the stove WiFi enabled. The vacuum switches were eliminated since they don't really provide any real level of safety other than shutting down the stove in 5 minutes if you leave the door open. Personally, I have never accidentally left my door open.

The thermistor is replaced by a digital I2C interfaced MLX90614B non-contact infrared temperature sensor **_(the device that's used in laser thermometers)_**. These are just more accurate than a thermistor and have a higher temperature range than a DS18B20 OneWire temperature sensor.
