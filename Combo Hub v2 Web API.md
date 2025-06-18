# Combo Hub v2 Web API

As of the 100% rewrite of the Climate Czar Combo Hub source code, a completely new web API comes with it. The Combo Hub now features a master/slave topology and the first part of the GET request is the ID number of the hub that you want to talk to. Climate Czar Server always talks directly to the master hub, which then relays API calls to slave units over LoRa WAN instead of TCP/IP.

Example Climate Czar Server web API calls:

`curl -s -m 5 --http0.9 http://192.168.1.100/0/humidity` (get humidity from the master hub)<br>
`curl -s -m 5 --http0.9 http://192.168.1.100/1/humidity` (get humidity from the first slave hub)<br>
`curl -s -m 5 --http0.9 http://192.168.1.100/2/humidity` (get humidity from the second slave hub)<br>

`-s` tells curl not to output download progress<br>
`-m 5` tells curl to time out after 5 seconds<br>
`--http0.9` tells curl to use a pre version 1 HTTP call<br>
