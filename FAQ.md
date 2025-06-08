# Frequently Asked Questions

Q: Why create this when there are already so many other home automation systems that could run a greenhouse?

A: They're all overly complicated and they just continue to get even more complicated with every release. I would rather have something where the actual software is simplified. If anything complicated needs to be done, let the commands/scripts handle it so all that overhead isn't complicating the rest of the software. "Keep it stupid simple" has always been the way I write software.
___
Q: Can you recommend any specific brands of network sensors or switches/outlets to use?

A: No, because it's not in my budget to frivolously buy a bunch of different devices just for testing. Personally, the only third party devices that I use are TP-Link/Kasa WiFi controlled outlets. Other than that, any sensors I use are various ones used in Raspberry PI or Arduino tutorials. These are actually kids' STEM class projects, so they're certainly not that hard to build.
___
Q: Why do the device read/write loops run on 15 second intervals?

A: In my original static built version, everything was done in one long loop. The more routines I crammed into it, the longer it took to get through the loop. Splitting things into two separate loops shortens the time to get through everything and prevents pegging the CPU at 100%. The 15 second delay between cycles relieves even more CPU stress.
___
Q: Can't this 15 second delay between each read and write process cause problems?

A: This system is only intended to be used in greenhouses, indoor grow operations, basic home automation, etc. This is not intended to be used in any setting where timing has to be precise down to the second or less. If your greenhouse temperature finally hits the point where exhaust fans need to be turned on, a 15 second delay between detection time and action certainly isn't going to kill your plants. All home automation systems have delays between detection and action and some perform the action before displaying the reading in order to trick you.
___
Q: My power went out for a few seconds and when it came back, my Combo Hub won't respond.

A: It's pretty tough to destroy the ESP32 with a power outage, it takes a pretty serious surge to do it since I include a switching power supply with them rather than a linear power supply. So, I'd really suggest restarting your router or access point, even if other devices are connected just fine. I've been able to reliably reproduce this with both a Netgear and Asus router, the best I can figure is that the router didn't completely discharge and the ARP table got botched. The cure is to unplug the router or access point and give it time for all capacitors to discharge, a minute should work. Then plug it back in and power cycle your Combo Hub after that.
___
Q: Can Climate Czar monitor things like pH, EC, and DO in a hydroponics system?

A: Climate Czar can monitor anything so long as you have a sensor that can output a variable value or a binary value. There are Arduino projects/kits for both pH and EC all over the web, it's just a matter of connecting the output of them to one of the 8 inputs of the Combo Hub and pulling those values into Climate Czar with a read command.
