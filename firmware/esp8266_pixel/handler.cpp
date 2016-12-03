/** This is used to manage the various handlers that we need and route
 * things to the appropriate places **/

#include "handler.h"

PixelPeripheral * strips[MAX_STRIPS];
String strip_subscriptions[MAX_STRIPS];

uint8_t strip_count = 0;
String _client_name;

Messaging * _mqtt_client;

void handler_start() {

    _client_name = String("ESP_") + String(ESP.getChipId(), 16);
}

void init_peripherals(Messaging& m) {
    // initialises the various peripherals attached to the board.

    _mqtt_client = &m;
    // set up peripheral
    //
    // add a strip on PIN 2 which is ALWAYS available by default
    add_strip(DEFAULT_PX_PIN, DEFAULT_PX_LENGTH);
    //add_strip(0, 15);
}

void add_strip(String payload) {
    // here we add a new strip to our list. Specifically we will be
    // provided a string which is `<pin>:<length>` which we use to configure
    // the strip.

    if (payload.length() > 16) {
        _mqtt_client->publish(F("sys/error"), F("Invalid add strip payload"));
        return;
    }

    char s[16]; // this should not be a long message
    strcpy(s, payload.c_str());
    char* token = strtok(s, ":");

    uint8_t pin;
    uint16_t length;

    // look for the pin number first
    String t = String(token);
    // test explicitly for PIN 0.
    if (t == "0") {
        pin = 0;
    } else {
        // otherwise look what it is.
        pin = t.toInt();
        if (pin == 0) {
            // there was an error
            _mqtt_client->publish(F("sys/error"), F("Invalid pin"));
            return;
        }
    }

    // now we go get the length
    token = strtok(NULL, "/");
    t = String(token);
    // again test explictly for length 0 (why would you do that?)
    if (t == 0) {
        length = 0;
    } else {
        length = t.toInt();
        if (length == 0) {
            // error so call it
            _mqtt_client->publish(F("sys/error"), F("Invalid length"));
            return;
        }
    }

    // if you've made it here you have a real(ish) pin and length
    add_strip(pin, length);
}

void add_strip(uint8_t pin, uint16_t num_pixels) {

    if (strip_count >= MAX_STRIPS) {
        _mqtt_client->publish(F("sys/error"), F("max strips exceeded"));
    } else {

        if (strip_count > 0) {
            // make sure we're not adding to something already assigned
            for (uint8_t i; i < strip_count; i++) {
                if (pin == strips[i]->get_pin()) {
                    _mqtt_client->publish("sys/error", "Pin already in use");
                    return;
                }
            }
        }

        // now create the new strip and start it up with an ID
        strips[strip_count] = new PixelPeripheral(strip_count);
        strips[strip_count]->begin(*_mqtt_client, pin, num_pixels);

        // get the subscription topic so we can add it to the list

        String sub = strips[strip_count]->get_subscription_topic();
        // deal with wildcard completion
        if (sub.endsWith("#")) {
            sub.remove(sub.length() - 1);
        }

        // add client name to front so we don't need to keep calculating it.
        sub = _client_name + "/" + sub;
        Serial.print("Strip subscription topic: ");
        Serial.println(sub);

        strip_subscriptions[strip_count] = sub;

        strip_count++;
    }
}

void process_updates() {

    // periodically we need to do some work especially on the peripherals
    //pixels->update();
    if (strip_count > 0) {
        for (uint8_t i=0; i< strip_count; i++) {
            strips[i]->update();
        }
    }
}

void subscription_handler(char* topic, byte* payload, unsigned int length) {

    String t = (String)topic;

    // grab the data from the payload and get it in a form we can use it.
    char buf[length + 1];
    for (int i = 0; i < length; i++) {
        buf[i] = (char)payload[i];
    }
    buf[length] = '\0';
    String p = String(buf);

#ifdef DEBUG
    Serial.print("Topic: ");
    Serial.print(t);
    Serial.print(" Payload: ");
    Serial.println(p);
#endif

    // process the items in the strip first as it's most likely one of these
    if (strip_count > 0) {
        for (uint8_t i=0; i < strip_count; i++) {
            /**Serial.print("T: ");
            Serial.print(t);
            Serial.print(" Subscription: ");
            Serial.print(i);
            Serial.print(" ");
            Serial.println(strip_subscriptions[i]);**/
            if (t.startsWith(strip_subscriptions[i]) ) {
                // rip off all the preamble that we no longer need now we know
                // where to route this.
                t.remove(0, strip_subscriptions[i].length());
                strips[i]->sub_handler(t, p);
                return;
            }
        }
    }
    if (t.endsWith("i/d/add")) {
        add_strip(p);
    }
}
