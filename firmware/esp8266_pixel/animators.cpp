#include "pixel_peripheral.h"

ShiftAnimator::ShiftAnimator() {
 // set up
}

ShiftAnimator::~ShiftAnimator() {
    // do nothing
}

void ShiftAnimator::define(uint8_t amt, uint16_t shift_period, uint16_t duration,
        bool wrap, bool forwards) {

    // this method configures the animation process

    _amt = amt;
    _shift_period = shift_period;
    _duration = duration;
    _wrap = wrap;
    _forwards = forwards;
    _end_time = millis() + duration;
}

bool ShiftAnimator::define(String config) {
    // used for the scenario where we're taking a payload string and we
    // need to just parse values out.

    char s[256];
    strcpy(s, config.c_str());
    s[config.length()] = '\0'; // put a proper end on it.

    char* token = strtok(s, ":");


    // item 0 is shift amount
    String t = String(token);
    _amt = t.toInt();
    if (_amt == 0) { return false; } // there was an error

    uint8_t position = 0;
    // we can drop the first as we've already looked at it.
    while (++position <= 4 && token) {

        // pop the next token off the stack
        token = strtok(NULL, ":");
        t = String(token);

        switch (position) {
            case 1:
                _shift_period = t.toInt();
                if (_shift_period == 0) { return false; }
                break;

            case 2:
                _duration = t.toInt();
                // not going to error check this. If it's invalid it will
                // loop forever.
                break;

            case 3:
                if (t == "0") {
                    _forwards = false;
                } else {
                    _forwards = true;
                }
                break;

            case 4:
                if (t == "0") {
                    _wrap = false;
                } else {
                    _wrap = true;
                }
                break;

        } // end switch

    }// end token while loop

    // if we're here it's pretty good.
    _end_time = millis() + _duration;
    return true;
}

bool ShiftAnimator::animating() { return _is_animating; }

void ShiftAnimator::start() {
    _is_animating = true;
}

void ShiftAnimator::stop() {
    _is_animating = false;
}

void ShiftAnimator::update(uint8_t *px, uint16_t num_pixels, uint8_t colour_depth){
    // take a pixel strip and then update it based on the animation settings.

    if (_is_animating) {
        // in theory we're animating. Check the time.
        unsigned long curr_time = millis();

        if (curr_time > _end_time && _duration > 0) {
            // we only stop if we have a defined stop point.
            stop();
            return;
        }

        if (_last_shift == 0 || curr_time > (_last_shift + _shift_period)) {
            // update the pixel buffer
            shift_pixels(px, num_pixels, colour_depth);
            _last_shift = curr_time;
        }
    }
}

void ShiftAnimator::shift_pixels(uint8_t *px, uint16_t px_count, uint8_t colour_depth) {
    // take the pixel array and shift the items along the array
    // shift forwards determines direction of travel and wrap determines
    // if the values need to be wrapped around again.

    uint8_t *tmp_px;
    uint16_t slice_index;
    uint8_t copy_byte_length;
    if (_amt > 0) {
        copy_byte_length = _amt*colour_depth;
    } else {
        return;
    }

    if (_wrap) {
        // need to allocate and then copy the memory from end of the array
        // into temporary array before we move it.
        if (tmp_px = (uint8_t *)malloc(copy_byte_length)) {
            memset(tmp_px, 0, copy_byte_length);
        }

        if (_forwards) {
            // grab from the end of the array;
            slice_index = (px_count - _amt);
        } else {
            // grab from the start of the array;
            slice_index = 0;
        }
        memcpy(tmp_px, px+slice_index*colour_depth, copy_byte_length);
    }
    // now memmove the data appropriately
    if (_forwards) {
        // memmove data down the array from 0 to length-amt
        memmove(px+copy_byte_length, px, (px_count - _amt) * colour_depth);
    } else {
        // memmove data up the array from amt to length to pos 0
        memmove(px, px+copy_byte_length, (px_count - _amt) * colour_depth);
    }

    if (_wrap) {
        uint16_t copy_index = 0;
        if (! _forwards) {
            // get the position at the end to drop this in on.
            copy_index = px_count - _amt;
        }

        memcpy(px+copy_index*colour_depth, tmp_px, copy_byte_length);
        free(tmp_px);
    } else {
        // if we're not wrapping around then we'll need to fill the rest
        // with 0s
        uint16_t fill_index = 0;
        if (! _forwards) {
            // get position at the end to fill from
            fill_index = px_count - _amt;
        }

        memset(px+fill_index*colour_depth, 0, copy_byte_length);
    }
}

