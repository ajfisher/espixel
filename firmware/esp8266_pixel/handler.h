/*
 * General library functions we need to use - mostly for handler functions
 * that are better placed outside of the main .ino
 */

#ifndef ESPIXEL_HANDLER_h
#define ESPIXEL_HANDLER_h

#include <Arduino.h>
#include "messaging.h"
#include "pixel_peripheral.h"

#define MAX_STRIPS 8


void handler_start();
void init_peripherals(Messaging& m);
void process_updates();
void subscription_handler(char* topic, byte* payload, unsigned int length);


#endif // ESPIXEL_HANDLER
