/** Firmware to control WS2812 Pixels via MQTT on ESP8266 **/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "messaging.h"
#include "configurator.h"
#include "handler.h"

#define NPN_VERSION "0.7.1"
#define NPN_COMPILE (String(NPN_VERSION) + " - " + String(__DATE__) + " - " + String(__TIME__))

#define TRIGGER_PIN 5

// use this to get internal VCC value
ADC_MODE(ADC_VCC);

WiFiClient wifi_client;
Messaging mqtt(wifi_client);

Configurator config(mqtt);

bool state = false;
#define WAIT_TIME 1000
unsigned long _next_change = 0;

void reconnect() {
    // reconnects to mqtt
    if (! mqtt.configured ) {
        return;
    } else {
        if (! mqtt.connected() ) {

            // Do subscriptions to get the node's config data
            Serial.println(F("Connecting to MQTT"));
            mqtt.connect("i/d/#");
        }

        if (mqtt.connected() ) {
            // now we're connected publish version status.
            mqtt.publish("sys/version", NPN_COMPILE);
            mqtt.publish("o/c/status", "available");

            init_peripherals(mqtt);
        }
    }
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println();

    //client_name = String("ESP_") + String(ESP.getChipId(), 16);
    //clean FS, for testing
    //SPIFFS.format();

    // do test to see if we should reset.
    // set this momentarily.
    pinMode(TRIGGER_PIN, INPUT_PULLUP);
    Serial.println("Testing if we should reset");
    if (digitalRead(TRIGGER_PIN) == LOW) {
        // hold for a couple of seconds
        delay(2000);
        if (digitalRead(TRIGGER_PIN) == LOW) {
            // we're still low, format config.
            Serial.println("Formatting config");
            SPIFFS.format();
        }
    }

    handler_start();

    config.begin();

    if (mqtt.server) {

        mqtt.begin(subscription_handler);

        Serial.println("local ip");
        Serial.println(WiFi.localIP());

        reconnect();
    } else {
        Serial.println("We have no config setting for MQTT");
        // start the portal

    }
}

void loop() {
  // put your main code here, to run repeatedly:

    if (! mqtt.connected() ) {
        reconnect();
    } else {
        mqtt.handle_client();

        process_updates();
    }
}
