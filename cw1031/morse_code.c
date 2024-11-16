#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "includes/seven_segment.h"
#include "hardware/gpio.h"
#include <time.h>
#include "hardware/pwm.h"
#include "includes/buzzer.h"

#define DOT_THRESHOLD 250      // Less than 250ms = dot, more than 250ms = dash
#define INTERLETTER 400        // More than 400ms = new letter
#define BUTTON_PIN 16          // GPIO pin for button
#define DEBOUNCE_DELAY 200  ///to ensure clean button press duration
// Declare global variables
uint32_t start_time, end_time, timePressed, pause_start, pause_duration;
char morse_input[5];           // Store up to 4 symbols + null terminator
int morse_input_index = 0;
int frequency = 500;

// Function prototypes
void checkButton();
void decoder(const char *input);
void Letter();
void welcome_song();

void playNote( int frequency){
    buzzer_enable(frequency);
}
void welcome_song() {
    unsigned int song[] = {C,C,G,C,G};
    unsigned int songLength = sizeof(song)/sizeof(song[0]);

    for (unsigned int i = 0; i < songLength; i++){
        buzzer_enable(song[i]);
        sleep_ms(100);
        buzzer_disable();
        sleep_ms(100);
    }
    //buzzer_disable();
}
void errorSong(){
    unsigned int song[] = {A4,B4,A4,B4,A4,B4,E3};
    unsigned int songLength = sizeof(song)/sizeof(song[0]);

    for (unsigned int i = 0; i < songLength; i++){
        buzzer_enable(song[i]);
        sleep_ms(100);
        buzzer_disable();
        sleep_ms(50);
    }
    buzzer_disable();

}

// Morse code dictionary
const char morse_code[26][5] = {
    ".-", //a
	"-...", //b
	"-.-.", //c
	"-..", //d
	".", //e
	"..-.", //f
	"--.", //g
	"....", //h
	"..", //i
	".---", //j
    "-.-", //k
	".-..", //l
	"--", //m
	"-.", //n
	"---", //o
	".--.", //p
	"--.-", //q
	".-.", //r
	"...", //s
	"-", //t
    "..-", //u
	"...-", //v
	".--", //w
	"-..-", //x
	"-.--", //y
	"--.."//z
};

uint32_t time_ms() {
    return to_ms_since_boot(get_absolute_time());
}

int main() { 
    timer_hw->dbgpause = 0;
    stdio_init_all();
    buzzer_init();
    //buzzer_disable();
    //welcome_song();

    printf("Welcome\n");
    seven_segment_init();
    sleep_ms(1000);
    seven_segment_off();
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);

    while (true) {
        checkButton();
    }

    return 0;
}

void checkButton() {
    //buzzer_disable();
    while (!gpio_get(BUTTON_PIN)) {
        buzzer_disable();
        // Button not pressed, check for inter-letter pause
        if (pause_start > 0) {
            pause_duration = time_ms() - pause_start;
            if (pause_duration > INTERLETTER && morse_input_index > 0) {
                Letter();
            }
        }
        sleep_ms(20);
    }

    // Button pressed
    start_time = time_ms();
    while (gpio_get(BUTTON_PIN)) {
        buzzer_enable(frequency);
        sleep_ms(20);
    }
    //buzzer_disable();

    // Button released
    end_time = time_ms();
    timePressed = end_time - start_time;
    pause_start = time_ms(); // Mark pause start
    //buzzer_disable();

    if (timePressed < DOT_THRESHOLD) {
        if (morse_input_index < 4) {
            strcat(morse_input, ".");
            morse_input_index++;
        } else {
            printf("Error: Input exceeds limits.\n");
            errorSong();
        }
    } else {
        if (morse_input_index < 4) {
            strcat(morse_input, "-");
            morse_input_index++;
        } else {
            printf("Error: Input exceeds limits.\n");
            errorSong();

        }
    }

    // Debug: Print current Morse input
    printf("Morse input: %s\n", morse_input);
}

void Letter() {
    // Decode and display the letter
    decoder(morse_input);
    memset(morse_input, 0, sizeof(morse_input));
    morse_input_index = 0;
}

void decoder(const char *input) {
    for (int i = 0; i < 26; i++) {
        if (strcmp(input, morse_code[i]) == 0) {
            seven_segment_init();
            seven_segment_show(i);
            sleep_ms(1000);
            seven_segment_off();
            //sleep_ms(1000);
            printf("Letter detected: %c\n", 'A' + i);
            
            return;
        }
    }

    printf("Error: This morse code does not exist.\n");
    errorSong();

    seven_segment_off(); // Turn off display for errors
}



