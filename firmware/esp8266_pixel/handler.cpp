/** This is used to manage the various handlers that we need and route
 * things to the appropriate places **/

#include "handler.h"

PixelPeripheral * pixels;

void init_peripherals(Messaging& m) {
    // initialises the various peripherals attached to the board.

    // set up peripheral
    pixels = new PixelPeripheral();
    pixels->begin(m, 2, 8);

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

    Serial.print("Topic: ");
    Serial.print(t);
    Serial.print(" Payload: ");
    Serial.println(p);

    pixels->sub_handler(t, p);

}
