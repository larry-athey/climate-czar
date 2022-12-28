# Climate Czar
Universal remote climate monitoring and control system for greenhouses and indoor grow operations.


Climate Czar is a Linux and Wordpress based system that gathers readings from any type of sensor or switch that can be read with a bash script, whether it is a local device connected to a GPIO bus or a network device. By the same token, the system will control output switches that can also be toggled with a bash script. There are no brand/model hardware dependencies, you can even use it with your own custom IoT devices.

Virtual input sensors/switches can be created that return the average of multiple variable value sensors (temperature, humidity, etc) as well as AND/OR/NOT logic gates based on multiple binary switch type input sensors.

Enabling of output switches may be over-ridden by the status of another output switch. For example, an output switch that controls exhaust fans can be disabled if another switch running a heater is turned on.

Output switches really don't even have to physically exist. Since their operation is controlled with on/off bash scripts, these scripts can also be used to send email/SMS notifications or even make a curl URL call. Input sensors/switches can be assigned to multiple output switches, so you can easily receive a notification at the same time that a physical output switch is toggled on or off.

The Climate Czar Wordpress plugin is only necessary for adding, configuring, and deleting input devices and output switches. The undercarriage will continue to run in the background even without the web server even running. The plugin also provides dashboards and histograms to give you a visual representation of all devices and charts that you can use to fine tune your settings.

This system will run effectively on a Raspberry PI with a LAMP stack, or any web server that supports PHP, MySQL, and bash scripts. The Climate Czar server itself could actually monitor and control multiple greenhouses and grow operations in different locations if network enabled sensors and switches are utilized throughout (you'd be surprised by what you can accomplish with a public facing Nginx server running as a reverse web proxy).

# Input Device Types
1.	Variable Value Sensor (Temperature, Humidity, pH, EC, AD Convertor, Etc)
2.	Average From a Group of Variable Value Sensors
3.	Remote Switch (Float Switch, Door/Window Switch, Photocell Switch, Etc)
4.	AND Gate of a Group of Remote Switches
5.	OR Gate of a Group of Remote Switches
6.	NOT Gate of a Group of Remote Switches
7.	Timer (up to 24 on/off schedules per day or a repeating cycle of on/off intervals)
8.	Web Console Switch (Only Controllable Via Web UI)
9.	Raw Text Device (only for displaying text, cannot be used as a controller)

The virtual input devices are where the Climate Czar really shines because they allow you to consider multiple sample points before taking action. For example, having temperature sensors on the east and west ends of a greenhouse. Each one of those could be reporting significantly different temperatures at the same time. It makes more sense to turn on heating or cooling based on the average temperature.

Climate Czar is not region specific. If you have temperature sensors that only return values in Celsius and you want to work in Fahrenheit, you just convert the return value in your read script (command line utilities like bc allow you to do floating point math in bash). Climate Czar merely looks at numbers and performs actions based on math, no matter what unit of measurement is used.

Some WiFi controlled AC outlets such as TpLink/Kasa devices have a manual on/off button on them that could allow a person to turn them on when Climate Czar says they should be off. Output switches have a One Shot option that tells the system whether to only execute the on/off script one time, or to execute the script once every minute in order to maintain the intended on/off state.

Even though bash scripts are the most common in Linux, any other kind of script or even a compiled binary may be used. All that matters is that proper command shell or interpreter is installed on the system and the script is owned by the user that the web server runs as and has the executable bit set. Read scripts simply need to output a single integer or floating point numeric value to stdout.

While this system was designed and tested in a 51x18 foot greenhouse, there are probably many other uses for it besides greenhouses and indoor grow operations. If you need to read numeric values from sensors or switches and turn other devices on and off based on those values, then it's likely that this system will work for what you're needing to do. It was intentionally left completely open ended for this very reason.
