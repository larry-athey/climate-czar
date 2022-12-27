# climate-czar
Universal remote climate monitoring and control system for greenhouses and indoor grow operations.

Climate Czar is a Linux and Wordpress based system that gathers readings from any type of sensor or switch that can be read with a bash script, whether it is a local device connected to a GPIO bus or a network device. By the same token, the system will control output switches that can also be toggled with a bash script. There are no brand/model hardware dependencies, you can even use it with your own custom IoT devices.

Virtual input sensors/switches can be created that output the average of multiple variable value sensors (temperature, humidity, etc) as well as AND/OR/NOT logic gates based on multiple binary switch type input sensors.

Output switches may also be dependent upon the status of another output switch. For example, an output switch that controls exhaust fans can be disabled if a switch running a heater is turned on.

Output switches really don't even have to physically exist. Since their operation is controlled with on/off bash scripts, these scripts can also be used to send email/SMS notifications or even make a curl URL call. Input sensors/switches can be assigned to multiple output switches, so you can easily receive a notification any time that a physical output switch is toggled on and off.

The custom Wordpress plugin is only necessary for adding, configuring, and deleting input devices and output switches. The system will continue to run in the background without the web server even running. The plugin also provides dashboards and histograms to give you a visual representation of all devices and charts that you can use to fine tune your settings.

This system will run effectively on a Raspberry PI with a LAMP stack and anything else up to a high performance virtual private server. Any web server that supports PHP, MySQL, and bash scripts will work. The Climate Czar server itself could actually be in a different location as the physical input sensors and output switches if they are network devices.
