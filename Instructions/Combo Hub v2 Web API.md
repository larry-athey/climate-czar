# Combo Hub v2 Web API

As of the 100% rewrite of the Climate Czar Combo Hub source code, a completely new web API comes with it. The Combo Hub now features a master/slave topology and the first part of the GET request is the ID number of the hub that you want to talk to. Climate Czar Server always talks directly to the master hub, which then relays API calls to slave units over LoRa WAN instead of TCP/IP.

Example Climate Czar Server web API calls:

`curl -s -m 5 --http0.9 http://master-ip-address/0/humidity` (get humidity from the master hub)<br>
`curl -s -m 5 --http0.9 http://master-ip-address/2/humidity` (get humidity from the first slave hub)<br>
`curl -s -m 5 --http0.9 http://master-ip-address/3/humidity` (get humidity from the second slave hub)<br>

`-s` tells curl run silently and not output download progress<br>
`-m 5` tells curl to time out the connection attempt after 5 seconds<br>
`--http0.9` tells curl to use a pre version 1.0 HTTP call for compatibility safety<br>

The Climate Czar Combo Hub can also communicate with your own custom LoRa WAN powered sensors. Many people have created their own Arduino/ESP32 powered devices to read things like soil temperature/moisture/pH, dissolved oxygen in aquaponics, pH probes, etc and added LoRa WAN radios to them such as the RYLR998. This web API allows you to make web calls to the hub with your own custom request message to your device and will return its response as a web request.

---

Below are explanations of all available web API calls. All of these calls are case sensitive, there is no AI at work here to decide if you're just "close enough". Breaking the rules will result in a JSON formatted failure message being returned.

**/hub-id/device-name** - Returns the device name of the selected hub.

**/hub-id/ds18b20/address/format** - Returns the temperature of a DS18B20 temperature sensor connected to the selected hub in the specified format "c" or "f". Sensor address identification is done using option #4 in the serial configuration main menu. Use ice water to determine which sensor is under test and make a note of its address. See **/hub-id/sensor-address/which-one** for an alternative to using the serial configuration for this procedure.

**/hub-id/humidity** - Returns the humidity level detected at the selected hub.

**/hub-id/light/format** - Returns the ambient light level detected at the selected hub in the specified format, "0" for Lux or "1" for percentage.

**/hub-id/lora-sensor/payload** - Returns the reading from any custom LoRa WAN powered sensor. These sensors still use the same LoRa WAN addressing scheme as a Combo Hub, ID #1 is reserved for the master hub, all other devices use LoRa WAN ID values 2 through 65535. The **"payload"** field is a free-form message and can be anything that the LoRa WAN sensor requires.

**/hub-id/reboot** - Reboots the selected hub.

**/hub-id/relay/which-one/state** - Toggles the on/off state of a specified relay port at the selected hub. **"which-one"** is a value of 1 to 15, **"state"** is 1 for on and 0 for off. Returns a JSON formatted success or failure message. Avoid toggling relay 16 unless you modify the source code to no longer use that output to perform hardware resets of the RYLR998 modem.

**/hub-id/relay-state/which-one** - Returns the on/off state of a specified relay port at the selected hub. **"which-one"** is a value of 1 to 15.

**/hub-id/sensor-address/which-one** - Returns the address and current temperatures of the specified DS18B20 sensor connected to the selected hub. Since all API results are less than 100 characters due to LoRa WAN message length limitations, the entire list of all detected DS18B20 sensors via API call isn't possible. First make a call to **/hub-id/sensor-count** to get the total number of sensors, then call this function with any number from 1 to the total number of detected sensors. This can aid in Climate Czar Server configuration without the need to connect a laptop to the hub and using serial configuration menu option #4.

**/hub-id/sensor-count** - Returns the total number of DS18B20 temperature sensors connected to the selected hub.

**/hub-id/switch/which-one** - Returns the on/off state of a specified switch port at the selected hub. **"which-one"** is a value of 1 to 8.

**/hub-id/temperature/format** - Returns the temperature detected at the selected hub in the specified format "c" or "f".

**/hub-id/uptime** - Returns the device uptime at the selected hub.

**/hub-id/wifi-stats** - Returns the device WiFi stats of the selected hub, returns the channel number and signal level.
