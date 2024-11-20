/* Piezo-electric buzzer */

// --------------------------------------------------------------------
#pragma once

// --------------------------------------------------------------------

#define BUZZER_PIN      17      // Pin 22 (GPIO 17)

#define E3              165
#define F3              175
#define G3              196     // (Previous octave)
#define A               220     // A3
#define B               247     // B3
#define C               262     // C4 (middle C)
#define D               294
#define E               330
#define F               349
#define G               392
#define A4              440     // (Next octave)
#define B4              494
#define AS 466



// --------------------------------------------------------------------

/**
 * Compute (and set) PWM (pulse-width modulation) frequency and duty cycle for the specified frequency.
 * The return value is the computed PWM wrap value (which needn't necessarily be used).
 */
unsigned int pwm_set_freq_duty(
    /** The PWM generator "slice" to output the requested frequency on. */
    unsigned int slice,

    /** The PWM generator channel to output the requested frequency on. */
    unsigned int channel,

    /** The desired frequency. */
    unsigned int frequency,

    /**
     * The duty cycle (expressed as a percentage - e.g., 0.5 for 50% duty cycle)
     * For a buzzer, the duty cycle affects only the volume (from 0%, silence, up to 50%, max volume).
     * (50% to 100% is the same as 0% to 50%).
     */
    double duty
);

// --------------------------------------------------------------------

/**
 * Called once at the start of the program to initialize all of the GPIO
 * pins responsible for the buzzer.
 */
void buzzer_init();

/**
 * Disable the buzzer.
 */
void buzzer_disable();

/**
 * Enable the buzzer at the specified frequency.
 */
void buzzer_enable(unsigned int frequency);

void buzzer_quiet();
