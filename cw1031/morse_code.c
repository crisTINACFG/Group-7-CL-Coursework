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
#include <time.h>

#define DOT_THRESHOLD 250 //less than 250 = dot, more than 250 = dash
#define INTERLETTER 400 //more than 400 = new letter, less than 400 = same letter
#define BUTTON_PIN 16

// declare global variables 
uint32_t start_time, end_time, timePressed, pause_start, pause_duration;
char morse_input[4] = {0};
int index = 0;

// Function prototypes
void checkButton();
void decoder(const char *input);

const char morse_code[26][5] =
{
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
"-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
"..-", "...-", ".--", "-..-", "-.--", "--.." 
};

uint32_t time_ms() {
   return to_ms_since_boot(get_absolute_time());
}

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
   

   while (true) {
    	while (!gpio_get(BUTTON_PIN)) {  // Wait for button to go true (pressed)
    		sleep_ms(10);
    	}
       	start_time = time_ms();
		pause_duration = pause_start - start_time; //calculates time it took to press button

       	while (gpio_get(BUTTON_PIN)) {  // Wait for button to go false (released)
        	sleep_ms(10);
       	}
       	end_time = time_ms();
       	pause_start = time_ms();

       	timePressed = end_time - start_time;

		if (pause_duration > INTERLETTER){ //ADD LOGIC FOR: AND IF mourse_input size reached 4!!
			decoder(morse_input);
			memset(morse_input, 0, sizeof(morse_input)); //based on https://www.geeksforgeeks.org/memset-c-example/
			index = 0; 
		} else {
			checkButton();
		}
   		
}


void decoder(const char *input){
   for (int i = 0; i < 26; i++) {
   if (strcmp(input, morse_code[i]) == 0) { //Based on: https://www.geeksforgeeks.org/strcmp-in-c/
       seven_segment_show(values[i]);
       return;
       }
    else{
       printf("Error: This morse code is unrecognized.\n");
   }}}

void checkButton() {
	if (timePressed < DOT_THRESHOLD) { 
		morse_input[index++]= '.';
		//printf(".");  
	} else if (timePressed > DOT_THRESHOLD) { 
		morse_input[index++] = '-';
	}
}

// for(int i = 0; i < 4; i++){
// 	printf("%c", morse_input[i]);
// }