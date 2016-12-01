#ifndef ESPIXEL_MESSAGING_h
#define ESPIXEL_MESSAGING_h

/*
 * Used to abstract and manage some of the common messaging tasks such as the
 * specifics around where to subscribe topics and how to handle them
 */
#include <PubSubClient.h>

#define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback

class Messaging {

    public:

        Messaging();
        Messaging(Client& wc);
        ~Messaging() {};

        void begin();
        void begin(MQTT_CALLBACK_SIGNATURE);
        bool connect();
        bool connect(String topic);
        bool connected();
        void publish(String topic, String payload);
        bool subscribe(String topic);

        void set_callback(MQTT_CALLBACK_SIGNATURE);
        void handle_client();

        String server;
        uint16_t port;
        String id;

    private:

        PubSubClient _client;
};



#endif // ESPIXEL_MESSAGING_h
