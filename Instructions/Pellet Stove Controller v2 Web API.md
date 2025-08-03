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

**/bottom-auger/0** or **/bottom-auger/1** - Turns the bottom auger motor on or off, while the controller is not performing an active run.

**/burn-mode/0** or **/burn-mode/1** - Sets the stove burn mode during an active run (0=idle, 1=high).

**/combustion/0** or **/combustion/1** - Turns the combustion blower on or off, while the controller is not performing an active run.

**/cz-stats** - Returns an HTML block with Bootstrap classes for use with Climate Czar Server in a Raw Text Output Device.

**/feed-high/??.?** - Sets the top auger feed time (in seconds) when the stove is in high burn mode.

**/feed-low/??.?** - Sets the top auger feed time (in seconds) when the stove is in idle burn mode.

**/ignitor/0** or **/ignitor/1** - Turns the ignitor on or off, while the controller is not performing an active run.

**/json-stats** - Returns a JSON object containing the stove's current operational stats for use with third-party software.

**/max-temp/??.?** - Sets the stove maximum operating temperature (C or F depending on **temp-mode**).

**/min-temp/??.?** - Sets the stove minimum operating temperature (C or F depending on **temp-mode**).

**/op-mode** - Returns the current operation mode. 0=Off, 1=Startup, 2=Running, 3=TempFail, 4=Shutdown, 5=Fault

**/reboot** - Reboots the controller, the previous run state is resumed.

**/reset** - Performs a reset of the controller startup/shutdown state, does not affect settings saved in flash memory.

**/room-blower/0** or **/room-blower/1** - Turns the room blower on or off, while the controller is not performing an active run.

**/room-temp** - Returns the current room temperature detected by the internal thermostat.

**/runtime** - Returns the current stove runtime.

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
