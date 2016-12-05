# ESP Pixel

Controlling NeoPixels using ESP8266 and MQTT messaging.

## Installation

Clone this repo and then install basic dependencies (note Node 6+ is preferred):

```
git clone https://github.com/ajfisher/espixel.git && cd espixel
npm install
```

This will install:

* Node Red server that can be run locally (including contrib modules for Johnny
Five, InfluxDB
* Johnny Five (to interact with a local arduino)
* Interchange (so you can flash firmware to an arduino)
* MQTT js (to talk to MQTT servers)

Once running you'll need to know your ip address (`ifconfig`) and you'll want
to start up node red and a local MQTT server. You can do this with the packages
you just installed which are explained below.

## Running Node Red

To run node-red, `cd` to the root of your project folder then run:

```
./node-modules/.bin/node-red
```

And you will see Node Red spin up. If there are no errors you can access it
on [Localhost:1880](http://127.0.0.1:1880)

By default, contrib packages have been installed for:

* Johnny Five (GPIO nodes)
* MQTT
* Twitter

You can install more by simply npm-installing from your project folder and use
the packages that are provided at [http://flows.nodered.org/](http://flows.nodered.org/).

## Running a local MQTT Server

If you've installed the repo dependencies then you'll be able to run your own
node-based MQTT server using `mosca`.

To run the server:

```
./node_modules/.bin/mosca -v --http-port 8883
```

This will pass all the config details to the command line but should set up
the standard MQTT server on port 1883 as well as an HTTP Websockets port on
8883 on your local host.

You can demo this working by subscribing to a topic and then publishing a message.

In a new terminal execute a subscription request to the topic `hello`:

```
./node_modules/.bin/mqtt sub -t "hello" -h "localhost" -v
```

Then in yet *another* new terminal, publish a message to the `hello` topic:

```
./node_modules/.bin/mqtt pub -t 'hello' -h 'localhost' -m 'from MQTT.js'
```

If everything has gone properly then in your server terminal you should see the
message and payload details and in the subscription terminal you should see
the message results.

Congrats, you now have your very own MQTT broker and can publish messages and
subscribe to topics. If you wanted to, you could now point your Node-Red
instance at it using the MQTT publish and subscribe nodes.

### Alternative servers

If you have a linux VM or linux machine you can install mosquitto
with simply an `apt-get install mosquitto mosquitto-client` which will give you
a highly configurable and robust MQTT broker to play with. This is what is running
the site infrastructure for BuzzConf.

## Interacting with the module

First determine what your ESP's address is - this is likely written on it or
you can plug it in and see it spin up an access point. If not, borrow a serial
cable and try that as it will print out on the serial line.

Now you know the node address, connect to it's WiFi Access Point from your computer
and then once that's done connect to it via http://192.168.4.1

You'll be prompted to scan for a network, you can do that and then enter the
details and you'll also want to enter the name of an MQTT server. You can use
either the IP address of your mosca server locally or you can use a public one
like `test.mosquitto.org`.

Save the details and then power cycle the module. If you run a subscription
on `<moduleid>/#` you should see the module appear with a `sys/version` message
and ultimately a `/o/c/status` `available` message.

If you get this, you're in business. You can now publish messages and your
module should behave.

By default all modules are configured to have a neopixel strip on GPIO2 which is
identified by strip `0`. You can have up to 8 independent strips and they can
be of different lengths.

### To make a new strip

Send a definition message to your module like this:

`<moduleid>/i/d/add` `<pin>:<length>`

Where `<pin>` is which GPIO pin you want to connect to and `<length>` is how many
pixels are in the strip connected to it.

If configured correctly you'll see a new status message appear saying the strip
is available.

### Make a strip turn a particular colour

Send a content message to your module like this:

`<moduleid>/<id>/i/c/strip` `rrggbb`

Where `<id>` is the ID of the strip you want to target and `rrggbb` is a hex
string of the colour you want the strip to be.

### Make a pixel turn a particular colour

Send a content message to your module like this:

`<moduleid>/<id>/i/c/px/<px num>` `rrggbb`

Where `<id>` is the ID of the strip you want to target, `<px num>` is the pixel
you're interested in and `rrggbb` is the colour you want to set.

### Make the pixels shift animate.

TODO



## Firmware installation

For those who want to install the firmware on an ESP8266, here's how you do it.

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
