# Install some dependencies
apt install software-properties-common

# Add ppa for python 3.7 and install
add-apt-repository ppa:deadsnakes/ppa
apt update
apt install python3.7 python3.7-dev autoconf automake libtool

# check python 3.7 version
python3.7 --version

# Create 3.7 venv
virtualenv -p /usr/bin/python3.7 /srv/homeassistant
cd /srv/homeassistant
# Activate it
. bin/activate
# Check that venv uses 3.7
python --version

# Upgrade pip and some tools
pip install --upgrade pip
pip install setuptools

# Install homeassistant
pip install --upgrade homeassistant
