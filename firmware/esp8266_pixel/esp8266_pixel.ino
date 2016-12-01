
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

//#include "espixel_lib.h"
#include "messaging.h"
#include "configurator.h"

#define NPN_VERSION "0.1.0"
#define NPN_COMPILE (String(NPN_VERSION) + " - " + String(__DATE__) + " - " + String(__TIME__))

// use this to get internal VCC value
ADC_MODE(ADC_VCC);

WiFiClient wifi_client;
Messaging mqtt(wifi_client);

Configurator config(mqtt);

String clientname;

void mqtt_setup() {
    // sets up server and connection for MQTT
    //mqtt_client.setServer(mqtt_server, 1883);
    // organise the callback here too.
}

void publish(String topic, String payload) {

//	mqtt_client.publish((clientname + "/" + topic).c_str(), payload.c_str());
}

void mqtt_reconnect() {

/**	if (mqtt_client.connect(clientname.c_str())) {
		Serial.println("connected");
		mqtt_client.subscribe((clientname + "/ic").c_str());
	} else {
		Serial.print("failed, rc=");
		Serial.print(mqtt_client.state());
		Serial.println(" try again in 5 seconds");
		// Wait 5 seconds before retrying
		delay(5000);
	}**/

}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println();

    clientname = String("ESP_") + String(ESP.getChipId(), 16);
    //clean FS, for testing
    //SPIFFS.format();

    config.begin();
    mqtt.begin();

    Serial.println("local ip");
    Serial.println(WiFi.localIP());

    if (! mqtt.connected() ) {

        // Do subscriptions to get the node's config data
        Serial.println(F("Connecting to MQTT"));
		mqtt.connect("id/#");
        if (mqtt.connected() ) {
            // now we're connected publish status.
            mqtt.publish("id/sys/version", NPN_COMPILE);
        }
    }

}

void loop() {
  // put your main code here, to run repeatedly:

    mqtt.handle_client();

}
