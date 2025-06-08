# FYI

**This device is currently going through a complete rewrite.**

The new version will still be ESP32 based, but will now feature ethernet as well as WiFi connectivity as well as master/slave operation. Slave units will connect via RYLR998 LoRa WAN network. This will result in changes to the web API, for example...

http://combo-hub-ip/0/humidity reads the humidity from the master unit<br>
http://combo-hub-ip/1/humidity reads the humidity from the first slave<br>
http://combo-hub-ip/02/humidity reads the humidity from the second slave<br>

All configuration will be done using a serial terminal program rather than the current method that requires you to set global variables before flashing the ESP32.
