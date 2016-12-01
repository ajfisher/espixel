#ifndef PixelPeripheral_h
#define PixelPeripheral_h

#include "peripherals.h"

enum PX_STATES {
    PXP_NONE,
    PXP_START,
    PXP_END,
    PXP_PIXEL,
    PXP_STRIP,
    PXP_DATA
};

class PixelPeripheral : public Peripheral {
    public:
        PixelPeripheral();
        void begin(Messaging& m);
        void publish_data();
        void sub_handler(String topic, String payload);
        void update();

    private:
        uint8_t * _px = 0;
        uint16_t _px_count = 0;
        uint8_t _colour_depth = 3;
        unsigned long _last_update = 0;

        void initialise_pixels(uint16_t num_pixels);
        void set_pixel(uint16_t pixel, uint8_t r, uint8_t g, uint8_t b);
        void set_strip(uint8_t r, uint8_t g, uint8_t b);

        // TODO:
        // Do a method to allow reading a binary data stream straight into
        // memory. This would be a stream of bytes up to _px_count * _colour_depth
        // in length and 0 indexed. On receipt using /ic/data it just reads it
        // off and straight into px buffer.

};

#endif



