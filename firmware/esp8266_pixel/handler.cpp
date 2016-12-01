/** This is used to manage the various handlers that we need and route
 * things to the appropriate places **/

#include "handler.h"

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

}
