#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "includes/potentiometer.h"

// --------------------------------------------------------------------

// Notice that these constants are defined in the implementation file,
// rather than the header file.
//
// They are implementation-specific, so we place them here rather than
// bloat the header file with them. (Header files should be
// implementation-agnostic - that is, they should contain sufficient
// information for *all* implementations, but not *any one specific*
// implementation.)

#define POTENTIOMETER_MIN       20
#define POTENTIOMETER_MAX       4050

// You can "calibrate" the above values by continuously printing the
// result of potentiometer_read() and twisting the dial from one side
// to the other. Set POTENTIOMETER_MIN to the smallest value you see
// and POTENTIOMETER_MAX to the largest.
// (You can allow some room for error, for instance the smallest value
//  we observed was about 10, but it fluctuates some, so it's worth
//  setting the minimum to 20 to allow for those fluctuations.)

// --------------------------------------------------------------------

void potentiometer_init() {
    adc_init();
    adc_gpio_init(POTENTIOMETER_PIN);
}

unsigned int potentiometer_read_raw() {
    // Valid ADC pins are 26 to 29 inclusive (for inputs 0 to 3, respectively).
    // We can trivialyl convert between GPIO pin and ADC pin by subtracting 26
    // (to map 26-29 to 0-3).
    adc_select_input(POTENTIOMETER_PIN - 26);

    // adc_read obtains the 12-bit value (from 0 to 0xFFF) from the selected
    // ADC input.
    return adc_read();
}

/**
 * Map the specified value into the range [min, max] (assuming it was previously
 * in the range [original_min, original_max].)
 * 
 * This function performs integer division, so as to ensure that the output
 * values are an integer.
 * 
 * This is adapted from the Arduino map() function.
 * See: https://www.arduino.cc/reference/en/language/functions/math/map/
 */
int map(
    int value,
    int original_min,
    int original_max,
    int min,
    int max
) {
    int original_range = original_max - original_min;
    int range = max - min;

    return (value - original_min) * range / original_range + min;
}

/** Clamp the specified value into the range [min, max]. */
int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

unsigned int potentiometer_read(unsigned int limit) {
    // Map the raw value from the MIN and MAX above into the range
    // [0, limit] and return it.
    return map(
        clamp(potentiometer_read_raw(), POTENTIOMETER_MIN, POTENTIOMETER_MAX),
        POTENTIOMETER_MIN, POTENTIOMETER_MAX,
        1, limit
    );
}