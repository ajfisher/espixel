
#include "configurator.h"

WiFiManagerParameter * custom_mqtt_server;
WiFiManagerParameter * custom_mqtt_port;

bool Configurator::_save_config = false;

Configurator::Configurator() {

    custom_mqtt_server = new WiFiManagerParameter("server", "MQTT server", _mqtt_server, 40);
    custom_mqtt_port = new WiFiManagerParameter("port", "MQTT port", _mqtt_port, 6);

    _save_config = false;
}

Configurator::Configurator(Messaging& m) {

    Configurator();

    // add pointer to messaging object so we can use it later.
    _mqtt_client = &m;

    // cache this so we can refer to it when we need to.
    _clientname = _mqtt_client->id;

}

void Configurator::begin() {

    load_config();

    wifi_config();

    //save the custom parameters to FS
    if (_save_config) {
        //read updated parameters
        strcpy(_mqtt_server, custom_mqtt_server->getValue());
        strcpy(_mqtt_port, custom_mqtt_port->getValue());

        // now save off the file
        save_config();
    }

    // now configure the messaging system with the appropriate values
    _mqtt_client->server = String(_mqtt_server);
    _mqtt_client->port = atoi(_mqtt_port);

}

void Configurator::save_config_cb() {
  Serial.println("Should save config");
  _save_config = true;
}

void Configurator::save_config() {
    // saves the actual configuration down to a file so it can be used later

    Serial.println(F("Saving configuration to file"));

    DynamicJsonBuffer json_buffer;
    JsonObject& json = json_buffer.createObject();

    json["mqtt_server"] = _mqtt_server;
    json["mqtt_port"] = _mqtt_port;

    File cf = SPIFFS.open("/config.json", "w");
    if (!cf) {
        Serial.println(F("Failed to open config file for writing"));
    }

    json.printTo(Serial);
    json.printTo(cf);
    cf.close();
    //end save

}

// loading the config details
void Configurator::load_config() {
    // load all the config info.

    //read configuration from FS json

    if (SPIFFS.begin()) {
        if (SPIFFS.exists("/config.json")) {
            //file exists, reading and loading
            File cf = SPIFFS.open("/config.json", "r");
            if (cf) {
                size_t size = cf.size();

                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf(new char[size]);

                cf.readBytes(buf.get(), size);
                DynamicJsonBuffer json_buffer;
                JsonObject& json = json_buffer.parseObject(buf.get());

                //json.printTo(Serial);

                if (json.success()) {
                    strcpy(_mqtt_server, json["mqtt_server"]);
                    strcpy(_mqtt_port, json["mqtt_port"]);
                } else {
                    Serial.println(F("Failed to load json config"));
                }
            }
        }
    } else {
        Serial.println("Failed to mount FS");
    }
    //end read
}

void Configurator::wifi_config() {

    // The extra parameters to be configured (can be either global or just in the setup)
    // After connecting, parameter.getValue() will get you the configured value
    // id/name placeholder/prompt default length

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    //set config save notify callback
    wifiManager.setSaveConfigCallback(save_config_cb);

    //add all your parameters here
    wifiManager.addParameter(custom_mqtt_server);
    wifiManager.addParameter(custom_mqtt_port);

    //reset settings - for testing
    //wifiManager.resetSettings();

    //sets timeout until configuration portal gets turned off
    //useful to make it all retry or go to sleep
    //in seconds
    wifiManager.setTimeout(DEFAULT_PORTAL_TIMEOUT);

    //fetches ssid and pass and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    if (!wifiManager.autoConnect(_clientname.c_str())) {
        Serial.println(F("Failed to connect and hit timeout point"));
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);
    }

    //if you get here you have connected to the WiFi
    Serial.println(F("Connected to network..."));
}
