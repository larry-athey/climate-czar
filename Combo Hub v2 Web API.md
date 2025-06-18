# Combo Hub v2 Web API

As of the 100% rewrite of the Climate Czar Combo Hub source code, a completely new web API comes with it. The Combo Hub now features a master/slave topology and the first part of the GET request is the ID number of the hub. Climate Czar Server always talks directly to the master hub and relays API calls to slave units over LoRa WAN instead of TCP/IP.
