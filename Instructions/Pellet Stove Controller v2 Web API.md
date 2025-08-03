# Pellet Stove Controller v2 Web API

As of the 100% rewrite of the Climate Czar Pellet Stove Controller source code, a completely new web API comes with it. This version of the controller now uses the same 100% GET web API that is featured in the new version of the Climate Czar Combo Hub.

Example Climate Czar Server web API calls:

`curl -s -m 5 --http0.9 http://controller-ip-address/startup` (start up the pellet stove)<br>
`curl -s -m 5 --http0.9 http://controller-ip-address/temp-mode/f` (set the temperature mode to Fahrenheit)<br>
`curl -s -m 5 --http0.9 http://controller-ip-address/thermostat/70.5` (set the built-in thermostat to 70.5F)<br>

`-s` tells curl run silently and not output download progress<br>
`-m 5` tells curl to time out the connection attempt after 5 seconds<br>
`--http0.9` tells curl to use a pre version 1.0 HTTP call for compatibility safety<br>

---

**/reset** - Performs a reset of the controller startup/shutdown state, does not affect settings saved in flash memory.

**/room-blower/0** or **/room-blower/1** - Turns the room blower on or off, while the controller is not performing an active run.

**/uptime** - Returns the current room temperature detected by the internal thermostat.

**/uptime** - Returns the current stove runtime.

**/shutdown** - Shut down the stove or cancel a start up. Shutdown time is 2x the startup time.

**/startup** - Start up the stove or cancel a shut-down, so long as the stove isn't in a thermal failure or fault state.

**/startup-timer/????** - Sets the number of seconds that the stove is allowed to reach its minimum operating temperature.

**/stove-temp** - Returns the current stove body temperature.

**/temp-mode/c** or **/temp-mode/f** - Sets the controller temperature mode to Celcius or Fahrenheit.

**/therm-temp/??.?** - Sets the target temperature of the internal thermostat (C or F depending on **temp-mode**).

**/thermostat/0** or **/thermostat/1** - Enables or disables the internal DS18B20 based thermostat.

**/toggle-run** - Toggles the current run state, this is used by the web UI to start and stop the controller.

**/top-auger/0** or **/top-auger/1** - Turns the top auger motor on or off, while the controller is not performing an active run.

**/uptime** - Returns the current device uptime.

**/wifi-stats** - Returns the current WiFi stats of the controller, returns the channel number and signal level.
