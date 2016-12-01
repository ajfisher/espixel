
#include "messaging.h"

Messaging::Messaging() {}

Messaging::Messaging(Client& wc)
{
    // wires up the various components.

    _client = wc;
    id = String("ESP_") + String(ESP.getChipId(), 16);

}

void Messaging::begin() {

    // do any work needed to start up.
    Serial.print("Node: ");
    Serial.print(id);
    Serial.print(" MQTT Server: ");
    Serial.print(server);
    Serial.print(" Port: ");
    Serial.println(port);

    _client.setServer(server.c_str(), port);

    // connect is called explicitly.
}

void Messaging::begin(MQTT_CALLBACK_SIGNATURE) {

    begin();

    set_callback(callback);

}
bool Messaging::connect() {

    return _client.connect(id.c_str());
}

bool Messaging::connect(String topic) {

    // sets up the connection and then subscribes to the topic.
    if( connect() ) {
        Serial.println("Now subscribe");
        subscribe(topic);
    }
}

bool Messaging::connected() {
    return _client.connected();
}


void Messaging::handle_client() {
    // used to pass control over to the PubSubClient to handle any
    // subscription or publication tasks as needed.
    if (_client.connected() ) {
        _client.loop();
    }
}


void Messaging::publish(String topic, String payload) {
    // takes a topic and payload, prepends it with node id and then
    // publishes it.

    if (connected()) {

        String t = String(id) + String("/") + topic;
        _client.publish(t.c_str(), payload.c_str());

        Serial.println(t);
        Serial.println(payload);
    } else {
        Serial.println("Not connected");
    }

}

bool Messaging::subscribe(String topic) {

	String t = String(id) + String("/") + topic;

    Serial.print("Subscribing to: ");
    Serial.print(t);

    bool substate = _client.subscribe(t.c_str());
    if (substate) {
        Serial.println(" - success");
    } else {
        Serial.println(" - failure");
    }

    return (substate);
}

void Messaging::set_callback(MQTT_CALLBACK_SIGNATURE) {

    _client.setCallback(callback);

}
