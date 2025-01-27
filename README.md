# Climate Czar
Universal remote climate monitoring and management system for greenhouses and indoor grow operations.

Live demo at http://panhandleponics.ddns.net:1966/climateczar/

Ordering and purchasing website https://3dgtower.com<br>
You may also contact me directly at https://panhandleponics.com<br>
Subscribe to the official YouTube channel at https://www.youtube.com/@PanhandlePonics<br>

_**NOTE:** Climate Czar is currently being ported to a stand-alone PHP web app in order to reduce its system resource footprint. While it is already possible to use a Raspberry Pi (or clone) as the server, this will make it run smoother on small low-powered single board computers by eliminating the Wordpress dependency._

---

Climate Czar is a Linux and Wordpress based system that gathers readings from any type of sensor or switch that can be read from the command line, whether it is a local device connected to a GPIO bus or a network device. By the same token, the system will control output switches that can also be toggled from the command line. There are no brand/model hardware dependencies, you can even use it with your own custom IoT devices.

Most smart home devices are controllable from the command line of your computer over your network. For example, you can download a utility for TP-Link/Kasa devices from https://github.com/python-kasa/python-kasa for this or you can write scripts for this that use standard command line tools in Linux and MacOS. There are many other brands with this capability as well.

Virtual input sensors/switches can be created that return the average of multiple variable value sensors (temperature, humidity, etc) as well as AND/OR/NOT logic gates based on multiple binary switch type input sensors.

Enabling of output switches may be over-ridden by the status of another output switch. For example, an output switch that controls exhaust fans can be disabled if another switch running a heater is turned on.

Output switches really don't even have to physically exist. Since their operation is controlled with on/off bash scripts, these scripts can also be used to send email/SMS notifications or even make a curl URL call. Input sensors/switches can be assigned to multiple output switches, so you can easily receive a notification at the same time that a physical output switch is toggled on or off.

The Climate Czar Wordpress plugin is only necessary for adding, configuring, and deleting input sensors and output switches. The undercarriage will continue to run in the background even without the web server running. The Wordpress plugin also provides dashboards and histograms to give you a visual representation of all devices and charts that you can use to fine tune your settings.

All connected devices can be assigned to separate groups which allows you to have separate dashboards for multiple greenhouses or grow operations. There's no limit to the number of attached devices and groups. This system running on a small i5 based server with 8GB of RAM was tested with 500 virtual devices and still completed all read and write operations within the every-15-seconds processing loop.

This system will run effectively on a Raspberry PI with a LAMP stack, or any web server that supports PHP, MySQL, and bash scripts. The Climate Czar server itself could actually monitor and control multiple greenhouses and grow operations in different locations if network enabled sensors and switches are utilized throughout (you'd be surprised by what you can accomplish with a public facing Nginx server running as a reverse web proxy).

### Input Device Types
1.	Variable Value Sensor (Temperature, Humidity, pH, EC, AD Convertor, Etc)
2.	Average From a Group of Variable Value Sensors
3.	Remote Switch (Float Switch, Door/Window Switch, Photocell Switch, Etc)
4.	AND Gate of a Group of Remote Switches
5.	OR Gate of a Group of Remote Switches
6.	NOT Gate of a Group of Remote Switches
7.	Scheduler (unlimited on/off schedules per day)
8.	Web Console Switch (Only Controllable Via Web UI)
9.	Raw Text Output Device (only for displaying text, cannot be used as a controller)

The virtual input devices are where the Climate Czar really shines because they allow you to consider multiple sample points before taking action. For example, having temperature sensors on the east and west ends of a greenhouse. Each one of those could be reporting significantly different temperatures at the same time. It makes more sense to turn on heating or cooling based on the average temperature.

Climate Czar is not region specific. If you have temperature sensors that only return values in Celsius and you want to work in Fahrenheit, you just convert the return value in your read script (command line utilities like bc allow you to do floating point math in bash). Climate Czar merely looks at numbers and performs actions based on math, no matter what unit of measurement is used.

WiFi controlled AC outlets such as TpLink/Kasa devices have a manual on/off button on them that could allow a person to turn them on when Climate Czar says they should be off. Output switches have a One Shot option that tells the system whether to only execute the on/off script one time, or to execute the script once every minute in order to maintain the intended on/off state.

With the user interface being based on Wordpress, creating custom dashboards is as simple as creating your own pages or posts and inserting shortcodes with ID number of the input sensor or output switch passed to it. You can even similarly embed these live device views in an external website if you wish by making calls to specific PHP files in the plugin folder the same way.

Anybody who has Wordpress experience can 100% customize the look and feel of your system in order to tailor it to your operation. Everything is built on a Bootstrap based Wordpress theme, so it's real easy to over-ride colors and styles. The pages of the default system are built and rendered using the WP Bakery Page Builder plugin, but you could easily replace that with your own favorite editor instead.

Even though bash scripts are the most common in Linux, any other kind of script or even a compiled binary may be used. All that matters is that the proper command shell or interpreter is installed on the system and the script is owned by the user that the web server runs as and has the executable bit set. Device read scripts simply need to output a single integer or floating point numeric value to stdout.

While this system was designed and tested in a 51x18 foot greenhouse, there are probably many other uses for it besides greenhouses and indoor grow operations. If you need to read numeric values from sensors or switches and turn other devices on and off based on those values, then it's likely that this system will work for what you're needing to do. It was intentionally left completely open ended for this very reason.

![CZ1](https://user-images.githubusercontent.com/121518798/215337977-8dce3d9a-6dbe-4bbb-80d5-a0ea44c87858.png)
![CZ2](https://user-images.githubusercontent.com/121518798/214509996-b0c7db6d-8707-4546-aba5-0d0977ae30f6.png)
![CZ3](https://user-images.githubusercontent.com/121518798/214509998-49c52385-33d1-4e0b-bd5d-21f7252e0ac0.png)
![CZ4](https://user-images.githubusercontent.com/121518798/214510002-5ea0cbb7-f254-4674-a16c-913e79add1c7.png)
![CZ5](https://user-images.githubusercontent.com/121518798/214510005-05b1e945-b3d6-44c2-aa6c-2df3dcc4ed1c.png)
![CZ6](https://user-images.githubusercontent.com/121518798/214510006-e0c5b1fd-0a17-4c4a-9841-76b6e4b12442.png)
