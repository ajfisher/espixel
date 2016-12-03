#ifndef PixelPeripheral_h
#define PixelPeripheral_h

#include "peripherals.h"

#define DEFAULT_PIXEL_PIN  2
#define DEFAULT_PIXEL_COUNT 64

#define SUB_TOPIC (String(_id) + String("/i/#"))

// corresponds to 30fps which is fast enough for static displays
#define FRAME_MILLIS 30

enum PX_STATES {
    PXP_NONE,
    PXP_START,
    PXP_END,
    PXP_DEFINITION,
    PXP_CONTENT,
    PXP_PIXEL,
    PXP_STRIP,
    PXP_DATA
};

class PixelPeripheral : public Peripheral {
    public:
        PixelPeripheral();
        PixelPeripheral(uint8_t id);
        ~PixelPeripheral();
        void begin(Messaging& m);
        void begin(Messaging& m, uint8_t pin, uint16_t num_pixels);
        uint8_t get_pin();
        uint16_t get_length();
        String get_subscription_topic();
        void publish_data();
        void sub_handler(String topic, String payload);
        void update();

    private:
        uint8_t _id = 0;
        uint8_t _pin = 0;
        bool _configured = false;

        uint8_t * _px = 0;
        uint16_t _px_count = 0;

        uint8_t _colour_depth = 3;

        unsigned long _last_update = 0;

        String _subscription = ""; // subscription topic.

        void initialise_pixels(uint8_t pin, uint16_t num_pixels);
        void _subscribe();
        void _set_length(uint16_t num_pixels);
        void _set_pin(uint8_t pin);
        void set_pixel(uint16_t pixel, uint8_t r, uint8_t g, uint8_t b);
        void set_strip(uint8_t r, uint8_t g, uint8_t b);

        // TODO:
        // Do a method to allow reading a binary data stream straight into
        // memory. This would be a stream of bytes up to _px_count * _colour_depth
        // in length and 0 indexed. On receipt using /ic/data it just reads it
        // off and straight into px buffer.

};

#endif



