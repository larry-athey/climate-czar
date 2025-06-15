# Climate Czar
Universal remote climate monitoring and management system for greenhouses and indoor grow operations. No hardware brand dependencies, if you can read or control a sensor or smart switch from the command line, then it's compatible with Climate Czar.

Live demo at https://greenhouse.panhandleponics.com/cz/?CZ_THEME=1

You may also contact me directly at https://panhandleponics.com<br>
Subscribe to the official YouTube channel at https://www.youtube.com/@PanhandlePonics<br>

![image](https://github.com/user-attachments/assets/7b411953-9304-4b04-ab87-2358f437c058)
---

Climate Czar is a Linux based system that gathers readings from any type of sensor or switch that can be read from the command line, whether it is a local device connected to a GPIO bus or a network device. By the same token, the system will control output switches that can also be toggled from the command line. There are no brand/model hardware dependencies, you can even use it with your own custom IoT devices.

Most smart home devices are controllable from the command line of your computer over your network. For example, you can download a utility for TP-Link/Kasa devices from https://github.com/python-kasa/python-kasa for this or you can write scripts for this that use standard command line tools in Linux and MacOS. There are many other brands with this capability as well.

Virtual input sensors/switches can be created that return the average of multiple variable value sensors _(temperature, humidity, etc)_ as well as AND/OR/NOT logic gates based on multiple binary switch type input sensors.

Enabling of output switches may be over-ridden by the status of another output switch. For example, an output switch that controls exhaust fans can be disabled if another switch running a heater is turned on. However, you can manually over-ride this if need be.

Output switches really don't even have to physically exist. Since their operation is controlled with on/off bash scripts, these scripts can also be used to send email/SMS notifications or even make a curl URL call. Input sensors/switches can be assigned to multiple output switches, so you can easily receive a notification at the same time that a physical output switch is toggled on or off.

All connected devices can be assigned to separate groups which allows you to have separate dashboards for multiple greenhouses or grow operations. There's no limit to the number of attached devices and groups. This system running on an Orange Pi 3B with 8GB of RAM was tested with 500 virtual devices and still completed all read and write operations within the every-15-seconds processing loop.

This system will run effectively on a Raspberry PI or clone, or any Linux powered web server that supports PHP and MySQL. The Climate Czar server itself could actually monitor and control multiple greenhouses and grow operations in different locations if network enabled sensors and switches are utilized throughout with an on-site Raspberry Pi running an Nginx reverse proxy.

### Input Device Types
1.	Variable Value Sensor (Temperature, Humidity, pH, EC, AD Convertor, etc)
2.	Average From a Group of Variable Value Sensors
3.	Remote Switch (Float Switch, Door/Window Switch, Photocell Switch, etc)
4.	AND Gate of a Group of Remote Switches
5.	OR Gate of a Group of Remote Switches
6.	NOT Gate of a Group of Remote Switches
7.	Scheduler (unlimited on/off schedules per day)
8.	Web Console Switch (Only controllable via web UI)
9.	Raw Text Output Device (only for displaying text, cannot be used as a controller)

The virtual input devices are where the Climate Czar really shines because they allow you to consider multiple sample points before taking action. For example, having temperature sensors on the east and west ends of a greenhouse. Each one of those could be reporting significantly different temperatures at the same time. It makes more sense to turn on heating or cooling based on the average temperature.

Climate Czar is not region specific. If you have temperature sensors that only return values in Celsius and you want to work in Fahrenheit, you just convert the return value in your read script _(command line utilities like bc allow you to do floating point math in bash)_. Climate Czar merely looks at numbers and performs actions based on math, no matter what unit of measurement is used.

WiFi controlled AC outlets such as TpLink/Kasa devices have a manual on/off button on them that could allow a person to turn them on when Climate Czar says they should be off. Output switches have a One Shot option that tells the system whether to only execute the on/off script one time, or to execute the script once every minute in order to maintain the intended on/off state.

While this system was designed and tested in a 51x18 foot greenhouse, there are probably many other uses for it besides greenhouses and indoor grow operations. If you need to read numeric values from sensors or switches and turn other devices on and off based on those values, then it's likely that this system will work for what you're needing to do. It was intentionally left completely open ended for this very reason.

Again, there is no question as to whether Climate Czar is compatible with your devices. The only question is if your devices can be read or controlled from the command line. If so, then they'll work with Climate Czar.

![image](https://github.com/user-attachments/assets/1ff35e7c-dc2b-4c1f-952a-eef99d6dbb8f)

![image](https://github.com/user-attachments/assets/fd390f94-8900-4b51-87f2-3d0caf7e173d)

![image](https://github.com/user-attachments/assets/1b728bad-f35d-4a7d-b292-7a1e50fda736)

![image](https://github.com/user-attachments/assets/3beab54c-0471-407b-a7ee-4b12bb0824fa)

![image](https://github.com/user-attachments/assets/eb33ef97-4ec6-4fda-8f36-16467c0c8961)

![image](https://github.com/user-attachments/assets/54d89655-f687-48da-a88c-eaa126cc90d7)
