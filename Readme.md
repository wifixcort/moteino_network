Moteino network

The purpose is create a network of sensors with moteinos and raspberry Pi as a gateway / server.

The GenSens library simplifies the job of receiving and transmitting packets.

All transmitions are a C string type, only to send the sensor measurement
separated by ";" as many sensors like you want; the first value will be the node id that send the information.

The gateway is responsible for split the string to send to the Raspberry Pi emoncms platform.

How to connect the RPI
http://www.raspberrypi-spy.co.uk/2014/07/raspberry-pi-b-gpio-header-details-and-pinout/


You can uncomment the line "#define DEBUG" for some debug messages.

In principle it is not necessary to have an RPI, just open SoftwareSerial to see the reads of incoming data

// TODO
Save data in the EEPROM and restore from it
Send configuration from the gateway to the nodes or from a node to other node
