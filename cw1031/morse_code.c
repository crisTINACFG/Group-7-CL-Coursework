/**
 * The given template is a guideline for your coursework only.
 * You are free to edit/create any functions and variables.
 * You can add extra C files if required.
*/


#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "includes/seven_segment.h"
#define DOT_THRESHOLD 250
#define INTERLETTER 700
#define INTERSIGNAL 400
#define BUTTON_PIN 16;
// Pin 21 (GPIO 16)

// #define BUTTON_PIN 16	// Pin 21 (GPIO 16)

// declare global variables e.g., the time when the button is pressed 
int pressed ;

// --------------------------------------------------------------------
// declare the function definitions, e.g, decoder(...); and ther functions
// given the user input, you can decode if the input is a character
// void decoder();

// check if the button press is a dot or a dash
void checkButton();

const char morse_code[][26] =
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
	// Turn the seven segment display on when the program starts.
	sleep_ms(1000);
	seven_segment_off();
}

	// Initialise the button's GPIO pin.
	gpio_init(BUTTON_PIN);
	gpio_set_dir(BUTTON_PIN, GPIO_IN);
	gpio_pull_down(BUTTON_PIN); // Pull the button pin towards ground (with an internal pull-down resistor).

	while (true) {

		while (gpio_get(BUTTON_PIN)){			
            // record how long the button is pressed
            // .....
			printf("This line is a test\n");  // you can remove this line
			sleep_ms(150); // adjust the sleep_ms as required
		} 
        // check if the button press is a dot or a dash
		checkButton();
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
	bool button_pressed = gpio_get(BUTTON_PIN);
	uint32_t start_time, end_time, timePressed;

	while (true) {
		while (button_pressed == 0) {
			sleep_ms(10);
		}
		start_time = time_us_32();

		while (button_pressed == 1) {
			sleep_ms(10);
		}
		end_time = time_us_32();

		// The time it was pressed for
		timePressed = (end_time - start_time) / 1000;
	}
	if (timePressed < DOT_THRESHOLD) {
		morse_code[index++] = '.';
	}	
	else if (timePressed >= DOT_THRESHOLD && timePressed < INTERLETTER) {
		morse_code[index++] = '-';
	}
	if(timePressed > INTERLETTER) {
		decoder(morse_code);
		memset(morse_code, 0, sizeof(morse_code));
		index = 0;
	}
/* We want a string variable to store the morse code
if User waits more that 700ms the string decoder is used and then reset*/

}

