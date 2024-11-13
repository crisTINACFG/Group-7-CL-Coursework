/**
 * The given template is a guideline for your coursework only.
 * You are free to edit/create any functions and variables.
 * You can add extra C files if required.
*/


#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "includes/seven_segment.h"
#include "hardware/gpio.h"

#define DOT_THRESHOLD 250
#define INTERLETTER 700
#define INTERSIGNAL 400
#define BUTTON_PIN 16

// declare global variables e.g., the time when the button is pressed 

// Function prototypes
void checkButton();
void decoder(const char *input);

const char morse_code[26][5] =
{
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
"-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
"..-", "...-", ".--", "-..-", "-.--", "--.." 
};

int main() {
	timer_hw->dbgpause = 0;
	stdio_init_all();

	//Print Welcome
	printf("Welcome \n");

	// Initialise the seven segment display.
	seven_segment_init();
	sleep_ms(1000);
	seven_segment_off();

	//Initialise the button's GPIO pin.
	gpio_init(BUTTON_PIN);
	gpio_set_dir(BUTTON_PIN, GPIO_IN);
	gpio_pull_down(BUTTON_PIN); // Pull the button pin towards ground (with an internal pull-down resistor).
	
	while(true){
		checkButton();
	}
}

void decoder(const char *input){
    for (int i = 0; i < 26; i++) {
	if (strcmp(input, morse_code[i]) == 0) { //Based on: https://www.geeksforgeeks.org/strcmp-in-c/
		seven_segment_show(values[i]);
		return;
		}
	}
	printf("Error: This morse code is unrecognized.\n");
}

void checkButton() {
	int index = 0;
	char morse_input[4] = {0}; // Buffer to store a single Morse character sequence
    uint32_t start_time, end_time, timePressed;

	while (true) {
		while (gpio_get(BUTTON_PIN)) {
			sleep_ms(10);
		}
		start_time = time_us_32();

		while (!gpio_get(BUTTON_PIN)) {
			sleep_ms(10);
		}
		end_time = time_us_32();

		// The time it was pressed for
		timePressed = (end_time - start_time) / 1000;
	}
	//Check if press is dot or dash
	if (timePressed < DOT_THRESHOLD) {
		morse_input[index++]= '.';
	}	
	else if (timePressed >= DOT_THRESHOLD && timePressed < INTERLETTER) {
		morse_input[index++] = '-';
	}
	if(timePressed > INTERLETTER) {
		decoder(morse_input);
		char morse_input[4] = {0}; //this should be resetting the morse_input string so that you can start another letter!!
	}
/* We want a string variable to store the morse code
if User waits more that 700ms the string decoder is used and then reset*/

}

