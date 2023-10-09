#!/bin/bash

sudo dpkg-reconfigure locales

sudo apt update
sudo apt upgrade -y
sudo apt autoremove -y
sudo apt install -y bc sqlite3 libsqlite3-dev git-core python3-pip lighttpd php php-common php-fpm php-sqlite3
sudo apt clean

sudo lighttpd-enable-mod fastcgi
sudo lighttpd-enable-mod fastcgi-php

sudo cp -f ./15-fastcgi-php.conf /etc/lighttpd/conf-available/15-fastcgi-php.conf

sudo cp -f ./cz* /var/www/html
sudo cp -f ./db-reset /var/www/html
sudo cp -f ./run-stove /var/www/html
sudo cp -f ./stove-off /var/www/html
sudo cp -f ./*.py /var/www/html
sudo cp -f ./*.php /var/www/html
cd /var/www/html
./db-reset
cd -

sudo chown -R www-data:www-data /var/www/html
sudo chmod g+w -R /var/www/html

sudo service lighttpd force-reload

wget https://project-downloads.drogon.net/wiringpi-latest.deb
sudo dpkg -i wiringpi-latest.deb

sudo pip3 install PyMLX90614

echo
echo
echo "Installation is now complete, but you still need to create the CRON job that"
echo "runs the undercarriage of the system."
echo
echo "Run 'sudo crontab -e' and paste the line of text below into the editor & save."
echo
echo "* * * * * /var/www/html/cz-cron"
echo
echo "The CRON job shown in the above is required to start/stop the pellet stove"
echo "and read its body temperature."
echo
echo "You will also need to run 'sudo raspi-config' and go to Interface Options to"
echo "enable I2C support. This will require your Raspberry PI to be rebooted."
echo
