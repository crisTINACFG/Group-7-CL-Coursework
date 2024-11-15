#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "includes/seven_segment.h"
#include "hardware/gpio.h"
#include <time.h>

#define DOT_THRESHOLD 250      // Less than 250ms = dot, more than 250ms = dash
#define INTERLETTER 400        // More than 400ms = new letter
#define BUTTON_PIN 16          // GPIO pin for button

// Declare global variables
uint32_t start_time, end_time, timePressed, pause_start, pause_duration;
char morse_input[5];           // Store up to 4 symbols + null terminator
int morse_input_index = 0;

// Function prototypes
void processButtonInput();
void decodeAndDisplay(const char *input);
void handleNewLetter();

// Morse code dictionary
const char morse_code[26][5] = {
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

    printf("Welcome\n");
    seven_segment_init();
    sleep_ms(1000);
    seven_segment_off();

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);

    while (true) {
        processButtonInput();
    }

    return 0;
}

void processButtonInput() {
    while (!gpio_get(BUTTON_PIN)) {
        // Button not pressed, check for inter-letter pause
        if (pause_start > 0) {
            pause_duration = time_ms() - pause_start;
            if (pause_duration > INTERLETTER && morse_input_index > 0) {
                handleNewLetter();
            }
        }
        sleep_ms(20);
    }

    // Button pressed
    start_time = time_ms();
    while (gpio_get(BUTTON_PIN)) {
        sleep_ms(20);
    }

    // Button released
    end_time = time_ms();
    timePressed = end_time - start_time;
    pause_start = time_ms(); // Mark pause start

    if (timePressed < DOT_THRESHOLD) {
        if (morse_input_index < 4) {
            strcat(morse_input, ".");
            morse_input_index++;
        } else {
            printf("Error: Morse input buffer overflow.\n");
        }
    } else {
        if (morse_input_index < 4) {
            strcat(morse_input, "-");
            morse_input_index++;
        } else {
            printf("Error: Morse input buffer overflow.\n");
        }
    }

    // Debug: Print current Morse input
    printf("Morse input: %s\n", morse_input);
}

void handleNewLetter() {
    // Decode and display the letter
    decodeAndDisplay(morse_input);
    memset(morse_input, 0, sizeof(morse_input));
    morse_input_index = 0;
}

void decodeAndDisplay(const char *input) {
    for (int i = 0; i < 26; i++) {
        if (strcmp(input, morse_code[i]) == 0) {
            seven_segment_init();
            seven_segment_show(values[i]);
            sleep_ms(500);
            seven_segment_off();
            sleep_ms(500);
            printf("Letter detected: %c\n", 'A' + i);
            
            return;
        }
    }

    printf("Error: Unrecognized Morse code.\n");
    seven_segment_off(); // Turn off display for errors
}


