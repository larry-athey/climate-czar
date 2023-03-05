# Frequently Asked Questions

Q: Why create this when there are already so many other home automation systems that could run a greenhouse?

A: They're all overly complicated and they just continue to get even more complicated with every release. I would rather have something where the actual software is simplified. If anything complicated needs to be done, let the commands/scripts handle it so all that overhead isn't complicating the rest of the software. "Keep it stupid simple" has always been the way I write software.

Q: Can you recommend any specific brands of network sensors or switches/outlets to use?

A: No, because it's not in my budget to frivolously buy a bunch of different devices just for testing. Personally, the only third party devices that I use are TP-Link/Kasa WiFi controlled outlets. Other than that, any sensors I use are various ones used in Raspberry PI or Arduino tutorials. These are actually kids' STEM class projects, so they're certainly not that hard to build.

Q: Why do the device read/write loops run on 15 second intervals?

A: In my original static built version, everything was done in one long loop. The more routines I crammed into it, the longer it took to get through the loop. Splitting things into two separate loops shortens the time to get through everything and prevents pegging the CPU at 100%. The 15 second delay between cycles relieves even more CPU stress.

Q: Can't this 15 second delay between each read and write process cause problems?

A: This system is only intended to be used in greenhouses, indoor grow operations, basic home automation, etc. This is not intended to be used in any setting where timing has to be precise down to the second or less. If your greenhouse temperature finally hits the point where exhaust fans need to be turned on, a 15 second delay between detection time and action certainly isn't going to kill your plants. All home automation systems have delays between detection and action and some perform the action before displaying the reading in order to trick you.

Q: My power went out for a few seconds and when it came back, my Combo Hub won't respond.

A: It's pretty tough to destroy the ESP32 with a power outage, it takes a pretty serious surge to do it since I include a switching power supply with them rather than a linear power supply. So, I'd really suggest restarting your router or access point, even if other devices are connected just fine. I've been able to reliably reproduce this with both a Netgear and Asus router, the best I can figure is that the router didn't completely discharge and the ARP table got botched. The cure is to unplug the router or access point and give it time for all capacitors to discharge, a minute should work. Then plug it back in and power cycle your Combo Hub after that.

Q: Can Climate Czar monitor things like pH and EC in a hydroponics system?

A: Climate Czar can monitor anything so long as you have a sensor that can output a variable value or a binary value. There are Arduino projects/kits for both pH and EC all over the web, it's just a matter of connecting the output of them to one of the 8 inputs of the Combo Hub and pulling those values into Climate Czar with a read command.

Q: Why isn't the Wordpress plugin available from the Wordpress plugin store/repository?

A: Because the Climate Czar undercarriage is actually not part of Wordpress, which is why it uses its own database separate from the Wordpress database. The plugin would do you no good without the undercarriage installed, so there's no point in going through all of the headaches of playing the formal Wordpress third-party developer game. It's not possible to install the undercarriage with the plugin since that procedure requires root access.

Q: What's the deal with the random warnings in the WP-Admin, such as when you edit a page?

A: This is a side effect of extra code in the theme's function.php to confuse version probes from somebody scanning your installation with WPScan. If you choose to install a Wordpress firewall (such as Wordfence) you can safely remove all of those lines of code. Just go to Appearance->Theme File Editor->functions.php and delete lines 10 through 51. These scans always result in a denial of service attack on your server, screw those people.

Q: What's the deal with all of this 3.3 volt stuff in your Combo Hub instructions? I've never heard of this before.

A: 3.3 volts DC is what most all micro controllers run on. This low voltage and low current is actually much safer than running 120 volt AC wires all over the place because shock and fire hazards are completely eliminated. You could actually get a high current 5 volt power brick to run an entire greenhouse and insert small 3.3 volt regulators (such as https://www.amazon.com/gp/product/B074FDLCLB/) where needed to bring the signal level down to a safe level for the inputs of the Combo Hub. So, while it may sound like a foreign concept to you, it's actually very common and no more complicated to work with.

Q: I'm building a new system with my own custom dashboards, is it safe to have two installations connected to the same greenhouse?

A: Only the input sensors, never have two installations running the same output switches. This would be like running two spam filters or antivirus programs at the same time, there's no way to know which one is causing the problem that you're trying to fix, even if they're both using the exact same settings.

Q: Can your Raspberry PI Pellet Stove Controller be used to replace a defective pellet grill/smoker controller?

A: After getting this question enough times, I have expanded things in order to work for this purpose and the database has slots for up to 10 meat probes. Granted, the room air blower relay doesn't have any use in a grill/smoker, but you can easily use that to run a peristaltic dosing pump on a cycle timer to squirt water on your heat spreader in order to have a steam smoker. The **read.php** API script already supports these probes, so it's just a matter of adding thermistors to an AD convertor and adding code to the **cz-main-loop** script to read them and stuff their readings in the database.

Q: Is it possible to use your Raspberry PI Pellet Stove Controller without the need for Climate Czar?

A: Certainly! The **cz-main-loop** script has two commented out code blocks that you can uncomment if you want to use toggle switches to turn it on/off and switch it back and forth between idle and high burn mode. You could also use the relay and switch in a traditional home thermostat for this. If you want to tweak the settings for the controller, you can do this with curl commands to the **write.php** API script on a Mac, Linux PC, or a Powershell in Windows. No need for Climate Czar at all.
