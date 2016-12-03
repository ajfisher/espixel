/** This is used to manage the various handlers that we need and route
 * things to the appropriate places **/

#include "handler.h"

PixelPeripheral * pixels;
String pixel_sub;

uint8_t strip_count = 0;
String _client_name;

void handler_start() {

    _client_name = String("ESP_") + String(ESP.getChipId(), 16);

}

void init_peripherals(Messaging& m) {
    // initialises the various peripherals attached to the board.

    // set up peripheral
    pixels = new PixelPeripheral();
    pixels->begin(m);

    pixel_sub = pixels->get_subscription_topic();
    // deal with wildcard completion
    if (pixel_sub.endsWith("#")) {
        pixel_sub.remove(pixel_sub.length() - 1);
    }

    // add client name to front so we don't need to keep calculating it.
    pixel_sub = _client_name + "/" + pixel_sub;
    Serial.print("Pixel subscription topic: ");
    Serial.println(pixel_sub);
}

void add_strip(Messaging& m, uint8_t pin, uint16_t num_pixels) {

}

void process_updates() {

    // periodically we need to do some work especially on the peripherals
    pixels->update();
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

    if (t.startsWith(pixel_sub) ) {
        // rip off all the preamble that we no longer need now we know
        // where to route this.
        t.remove(0, pixel_sub.length());
        pixels->sub_handler(t, p);
    }

}
