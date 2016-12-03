/** Firmware to control WS2812 Pixels via MQTT on ESP8266 **/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "messaging.h"
#include "configurator.h"
#include "handler.h"

#define NPN_VERSION "0.5.3"
#define NPN_COMPILE (String(NPN_VERSION) + " - " + String(__DATE__) + " - " + String(__TIME__))

// use this to get internal VCC value
ADC_MODE(ADC_VCC);

WiFiClient wifi_client;
Messaging mqtt(wifi_client);

Configurator config(mqtt);

bool state = false;
#define WAIT_TIME 1000
unsigned long _next_change = 0;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println();

    //client_name = String("ESP_") + String(ESP.getChipId(), 16);
    //clean FS, for testing
    //SPIFFS.format();

    handler_start();

    config.begin();
    mqtt.begin(subscription_handler);

    Serial.println("local ip");
    Serial.println(WiFi.localIP());

    if (! mqtt.connected() ) {

        // Do subscriptions to get the node's config data
        Serial.println(F("Connecting to MQTT"));
		mqtt.connect("id/#");
    }

    if (mqtt.connected() ) {
        // now we're connected publish version status.
        mqtt.publish("sys/version", NPN_COMPILE);

        init_peripherals(mqtt);
    }

}

void loop() {
  // put your main code here, to run repeatedly:

    mqtt.handle_client();

    process_updates();

}
