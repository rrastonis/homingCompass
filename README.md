homingCompass
=============

As part of my MFA Design thesis at California College of the Arts, I made a compass that always points "home" and logs where you have been. The home point can be set at anytime using a button. The Homing Compass includes some code from Adafruit's Flora NeoGeo Watch as well as some help from Scott Minneman and Adam Coogan.

The directions on how to build the basic compass can be found here: https://learn.adafruit.com/flora-geo-watch/overview

I also added a 256K EEPROM (https://www.sparkfun.com/products/525) to log the GPS data, however it will only log for a few hours. If I were to make another I would buy a larger EEPROM or use an SD card.

Use readEEPROM to download the GPS data log from the EEPROM.
