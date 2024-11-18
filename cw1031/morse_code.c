#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "includes/seven_segment.h"
#include "hardware/gpio.h"
#include <time.h>
#include "hardware/pwm.h"
#include "includes/buzzer.h"


////////////rgb
#define R 13 
#define G 12 
#define B 11 

#define BRIGHTNESS 50
#define LOOP_SLEEP 10
#define MAX_COLOUR_VALUE 255
#define MAX_PWM_LEVEL 65535

#define UP true
#define DOWN false

////////////////////

#define DOT_THRESHOLD 250      // Less than 250ms = dot, more than 250ms = dash
#define INTERLETTER 400        // More than 400ms = new letter
#define BUTTON_PIN 16          // GPIO pin for button
#define DEBOUNCE_DELAY 200  ///to ensure clean button press duration
#define TOO_LONG 700
// Declare global variables
uint32_t start_time, end_time, timePressed, pause_start, pause_duration;
char morse_input[5];           // Store up to 4 symbols + null terminator
int morse_input_index = 0;
unsigned int frequency = 500;

// Function prototypes
void checkButton();
void decoder(const char *input);
void displayLetter();
void welcome_song();
void setup_rgb();
void show_rgb();
void errorDisplay();
void errorSong();

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
    welcome_song();

    setup_rgb();
    show_rgb(0,0,0);

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
    while (!gpio_get(BUTTON_PIN)) { //if button is NOT pressed
        buzzer_disable();
        // Button not pressed, check for inter-letter pause
        if (pause_start > 0) {
            pause_duration = time_ms() - pause_start;
            if (pause_duration > INTERLETTER && morse_input_index > 0) {
                displayLetter();
            }
        }
        sleep_ms(20);
    }

    // Button pressed
    start_time = time_ms();
    while (gpio_get(BUTTON_PIN)) { //if button IS pressed
        buzzer_enable(frequency);
        sleep_ms(20);
    }

    // Button released
    buzzer_disable();
    end_time = time_ms();
    timePressed = end_time - start_time;
    pause_start = time_ms(); // Mark pause start

    if (timePressed < TOO_LONG){ //if not pressed for longer than 700 ms
        if (morse_input_index < 4) { //if array still has space for more symbols
            if (timePressed < DOT_THRESHOLD) {
                strcat(morse_input, ".");
                morse_input_index++;
            } else {
                strcat(morse_input, "-");
                morse_input_index++;
            }
        } 
        else { //else array is full (max 4 symbols for a morse letter)
            printf("Error: Input exceeds limits.\n");
            memset(morse_input, 0, sizeof(morse_input));
            morse_input_index = 0;
            errorDisplay();
        }

            // Debug: Print current Morse input
            if(morse_input_index > 0){
                printf("Morse input: %s\n", morse_input);
            }
        } else { //pressed button for more than 700 ms
            printf("Error: Button pressed longer than 700ms\n");
            errorDisplay();
        }
    } //end of checkbutton

void errorDisplay(){
    seven_segment_init();
    show_rgb(255,0,0);
    errorSong();
    sleep_ms(1000);
    seven_segment_off();
    show_rgb(0,0,0);
}
void displayLetter() {
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
            show_rgb(0,255,0);
            sleep_ms(1000);
            seven_segment_off();
            show_rgb(0,0,0);
            //sleep_ms(1000);
            printf("Letter detected: %c\n", 'A' + i);

            return;
        }
    }

    printf("Error: This morse code does not exist, this is an error input.\n");
	errorDisplay();
}


void setup_rgb()
{
    // Tell the LED pins that the PWM is in charge of its value.
    gpio_set_function(R, GPIO_FUNC_PWM);
    gpio_set_function(G, GPIO_FUNC_PWM);
    gpio_set_function(B, GPIO_FUNC_PWM);

    // Figure out which slice we just connected to the LED pin, this is done for the other colors below
    uint slice_num = pwm_gpio_to_slice_num(R);
    // Get the defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);

    slice_num = pwm_gpio_to_slice_num(G);
    pwm_init(slice_num, &config, true);

    slice_num = pwm_gpio_to_slice_num(B);
    pwm_init(slice_num, &config, true);
}

void show_rgb(int r, int g, int b)
{
    pwm_set_gpio_level(R, ~(MAX_PWM_LEVEL * r / MAX_COLOUR_VALUE * BRIGHTNESS / 100));
    pwm_set_gpio_level(G, ~(MAX_PWM_LEVEL * g / MAX_COLOUR_VALUE * BRIGHTNESS / 100));
    pwm_set_gpio_level(B, ~(MAX_PWM_LEVEL * b / MAX_COLOUR_VALUE * BRIGHTNESS / 100));
}

void welcome_song() {
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
