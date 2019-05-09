# APRS-Arduino-Tracker
This is a project for the Missouri Space Program at the University of Missouri Columbia.
It is a high altitude ballon tracker based on Arduino and the APRS packet reporting system. 

It is based off this [tutorial](https://learn.sparkfun.com/tutorials/hx1-aprs-transmitter-hookup-guide/all) with modification 
to work with our setup. It uses the [Trackuino library](https://github.com/sparkfun/SparkFun_Trackuino/tree/1.52/trackuino) for 
APRS packet transmission. Specifically the code to interact with the sensors is not used (we only have a GPS) and the GPS code was 
modified to work with the Adafruit Ultimate Breakout v3 GPS using the [Adafruit_GPS library](https://github.com/adafruit/Adafruit_GPS)

More information about APRS packet format can be found [here](http://www.aprs.net/vm/DOS/PROTOCOL.HTM)
