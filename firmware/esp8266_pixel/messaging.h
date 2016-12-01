#ifndef ESPIXEL_MESSAGING_h
#define ESPIXEL_MESSAGING_h

/*
 * Used to abstract and manage some of the common messaging tasks such as the
 * specifics around where to subscribe topics and how to handle them
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

class Messaging {

    public:

        Messaging();
        Messaging(Client& wc);
        ~Messaging() {};

        void begin();
        bool connect();
        bool connect(String topic);
        bool connected();
        void publish(String topic, String payload);
        void subscribe(String topic);

        void handle_client();

        String server;
        uint16_t port;
        String id;

    private:

        PubSubClient _client;

};



#endif // ESPIXEL_MESSAGING_h
