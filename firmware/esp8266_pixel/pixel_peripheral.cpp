#include "pixel_peripheral.h"

// ESP8266 show() is external to enforce ICACHE_RAM_ATTR execution
extern "C" void ICACHE_RAM_ATTR show(
  uint8_t pin, uint8_t *pixels, uint32_t numBytes);

PixelPeripheral::PixelPeripheral() {

    // set peripheral values
    _updateable = true;
    _publishable = false;

    _subscription = SUB_TOPIC;
}

PixelPeripheral::~PixelPeripheral() {
    // just need to free up any pixel memory that was allocated.

    Serial.println("Freeing memory");
    if (_px) {
        free(_px);
        _px_count = 0;
    }
    Serial.println("Memory freed");
}

void PixelPeripheral::initialise_pixels(uint8_t pin, uint16_t num_pixels) {
    // set up the pixel array, allocate mem etc.
    //

    _set_pin(pin);

    _set_length(num_pixels);

    _configured = true;
}

void PixelPeripheral::begin(Messaging& m) {
    // begin with defaults for everything
    begin(m, DEFAULT_PIXEL_PIN, DEFAULT_PIXEL_COUNT);
}

void PixelPeripheral::begin(Messaging& m, uint8_t pin, uint16_t num_pixels) {
    // set up the link to the mqtt server and do any other work.
    _mqtt_client = m;

    initialise_pixels(pin, num_pixels);

    _subscribe();

}

void PixelPeripheral::_subscribe() {
    // does the actual subscription process

    bool subbed = _mqtt_client.subscribe(_subscription);

    if (! subbed) {
        Serial.println("Couldn't subscribe to peripheral messages");
        _mqtt_client.publish((String(_id) + "/sys/error"), "sub_fail");
    } else {
        _mqtt_client.publish((String(_id) + "/o/c/status"), "available");
    }
}

void PixelPeripheral::_set_length(uint16_t num_pixels) {

    if (_px) {

        // if we already have pixels then set the old ones to zero,
        // call update one last time to blank them and then free them out.
        memset(_px, 0, _px_count * _colour_depth);
        show(_pin, _px, _px_count * _colour_depth);

        // now free them all up
        free (_px);
        _px_count = 0;
    }

    if (num_pixels > 0) {
        if (_px = (uint8_t *)malloc(num_pixels * _colour_depth)) {
            memset(_px, 0, num_pixels * _colour_depth);
            _px_count = num_pixels;
        } else {
            _px_count = 0;
        }
    }
}

void PixelPeripheral::_set_pin(uint8_t pin) {
    // change the pin from whatever it was to the newly supplied one

    // set old pin pinmode to input
    // set int variable to new pin
    // set new pin pinmode to output.

    if (pin != _pin) {
        // only do this if there's an actual change
        // turn the strip off then set it to the new pin
        if (_configured) {
            // we are configured properly so we can target an old pin.
            set_strip(0, 0, 0);
            show(_pin, _px, _px_count * _colour_depth);
            pinMode(_pin, INPUT);
        }
        _pin = pin;
        pinMode(_pin, OUTPUT);
    }

}


void PixelPeripheral::set_pixel(uint16_t pixel, uint8_t r, uint8_t g, uint8_t b) {
    // set the pixel value in memory

    uint16_t pa = pixel * _colour_depth;
    // assuming GRB addressing with this for speed
    _px[pa] = g;
    _px[pa+1] = r;
    _px[pa+2] = b;
}

void PixelPeripheral::set_strip(uint8_t r, uint8_t g, uint8_t b) {
    // sets the entire strip this colour.
    for (uint16_t i=0; i < _px_count; i++) {
        set_pixel(i, r, g, b);
    }
}

void PixelPeripheral::publish_data() {
    // for a pixel display - this is a NOOP
}

String PixelPeripheral::get_subscription_topic() {
    // returns the topic that a subscription has been set up
    // on for this strip which is useful for routing.
    return _subscription;
}

