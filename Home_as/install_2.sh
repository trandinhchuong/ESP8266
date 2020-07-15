Here are the steps I used to do a fresh install on an Ubuntu 16.04 server. Steps adapted from the HA links in the installation docs.

Install dependencies:

Copy to clipboard
sudo apt update
sudo apt install python3-dev python3-pip
sudo pip3 install --upgrade virtualenv
Create a user and group. Give the user access to serial lines (zwave, insteon, etc)

Copy to clipboard
sudo adduser --system homeassistant
sudo addgroup homeassistant
sudo adduser homeassistant dialout
Create a directory to install HA in and set it’s ownership and permissions.

Copy to clipboard
sudo mkdir /opt/homeassistant
sudo chown homeassistant:homeassistant /opt/homeassistant
Change to the homeassistant user to do the installs.

Copy to clipboard
sudo su -s /bin/bash homeassistant
Install a virtual env to sandbox the HA software and dependencies and activate it so further installs are done here.

Copy to clipboard
cd /opt/homeassistant
python3 -m venv /opt/homeassistant
source bin/activate
Install HA from pip. I got some weird pip install errors when I did this but it didn’t seem to cause any actual problems.

Copy to clipboard
pip3 install --upgrade homeassistant
I keep all the config files in the same directory (rather than having them in /home/homassistant) and I like to have the log file not in the config directory so I run it by calling:

Copy to clipboard
mkdir config
./bin/hass -c /opt/homeassistant/config --log-file /opt/homeassistant/hass.log
Try that - HA should install a few things and make a default config file (let it run for a little while - it takes a bit on the first start up). Hit ctrl-c to stop it. The config directory now contains a bunch of sample config files for you to edit.

Assuming it works, now make a service to start hass automatically. Create the following systemd init file. The serivce name ‘hass’ can be ‘homeassistant’ or whatever you want.

Copy to clipboard
nano hass.service

[Unit]
Description=Home Assistant
After=network.target

[Service]
Type=simple
User=homeassistant
ExecStart=/opt/homeassistant/bin/hass -c /opt/homeassistant/config --log-file /opt/homeassistant/hass.log

[Install]
WantedBy=multi-user.target
Exit the homeassistant user, copy the service file to the system, and update systemd to run the service.

Copy to clipboard
exit
sudo cp hass.service /etc/systemd/system/

sudo systemctl --system daemon-reload
sudo systemctl enable hass
sudo systemctl start hass
If something goes wrong with the start command, check the logs:

Copy to clipboard
sudo systemctl status hass
Finally, to make it easier to edit config files and try out code changes, I gave my regular user write permissions in the HA directory. (I’m running ACL’s on my drives which makes this simple)

Copy to clipboard
sudo setfacl -R -m u:[USER]:rwx /opt/homeassistant



hass –open-ui
 http://localhost:8123/

