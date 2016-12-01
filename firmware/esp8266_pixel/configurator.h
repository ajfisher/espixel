#ifndef ESPIXEL_CONFIGURATOR_h
#define ESPIXEL_CONFIGURATOR_h

/*
 * This library looks after the configuration loading and saving as well as
 * creating things like parameters for extra values in the capture portal etc
 */

#include <FS.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <ArduinoJson.h>

#include "messaging.h"

#define DEFAULT_PORTAL_TIMEOUT 120

class Configurator {

    public:
        Configurator();
        Configurator(Messaging &m);
        ~Configurator() {};

        void begin();


    private:
        void load_config();
        void save_config();
        static void save_config_cb();
        void wifi_config();

        String _clientname;

        char _mqtt_server[40];
        char _mqtt_port[6];
        Messaging * _mqtt_client;

        //flag for saving data
        static bool _save_config;

};


#endif //ESPIXEL_CONFIGURATOR_h