void PixelPeripheral::sub_handler(String topic, String payload) {
    // Handle any messages on topics we're subscribed to.
    //
    // bust up the string so we can grab the pertinent parts.
    char s[256];
    strcpy(s, topic.c_str());
    char* token = strtok(s, "/");

    uint16_t pixel_number;
    uint8_t r, g, b;

    PX_STATES state = PXP_NONE;

    if ( strlen(token) > 1) {
        return; // all input topics should start with c or d
    }

    if (token[0] == 'c') {
        state = PXP_CONTENT;
    } else if (token[0] == 'd') {
        state = PXP_DEFINITION;
    } else {
        return; // bail as we don't care
    }

    // we can drop the first as we've already looked at it.
    while (token) {

        // pop the next token off the stack
        token = strtok(NULL, "/");
        String t = String(token);

        switch (state) {

            case PXP_CONTENT: {
                // see if our token is px or strip
                if (t == "px") {
                    state = PXP_PIXEL;
                } else if (t == "strip") {
                    state = PXP_STRIP;
                } else if (t == "data") {
                    state = PXP_DATA;
                }
                continue;
                break;
            }

            case PXP_PIXEL: {
                // the next token should be a number
                // explicitly test for 0
                if (t == "0") {
                    pixel_number = 0;
                } else {
                    pixel_number = t.toInt();
                    if (pixel_number == 0) {
                        // there was an error
                        pixel_number = NULL;
                    }
                }

                // get the pixel values off the payload and turn them into
                // rgb vals
                r = g = b = 0;

                // convert to values which are then put into the pixel array.
                if (payload.length() != 6) {
                    Serial.print("PXP: Error pixel value incorrect: ");
                    Serial.println(payload);
                } else {
                    // we're good so peel the numbers off and let's go.
                    String v = payload.substring(0, 2);
                    r = (uint8_t)strtol(v.c_str(), NULL, 16);
                    v = payload.substring(2, 4);
                    g = (uint8_t)strtol(v.c_str(), NULL, 16);
                    v = payload.substring(4);
                    b = (uint8_t)strtol(v.c_str(), NULL, 16);

                    if (pixel_number != NULL) {
                        set_pixel(pixel_number, r, g, b);
                    }
                }

                break;  // PXP_PIXEL
            }

            case PXP_STRIP: {
                r = g = b = 0;

                // convert to values which are then put into the pixel array.
                if (payload.length() != 6) {
                    Serial.print("PXP: Error pixel value incorrect: ");
                    Serial.println(payload);
                } else {
                    // we're good so peel the numbers off and let's go.
                    String v = payload.substring(0, 2);
                    r = (uint8_t)strtol(v.c_str(), NULL, 16);
                    v = payload.substring(2, 4);
                    g = (uint8_t)strtol(v.c_str(), NULL, 16);
                    v = payload.substring(4);
                    b = (uint8_t)strtol(v.c_str(), NULL, 16);

                    set_strip(r, g, b);

                }
                break;
            }

            case PXP_DATA: {
                // we have here a buffer of pixel data.
                // reset to zeros
                uint16_t px_bytes = _px_count * _colour_depth;
                uint16_t cpy_len = px_bytes;

                memset(_px, 0, px_bytes);
                // now overwrite the px buffer

                if (payload.length() < px_bytes) {
                    cpy_len = payload.length();
                }
                memcpy(_px, payload.c_str(), cpy_len);
                break;
            }

            case PXP_DEFINITION: {

                if (t == "strip") {
                    // yep we're a strip, stay in this state
                    ;
                } else if (t == "length") {

                    uint16_t l = payload.toInt();
                    if (l > 0) {
                        _set_length(l);
                    } else {
                        // look specifically for a zero
                        if (payload == "0") {
                            _set_length(0);
                        }
                        // otherwise an error and we don't care
                    }

                } else if (t == "pin") {

                    uint8_t p = payload.toInt();
                    if (p > 0) {
                        _set_pin(p);
                    } else {
                        // look specifically for val "0"
                        if (payload == "0") {
                            _set_pin(0);
                        }
                        // otherwise it was an error and evaled to 0 so ignore
                    }

                }

                break;
            }

        }

    }

}

void PixelPeripheral::update() {

    // update the state of the pixels
    //
    // we do this to fps lock the pixels. The reason is that
    // update is called as fast as possible and it can sometimes
    // cause issues with flicker.
    if (millis() > (_last_update + FRAME_MILLIS) ) {
        show(_pin, _px, _px_count * _colour_depth);
        _last_update = millis();
    }
}

