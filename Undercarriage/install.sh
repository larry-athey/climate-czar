#!/bin/bash

clear
echo "  ____ _ _                 _          ____               "
echo " / ___| (_)_ __ ___   __ _| |_ ___   / ___|______ _ _ __ "
echo "| |   | | | '_ \` _ \\ / _\` | __/ _ \\ | |   |_  / _\` | '__|"
echo "| |___| | | | | | | | (_| | ||  __/ | |___ / / (_| | |   "
echo " \\____|_|_|_| |_| |_|\\__,_|\\__\\___|  \\____/___\\__,_|_|   "
echo
echo "THIS INSTALLER EXPECTS YOU TO BE USING A RAW UNMODIFIED OS INSTALLATION!"
echo
read -p "Press ENTER to continue the installation or CTRL+C to cancel..." nothing
echo

apt=$(which apt)
if [ "$apt" != "/usr/bin/apt" ]; then
  echo "Climate Czar Server requires a Debian derivative Linux operating system."
  exit 1
fi

sudo dpkg-reconfigure locales

sudo systemctl mask sleep.target suspend.target hibernate.target hybrid-sleep.target
sudo apt update
sudo apt install -y lighttpd php php-common php-fpm php-mysql mariadb-server mariadb-client bc netcat-traditional
sudo apt --fix-broken install -y
sudo apt autoremove -y

sudo systemctl enable lighttpd.service
sudo systemctl start lighttpd.service
sudo lighttpd-enable-mod fastcgi
sudo lighttpd-enable-mod fastcgi-php
PHPversion=$(php --version | sed -n 's/^PHP \([0-9]\+\.[0-9]\+\).*/\1/p')
sed -i "s/7.4/$PHPversion/g" ./15-fastcgi-php.conf
sudo cp -f 15-fastcgi-php.conf /etc/lighttpd/conf-available/15-fastcgi-php.conf
sudo chown -R www-data:www-data /var/log/lighttpd
sudo systemctl restart lighttpd.service

sudo mkdir -p /var/www/html/cz/commands
sudo cp -f *.ico /var/www/html/cz/
sudo cp -f *.gz /var/www/html/cz/
sudo cp -f *.php /var/www/html/cz/
sudo cp -f commands/* /var/www/html/cz/commands/
cd /var/www/html/cz
sudo tar -xzvf bootstrap.tar.gz
sudo tar -xzvf css.tar.gz
sudo tar -xzvf js.tar.gz
sudo tar -xzvf monaco-editor.tar.gz
sudo rm -f *.gz
cd -

ln -s /var/www/html/cz/ ~/cz

sudo systemctl enable mariadb > /dev/null 2>&1
sudo systemctl start mariadb > /dev/null 2>&1

clear
echo "  ____ _ _                 _          ____               "
echo " / ___| (_)_ __ ___   __ _| |_ ___   / ___|______ _ _ __ "
echo "| |   | | | '_ \` _ \\ / _\` | __/ _ \\ | |   |_  / _\` | '__|"
echo "| |___| | | | | | | | (_| | ||  __/ | |___ / / (_| | |   "
echo " \\____|_|_|_| |_| |_|\\__,_|\\__\\___|  \\____/___\\__,_|_|   "
echo
echo "Time to secure the MySQL server, you will want to answer Yes to all questions"
echo "EXCEPT for the one about using a Unix socket for authentication. Just be sure"
echo "to set the root password to one that you can remember, simple is fine."
echo

sudo mysql_secure_installation
sudo mysql < db-setup.sql

clear
echo "  ____ _ _                 _          ____               "
echo " / ___| (_)_ __ ___   __ _| |_ ___   / ___|______ _ _ __ "
echo "| |   | | | '_ \` _ \\ / _\` | __/ _ \\ | |   |_  / _\` | '__|"
echo "| |___| | | | | | | | (_| | ||  __/ | |___ / / (_| | |   "
echo " \\____|_|_|_| |_| |_|\\__,_|\\__\\___|  \\____/___\\__,_|_|   "
echo
echo "Now installing phpMyAdmin, be sure to select the lighttpd configuration!"
echo
read -p "Press ENTER to continue..." nothing

sudo apt install -y phpmyadmin
sudo apt purge -y apache2
sudo service lighttpd force-reload
sudo apt clean
sudo chown -R www-data:www-data /var/www/html
sudo chmod g+w -R /var/www/html

clear
echo "  ____ _ _                 _          ____               "
echo " / ___| (_)_ __ ___   __ _| |_ ___   / ___|______ _ _ __ "
echo "| |   | | | '_ \` _ \\ / _\` | __/ _ \\ | |   |_  / _\` | '__|"
echo "| |___| | | | | | | | (_| | ||  __/ | |___ / / (_| | |   "
echo " \\____|_|_|_| |_| |_|\\__,_|\\__\\___|  \\____/___\\__,_|_|   "
echo
echo "Installation is now complete, but you still need to create the CRON job that"
echo "runs the undercarriage of the system."
echo
echo "Run 'sudo crontab -e' and paste the line of text below into the editor & save."
echo
echo "* * * * * /var/www/html/cz/commands/cz-cron"
echo
echo "The CRON job shown in the above fires off every minute to verify that both of"
echo "the process scripts are running for input and output processing."
echo
echo
