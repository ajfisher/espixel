# ESP Pixel

Controlling NeoPixels using ESP8266 and MQTT messaging.

## Installation

Clone this repo and then install basic dependencies (note Node 6+ is preferred):

```
git clone https://github.com/ajfisher/espixel.git && cd espixel
npm install
```

### Firmware dependencies

Assuming you have the following:

* Latest version of Arduino IDE
* The following libraries (install via Library Manager in Sketch menu):
    * ArduinoJson by Benoit Blanchon
    * DNSServer by Kristijan Novoselic
    * ESP8266 core addons (should be by default now)
    * PubSubClient by Nick O'Leary
    * WiFi Manager by tzapu

Open up the `firmware/esp8266_pixel` directory and the ino file and compile
and install from within Arduino.
