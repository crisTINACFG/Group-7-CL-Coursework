/* Potentiometer */

// (Raspberry Pi Pico pinout: https://datasheets.raspberrypi.com/pico/Pico-R3-A4-Pinout.pdf)
// (Potentiometer pinout: https://www.circuitgeeks.com/wp-content/uploads/2022/01/How-Potentiometer-Works-768x231.png)

// --------------------------------------------------------------------

#pragma once

// --------------------------------------------------------------------

#define POTENTIOMETER_PIN       26      // Pin 31 (GPIO 26, ADC 0)

// --------------------------------------------------------------------
// (!) YOU NEED TO IMPLEMENT THESE METHODS!

/**
 * Called once at the start of the program to initialize all of the GPIO
 * pins for the potentiometer.
 */
void potentiometer_init();

/**
 * Read the raw value directly from the potentiometer.
 */
unsigned int potentiometer_read_raw();

/**
 * Read the raw value directly from the potentiometer with
 * potentiometer_read_raw and then express that as a number from 0 to
 * the specified limit (by mapping the value from the range of possible
 * raw values to the range [0, specified limit]).
 * 
 * Note that the limits on this function are upper-bound INCLUSIVE, not
 * exclusive. So if the limit is 9, the highest value produced by this
 * function is 9, not 8.
 */
unsigned int potentiometer_read(unsigned int limit);