/*
 * General library functions we need to use - mostly for handler functions
 * that are better placed outside of the main .ino
 */

#include <Arduino.h>

void subscription_handler(char* topic, byte* payload, unsigned int length);
