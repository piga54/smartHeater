# Smart Water Heater
Smart Heater Water project made with Raspberry Pi.

Sensor's IDs
1. 28-001522141bee --- Pedro
2. 28-0015230752ee --- Nick

Libraries for Makefile:
LIBS          = $(SUBLIBS) -L/usr/lib/arm-linux-gnueabihf -lqwt -lm -lQtGui -lQtCore -lpthread -lbcm2835 -lwiringPi

Making it work

To clone the git repository:

git clone https://github.com/piga54/smartHeater.git

To build:

cd smartHeater
qmake
make

To run (assuming you are logged into the RPi over ssh and no X-server is running):

startx ./smartHeater




