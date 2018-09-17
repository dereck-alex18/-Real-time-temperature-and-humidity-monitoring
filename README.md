Title: Real-time temperature and humidity monitoring system of a server room

Description:
This program was designed to monitor a server room which needed to be in a certain temperature and humidity, otherwise 
the servers could be damaged.

To build the prototype were used: 
1. An ESP8266 with Wifi connection, 
2. A DHT22 which is a temperature and humidity sensor,
3. A cooler to cool down the room in case of high temperatures
4. A humidifier to moisten the room in case of low humidity
5. A speaker to be used as an alarm
6. Leds, Resitors and jumpers

The program monitor the temperature and humidity through the sensor, in case of high temperatures, low humidity, or both
the microcontroller actives the alarm, the cooler, the humidifier or both depending on the situation. Subsequently the ESP8266
sends an alert message to my (or whatever which the user desire) twitter account. When everything is stabilized another message 
is also sent. All datas can be monitored in real time via an IoT platform called "ThingSpeak".