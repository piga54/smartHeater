# Smart Water Heater
Smart Heater Water project made with Raspberry Pi. The aim of the project is controlling the temperature of a central Heater through a GUI and display it in real time. The control can be done by predefined temperatures or manually according to user need.

## Libraries

* [Qwt](http://qwt.sourceforge.net/). The Qwt library is based on Qt and is primarily useful for programs with a technical background.
* [Wiring Pi](http://wiringpi.com/). This library is used for setting the GPIO in Raspberry Pi.

## Making it work

To clone the git repository:

```
git clone https://github.com/piga54/smartHeater.git
```

To build:

```
cd smartHeater
make
```

To run (assuming you are logged into the RPi over ssh and no X-server is running):
```
startx ./smartHeater
```

### Makefile

Whenever a new **Makefile** is created (*qmake*) you need to modify it and add the Wiring Pi library in the *LIBS* line. The following code shows how it should be:

```
LIBS          = $(SUBLIBS) -L/usr/lib/arm-linux-gnueabihf -lqwt -lm -lQtGui -lQtCore -lpthread -lbcm2835 -lwiringPi
```

### Configuring a new sensor

By default the sensor device that is written in the code is **28-001522141bee**. The program works only for this specific sensor, if a different one is connected the program will not work. In order to run the program with a new sensor, a line in the *window.cpp* file must be changed (assuming that the new sensor has been set in the raspberry pi). Open the file:

```
cd smartHeater
nano window.cpp
```

And modify the following line with the new device where *28-001522141bee* is the current device name:

```C++
openFile = open("/sys/bus/w1/devices/28-001522141bee/w1_slave", O_RDONLY);
```


