## Climate Czar Installation

This guide expects you to have a fully functioning PHP enabled web server and MySQL database server running on a Linux, Unix, or Mac OS machine. This system absolutely is not compatible with Windows and never will be. If you absolutely have to use Windows, you will need to install a Linux or Unix virtual machine and then install Climate Czar inside of it.

This guide doesn't cover the installation and configuration of your web server and database server since there are already so many guides for this online that go into great detail. This system has been tested with Apache, Nginx, and Lighttpd on both Intel and Raspberry PI hardware. There are umpteen guides on installing the LAMP stack in Linux, any one of those should be all you need to be installation-ready.

There is no need to dedicate the entire web server to Climate Czar, it installs itself into **/climateczar/** by default so that it is kept separate from everything else on your server. Unless you have set up your web server in a nonstandard way, this will ultimately be **/var/www/html/climateczar/**.

If you already have Wordpress installed on your web server and it uses the default database name of "Wordpress", you will need to change that or edit **Wordpress.sql** and the variables in **install.sh** files so they point to a different database name. If you fail to do that, your existing Wordpress website content will be overwritten.

Below are the only two variables in **install.sh** that you will need to edit before installation.

**Protocol** - If you have not enabled HTTPS in your web server, it's best to leave this as-is. You need to have a valid domain name pointing to your web server before you can install an SSL certificate from a certificate authority. If you are just going to access this system by IP address and want SSL, you will only be able to use a self-signed certificate which will trigger a warning in all web browsers.

**MyHost** - Your server's host name or IP address and any :PORT declaration that may be applicable. This is hard coded in the Wordpress database, so if you use port forwarding in your router, this needs to be your public IP address.

Before running the installation, you will want to confirm that your server is configured in your preferred time zone. For example, my time zone is **America/Denver** but Linux distributions like to set themselves to **UTC** and this doesn't play well with bash scripts if you want them to do things according to your local time zone. See the following page if you are unsure how to set your server's time zone.

<https://linuxize.com/post/how-to-set-or-change-timezone-in-linux/>

After you have confirmed that your server is now set to the correct time zone, execute the following command while in the same directory that you extracted the climate-czar-\*.zip archive to.

```
sudo ./install.sh
```

After the installation completes, you will be presented with a root CRON job on your screen that you need to add.

You will also be presented with the URL to log in to the Wordpress admin console. The default login credentials are as follows. You will definitely want to go to the **Users section** and change the password if it's possible that anybody else can access your system.

Username and Password: czwpadmin *(no, this won't get you into my system)*

There are two steps that you need to complete that I frankly can't figure out how to retain the settings in a Wordpress backup. I've even tested this with Updraft Backup and it has the same problem, so I guess this is just a Wordpress oddity that is even a mystery to backup specialists.

Go to **Settings-Permalinks** and click the Save button, even though you're not making any changes.

Then go to **Appearance->Customize->Other Customizations** and click on Select Color under Background Color. Paste this color code **#7fca90** in the field next to that button and then click on the Publish button at the top of the screen.

That's all there is to it, so long as your web server has a working rewrite engine *(which is how Wordpress creates multiple pages that really don't physically exist as HTML files)* then your system is ready to use. You will probably want to reboot your server just for good measure and to make sure that everything still works after a reboot.