#include "includes/buzzer.h"

#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
// ------ You do NOT need to edit this file ---------------------------
// --------------------------------------------------------------------

void buzzer_init() {
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
}

void buzzer_disable() {
    // Mark the slice associated with the BUZZER_PIN disabled.
    pwm_set_enabled(pwm_gpio_to_slice_num(BUZZER_PIN), false);
}

void buzzer_quiet() {

    // Set the frequency and duty cycle for the specified pin to the specified frequency.

    pwm_set_freq_duty(

    pwm_gpio_to_slice_num(BUZZER_PIN), // PWM Slice (comptued from pin)

    pwm_gpio_to_channel(BUZZER_PIN), // PWM Channel (computed from pin)

    0, // Frequency in Hz (as specified)

    0 // Duty cycle = 0.1 (max volume = 0.5)

    );


    // Mark the slice associated with the BUZZER_PIN enabled.

    pwm_set_enabled(pwm_gpio_to_slice_num(BUZZER_PIN), true);

}

void buzzer_enable(unsigned int frequency) {
    // Set the frequency and duty cycle for the specified pin to the specified frequency.
    pwm_set_freq_duty(
        pwm_gpio_to_slice_num(BUZZER_PIN),          // PWM Slice (comptued from pin)
        pwm_gpio_to_channel(BUZZER_PIN),            // PWM Channel (computed from pin)
        frequency,                                  // Frequency in Hz (as specified)
        0.1                                        // Duty cycle = 0.1 (max volume = 0.5)
    );

    // Mark the slice associated with the BUZZER_PIN enabled.
    pwm_set_enabled(pwm_gpio_to_slice_num(BUZZER_PIN), true);
}

// --------------------------------------------------------------------

unsigned int pwm_set_freq_duty(
    unsigned int slice,
    unsigned int channel,
    unsigned int frequency,
    double duty
) {

    // The Raspberry Pi Pico is clocked at 125Mhz.
    unsigned int clock = 125000000;
    
    // Compute the divider (multiplied by 16).
    // An 8-bit value is up to 256 (which is the integer limit for our divider), multiplied
    // again by 16 (256 * 16) gives us a multiple of 4096 for the frequency.
    unsigned int divider16 = ceil(clock / (frequency * (double) 4096));

    // Ensure the divider is no lower than 16.
    if (divider16 < 16) divider16 = 16;
    
    // The wrap (period) (number of cycles to count up to) can be computed as
    // clock (divided by divider) divided by the frequency (minus one because wrap
    // is the maximum cycle number, not the number of cycles).
    //
    // (This is computed as "clock * 16 / divider16" to avoid having to cast
    //  and divide the divider, but it is mathematically equivalent.)
    unsigned int wrap = (clock * 16 / divider16 / frequency) - 1;

    // Naturally, divider16 / 16 is the integer part of the divider, whilst
    // the argument after is the fractional part of the divider. It is a 4-bit (up to 16)
    // value, and 0xF, 15 in hexadecimal, represents the maximum possible 4-bit value.
    // Hence, bitwise-ANDing the divider with 0xF gives us the 4-bit fractional part of the
    // divider.
    pwm_set_clkdiv_int_frac(slice, divider16 / 16, divider16 & 0xF);

    // Set the wrap and level for the specified slice.
    pwm_set_wrap(slice, wrap);
    pwm_set_chan_level(slice, channel, wrap * duty);

    // Return the wrap value for (optional) use by the calling program.
    return wrap;

}
