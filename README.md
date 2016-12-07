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

## Wiring up the module

Note that on the breakout board for the ESP12E pin 4 and 5 are labelled back
to front.

Connect the pins as follows:

ESP Module:

* 5V supply  to the VIN pin
* Ground to GND pin

NeoPixel Strip:

* VCC to 5V
* GND to GND
* DIN to ESP Module GPIO 2



## Interacting with the module

First determine what your ESP's address is - this is likely written on it or
you can plug it in and see it spin up an access point. If not, borrow a serial
cable and try that as it will print out on the serial line.

Now you know the node address, connect to it's WiFi Access Point from your computer
(there'll be no password) and then connect to it via http://192.168.4.1

You'll be prompted to scan for a network, you can do that and then enter the
details. You'll also want to enter the name of an MQTT server. You can use
either the IP address of your mosca server locally or you can use a public one
like `test.mosquitto.org` with port `1883`. Note that if you use the public
server *any* message you send will be publicly available.

Save the details and then power cycle the module. If you run a subscription
on `<moduleid>/#` you should see the module appear with a `sys/version` message
and ultimately a `/o/c/status` `available` message.

To do this via mqttjs assuming your MQTT server is your localhost do this:

```
./node_modules/.bin/mqtt_sub -v -t "ESP_f2df79/#" -h "127.0.0.1"
```

Change `ESP_f2df79` for your module name, and the `127.0.0.1` for
`test.mosquitto.org` if you're using that (or some other server address).

If the module has come up appropriately then you'll see a message like this:

```
ESP_f2df79/sys/version 0.7.0 - Dec  5 2016 - 23:23:17
ESP_f2df79/o/c/status available
ESP_f2df79/0/o/c/status available
```

If you get this, you're in business. You can now publish messages and your
module should behave.

* 1: tells you the version of the firmware being run.
* 2: tells you that the module itself has subscribed to inbound messages so
it can take configuration and data
* 3: tells you that the strip with ID 0 is available to take pixel messages.

By default all modules are configured to have a neopixel strip on GPIO2 which is
identified by strip `0`. You can have up to 8 independent strips and they can
each be of different lengths.

#### Resetting the module

By default, if the module can't find it's known wifi network it will reset back
into Access Point mode. Thus if you change location you should be fine.

If you want to force it (to change your MQTT server config for example) then
the easiest way to do this is to jumper GPIO 5 (incorrectly labelled '4' on the
breakout boards) to GROUND. Once this is done you should see the Access Point
spin back up again.

### To make a new strip

Send a definition message to your module like this:

`<moduleid>/i/d/add` `<pin>:<length>`

Where `<pin>` is the GPIO pin you want to connect to and `<length>` is how many
pixels are in the strip connected to it.

If configured correctly you'll see a new status message appear saying the strip
is available.

### Redefine a strip's length or pin

Send a definition message to your module as shown below.

Change length:

`<moduleid>/<id>/i/d/length` `<length>`

Where `<id>` is the ID of the strip you want to target and `<length>` is an
integer value that is the length you want to set it to.

Change pin:

`<moduleid>/<id>/i/d/pin` `<pin>`

Where `<id>` is the ID of the strip you want to target and `<pin>` is an
integer value that is the GPIO pin you want to set it to.

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

Send a content message to your module like this:

`<moduleid>/<id>/i/c/anim/shift` `<amt>:<shift period>:<duration>:<forwards>:<wrap>`

Where `<id>` is the ID of the strip you want to target, `<amt>` is the amount
you want to shift the pixels by each frame, `<shift period>` is how long
you want to wait in msec between each frame of animation, `<duration>` is how
long the animation should go for (`0` means indefinitely), `<forwards>` is a
boolean value indicating whether direction of shifting should be forwards (1) or
backwards (0) - forwards is along the strip. Finally `<wrap>` is a boolean
indicating whether to wrap the shifted pixels to the start (1) or not (0).

### Stop the animation

Send a content message to your module like this:

`<moduleid>/<id>/i/c/anim/stop` ` `

Where `<id>` is the ID of the strip you want to target and the message
is intentionally an empty string.

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
