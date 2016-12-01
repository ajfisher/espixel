/*
 * General library functions we need to use - mostly for handler functions
 * that are better placed outside of the main .ino
 */

#include <Arduino.h>
#include "messaging.h"
#include "pixel_peripheral.h"

void init_peripherals(Messaging& m);
void process_updates();
void subscription_handler(char* topic, byte* payload, unsigned int length);

