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

**/wifi-stats** - Returns the current WiFi stats of the controller, returns the channel number and signal level.
