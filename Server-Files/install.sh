#!/bin/bash

if [ "$EUID" -ne 0 ]; then
  echo "Must be run as root"
  echo -en "\007"
  exit
fi

Protocol="http"
# ^^^--- If you have not enabled HTTPS in your web server, it's best to leave this
# as-is. You need to have a valid domain name pointing to your web server before
# you can install an SSL certificate from a certificate authority. If you are just
# going to access this system by IP address and want SSL, you will only be able to
# use a self-signed certificate which will trigger a warning in all web browsers.

MyHost="10.20.30.149"
# ^^^--- Your server's host name or IP address and any :PORT declaration that may
# be applicable. This is hard coded in the Wordpress database, so if you use port
# forwarding in your router, this needs to be your public IP address.

CronJob="* * * * * /var/www/html/climateczar/commands/cz-cron"
# ^^^--- Do not make any changes to this or you will need to edit all three of the
# /climateczar/commands/cz-* bash scripts to use your new installation path.

mkdir -p /var/www/html
# ^^^--- This is only here in case this machine uses a nonstandard web server setup.
# If so, simply add the /climateczar/ directory as a location to your configuation.

OldProto="http"
OldHost="panhandleponics.ddns.net:1966"
sed -i "s/$OldProto:/$Protocol:/g" Wordpress.sql
sed -i "s/$OldHost/$MyHost/g" Wordpress.sql

echo "Importing: dbsetup.sql"
mysql < dbsetup.sql
echo "Importing: ClimateCzar.sql"
mysql < ClimateCzar.sql
echo "Importing: Wordpress.sql"
mysql < Wordpress.sql

echo
echo "Extracting climateczar.tar.gz into /var/www/html/"
cp -f climateczar.tar.gz /var/www/html/
cd /var/www/html
tar -xzvf climateczar.tar.gz
rm -f climateczar.tar.gz
rm -f /var/www/html/climateczar/wp-content/plugins/climate-czar/CZ_TZ.php
chown -R www-data:www-data /var/www/html/climateczar
chmod g+w -R /var/www/html/climateczar
chmod +x /var/www/html/climateczar/commands/*

dpkg-reconfigure locales

apt update
apt upgrade -y
apt autoremove -y
apt install -y bc netcat
apt --fix-broken install -y
apt clean

echo -en "\007"
echo
echo
echo "Installation is now complete, but you still need to create the CRON job that"
echo "runs the undercarriage of the system."
echo
echo "Run 'sudo crontab -e' and paste the line of text below into the editor & save."
echo
echo "$CronJob"
echo
echo
echo "After that, go to $Protocol://$MyHost/climateczar/wp-admin and log in with the"
echo "credentials contained in Installation.md and complete the final setup steps."
echo
