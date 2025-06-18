# Combo Hub v2 Web API

As of the 100% rewrite of the Climate Czar Combo Hub source code, a completely new web API comes with it. The Combo Hub now features a master/slave topology and the first part of the GET request is the ID number of the hub that you want to talk to. Climate Czar Server always talks directly to the master hub, which then relays API calls to slave units over LoRa WAN instead of TCP/IP.

Example Climate Czar Server web API calls:

`curl -s -m 5 --http0.9 http://master-ip-address/0/humidity` (get humidity from the master hub)<br>
`curl -s -m 5 --http0.9 http://master-ip-address/1/humidity` (get humidity from the first slave hub)<br>
`curl -s -m 5 --http0.9 http://master-ip-address/2/humidity` (get humidity from the second slave hub)<br>

`-s` tells curl run silently and not output download progress<br>
`-m 5` tells curl to time out the connection attempt after 5 seconds<br>
`--http0.9` tells curl to use a pre version 1.0 HTTP call for compatibility safety<br>

Below are explanations of all available web API calls. All of these calls are case sensitive, there is no AI at work here to decide if you're just "close enough". Breaking the rules will result in a JSON formatted error message being returned.

**/hub-id/device-name** - Returns the device name of the selected hub.

**/hub-id/ds18b20/address/format** - Returns the temperature of a DS18B20 temperature sensor connected to the selected hub in the specified format "c" or "f". Sensor identification is done using option 4 in the serial configuration main menu.

**/hub-id/humidity** - Returns the humidity level detected at the selected hub.
