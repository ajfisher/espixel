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
    PXP_DATA,
    PXP_ANIMATION
};

enum PX_ANIMATION_MODES {
    PX_ANIM_NONE,
    PX_ANIM_FADE,
    PX_ANIM_SHIFT
};


class ShiftAnimator {
    public:
        ShiftAnimator();
        ~ShiftAnimator();

        bool animating();
        void define(uint8_t amt, uint16_t shift_period, uint16_t duration,
                bool wrap, bool forwards);
        bool define(String config);
        void start();
        void stop();
        void update(uint8_t *px, uint16_t num_pixels, uint8_t colour_depth);

    private:
        bool _is_animating = false;     // whether we're animating or not

        uint8_t _amt = 0;               // amount we skip by
        bool _wrap = false;             // wrap around or not
        bool _forwards = false;         // forwards is incrementing along strip
                                        // ie px[0] -> px[1] -> px[2] etc
        uint16_t _shift_period = 0;     // how long between shifts in ms
        uint16_t _duration = 0;         // how long to shift for, 0 is infinite

        unsigned long _end_time = 0;    // msec the anim is to finish
        unsigned long _last_shift = 0;  // last time we effected a shift

        void shift_pixels(uint8_t *px, uint16_t px_count, uint8_t colour_depth);
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
        void set_pixel(uint16_t pixel, uint8_t r, uint8_t g, uint8_t b);
        void set_strip(uint8_t r, uint8_t g, uint8_t b);

    private:
        uint8_t _id = 0;
        uint8_t _pin = 0;
        bool _configured = false;

        PX_ANIMATION_MODES _anim_mode = PX_ANIM_NONE;
        ShiftAnimator * _shift_animator;

        uint8_t * _px = 0;
        uint16_t _px_count = 0;
        uint8_t _colour_depth = 3;

        unsigned long _last_update = 0;

        String _subscription = ""; // subscription topic.

        void initialise_pixels(uint8_t pin, uint16_t num_pixels);
        void _subscribe();
        void _set_length(uint16_t num_pixels);
        void _set_pin(uint8_t pin);
};

#endif



