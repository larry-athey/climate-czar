# climate-czar
Universal remote climate monitoring and control system for greenhouses and indoor grow operations.

Climate Czar is a Linux and Wordpress based system that gathers readings from any type of sensor or switch that can be read with a bash script, whether it is a local device connected to a GPIO bus or a network device. By the same token, the system will control remote switches that can also be toggled with a bash script. There are no brand/model hardware dependencies.

Output switches really don't even have to physically exist. Since their operation is controlled with on/off bash scripts, these scripts can also be used to send email/SMS notifications or even make a curl URL call. Input sensors/switches can be assigned to multiple output switches, so you can easily receive a notification any time that a physical output switch is toggled on and off.

This system will run effectively on a Raspberry PI with a LAMP stack and anything else up to a high performance virtual private server.
