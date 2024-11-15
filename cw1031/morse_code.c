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
uint32_t start_time1, start_time2, end_time, timePressed, pause_start, pause_duration;
char morse_input[4];
int morse_input_index = 0;
int loop_index = 0;

// Function prototypes
void checkButton();
void decoder(const char *input);

const char morse_code[26][4] =
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
   
   //| morse_input_index >= 4

   while (true) {

    	while (!gpio_get(BUTTON_PIN)) {  // Wait for button to go true (pressed)
    		sleep_ms(20);
    	}
       	start_time1 = time_ms();
		
		if (loop_index > 0) {
			start_time2 = time_ms();
			pause_duration = (start_time2 - pause_start);
			//printf("start time2----------%lu\n", (unsigned long)start_time2);
		}

		//printf("start time--------%lu\n", (unsigned long)start_time1);
	
       	while (gpio_get(BUTTON_PIN)) {  // Wait for button to go false (released)
        	sleep_ms(20);
       	}
		loop_index++;
       	end_time = time_ms();
       	pause_start = time_ms();

       	timePressed = (end_time - start_time1);
		
		// printf("end time--------%lu\n", (unsigned long)end_time);
		// printf("pause start--------%lu\n", (unsigned long)pause_start);
		// printf("pause_duration--------%lu\n", (unsigned long)pause_duration);
		// printf("time pressed--------%lu\n", (unsigned long)timePressed);



		if (pause_duration > INTERLETTER  && morse_input_index > 0){ //ADD LOGIC FOR: AND IF mourse_input size reached 4!!
			decoder(morse_input);
			printf("%s\n", morse_input);
			memset(morse_input, 0, 4);//based on https://www.geeksforgeeks.org/memset-c-example/

			//seven_segment_init(); 
			morse_input_index = 0; 
		} else {	
			checkButton();
			for(int i = 0; i < 4; i++){
				printf("%c", morse_input[i] );
				
			}
		}
   }
}


void decoder(const char *input){
   for (int i = 0; i < 26; i++) {
   	if (strcmp(input, morse_code[i]) == 0) { //Based on: https://www.geeksforgeeks.org/strcmp-in-c/
       seven_segment_show(values[i]);
	  // printf(morse_code[i]);
       return;
       }
   }
       printf("Error: This morse code is unrecognized.\n");
   }

void checkButton() {
	if (timePressed < DOT_THRESHOLD) { 
		strcat(morse_input, ".");
		for (int i = 0; i < 4; i++)
            printf("%c ", morse_input[i]);
		//printf(".");
 
	} else { 
		strcat(morse_input, "-");
		for (int i = 0; i < 4; i++)
            printf("%c ", morse_input[i]);

		//printf("-");
	}
	morse_input_index++;
	
}

