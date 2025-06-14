# FYI

**This device is currently going through a complete redesign and rewrite.**

The new version will still be ESP32 based, but will now feature ethernet as well as WiFi connectivity as well as master/slave operation. Slave units will connect via RYLR998 LoRa WAN network. This will result in changes to the web API, for example...

http://combo-hub-ip/0/humidity reads the humidity from the master unit<br>
http://combo-hub-ip/1/humidity reads the humidity from the first slave<br>
http://combo-hub-ip/2/humidity reads the humidity from the second slave<br>

All web API calls will now include the unit number as the parent directory of the URL.

Each Combo Hub will feature an attached DHT22 for local temperature and humidity, a Dallas Semiconductor 1-Wire bus for unlimited remote temperature sensors, a BH1750 for local ambient light level detection, 8 digital inputs for binary switches, 8 digital outputs for controlling solid state relays, an SSD1306 128x32 OLED status display, and a Reyax RYLR998 radio module.

3D models will be provided for a case which will be strictly for the Combo Hub brains and connection terminals. Solid state relays will no longer be housed in the same enclosure.

All configuration will be done using a serial terminal program rather than the current method that requires you to set global variables before flashing the ESP32.
