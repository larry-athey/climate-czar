# Frequently Asked Questions

Q: Why create this when there are already so many other home automation systems that could run a greenhouse?

A: They're all overly complicated and they just continue to get even more complicated with every release. I would rather have something where the actual software is simplified. If anything complicated needs to be done, let the commands/scripts handle it so all that overhead isn't complicating the rest of the software. "Keep it stupid simple" has always been the way I write software.

Q: Can you recommend any specific brands of network sensors or switches/outlets to use?

A: No, because it's not in my budget to frivolously buy a bunch of different devices just for testing. Personally, the only third party devices that I use are TP-Link/Kasa WiFi controlled outlets. Other than that, any sensors I use are various ones used in Raspberry PI or Arduino tutorials. These are actually kids' STEM class projects, so they're certainly not that hard to build.

Q: Why do the device read/write loops run on 15 second intervals?

A: In my original static built version, everything was done in one long loop. The more routines I crammed into it, the longer it took to get through the loop. Splitting things into two separate loops shortens the time to get through everything and prevents pegging the CPU at 100%. The 15 second delay between cycles relieves even more CPU stress.

Q: What's the deal with all of this 3.3 volt stuff in your Combo Hub? I've never heard of this before.

A: 3.3 volts DC is what most all micro controllers run on. This low voltage and low current is actually much safer than running 120 volt AC wires all over the place because shock and fire hazards are completely eliminated. You could actually get a high current 5 volt power brick to run an entire greenhouse and insert small 3.3 volt regulators (such as https://www.amazon.com/gp/product/B074FDLCLB/) where needed to bring the signal level down to a safe level for the inputs of the Combo Hub. So, while it may sound like a foreign concept to you, it's actually very common and no more complicated to work with.
