## Climate Czar Installation

This guide expects you to have a fully functioning PHP enabled web server and MySQL database server running on a Linux, Unix, or Mac OS machine. This system absolutely is not compatible with Windows and never will be. If you absolutely have to use Windows, you will need to install a Linux or Unix virtual machine and then install Climate Czar inside of it.

This guide doesn't cover the installation and configuration of your web server and database server since there are already so many guides for this online that go into great detail. This system has been tested with Apache, Nginx, and Lighttpd on both Intel and Raspberry PI hardware. There are umpteen guides on installing the LAMP stack in Linux, any one of those should be all you need to be installation-ready.

There is no need to dedicate the entire web server to Climate Czar, it installs itself into **/climateczar/** by default so that it is kept separate from everything else on your server. Unless you have set up your web server in a nonstandard way, this will ultimately be **/var/www/html/climateczar/**.

If you already have Wordpress installed on your web server and it uses the default database name of "Wordpress", you will need to change that or edit **Wordpress.sql** and the variables in **install.sh** files so they point to a different database name. If you fail to do that, your existing Wordpress website content will be overwritten.

Below are the variables in **install.sh** that you will need to edit before installation.
