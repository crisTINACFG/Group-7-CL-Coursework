#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include "includes/seven_segment.h"
#include "hardware/gpio.h"
#include <time.h>
#include "hardware/pwm.h"
#include "includes/buzzer.h"
#include "hardware/timer.h"

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
#define TOO_LONG      700
#define DOT_THRESHOLD 250      // Less than 250ms = dot, more than 250ms = dash
#define INTERLETTER 400        // More than 400ms = new letter
#define BUTTON2_PIN 16      // RIGHT  
#define BUTTON1_PIN 22      // LEFT
////////////////////

//Declare global variables
uint32_t start_time, end_time, timePressed, pause_start, pause_duration;
char morse_input[5];           // Store up to 4 symbols + null terminator
int morse_input_index = 0;
int frequency = 500;
int letter_count = 0;
char decoded_letters[69];
unsigned int limit = 4000;
volatile bool time_expired = false;

//Morse code dictionary
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

//Utility Functions
uint32_t time_ms() {
    return to_ms_since_boot(get_absolute_time());
}

//Timer callback
bool letter_timer_callback(repeating_timer_t *rt) {
    time_expired = true; // Set the flag when timer expires
    return false;        // Don't repeat the timer
}

//Start timer for letter timeout
void start_letter_timer(uint32_t duration_ms) {
    static repeating_timer_t timer;
    time_expired = false; // Reset the flag
    add_repeating_timer_ms(-duration_ms, letter_timer_callback, NULL, &timer);
}

void potentiometerSettings() {
    printf("Please set a time limit:\n(1) Press left button to set limit on potentiometer\n(2) Press right button to keep default\n");
    
     while(true){
        if(gpio_get(BUTTON1_PIN) && !gpio_get(BUTTON2_PIN)){//if button1 (left) IS and button2 (right) is NOT pressed
            printf("\nSet your potentiometer now, then press left button to confirm:\n");
            sleep_ms(200);
            while(!gpio_get(BUTTON1_PIN)){
                unsigned int value = potentiometer_read(4);
                seven_segment_show_numbers(value);
                limit = value *1000;
                sleep_ms(20);
            }
            printf("You set time limit is now set to: %d seconds\n", limit/1000);
            sleep_ms(200);
            letter_start = time_ms();
            break;
        } else if(!gpio_get(BUTTON1_PIN) && gpio_get(BUTTON2_PIN)){//if button1 (left) is NOT and button2 (right) IS pressed
            printf("Default limit: %d seconds \n",limit/1000);
            break;
        } else if (gpio_get(BUTTON1_PIN) && gpio_get(BUTTON2_PIN)) { //if both pressed
            printf("Don't press both at the same time!!! >;( try again!!\n");
            errorDisplay();
        }

    }
}

void checkButton() {
    while (!gpio_get(BUTTON1_PIN)) {
        buzzer_disable();
        // Button not pressed, check for inter-letter pause
        if (pause_start > 0) {
            pause_duration = time_ms() - pause_start;
            if (pause_duration > INTERLETTER && morse_input_index > 0) {
                Letter();
                pause_start = 0; // Reset pause timing
                return;
            } 
        }
        sleep_ms(20);
    }
    sleep_ms(200);
    
    // Button pressed
    start_time = time_ms();
    if (morse_input_index == 0) { //if index is 0 therefore first letter
        start_letter_timer(limit); //start timer for new letter
    }

    while (gpio_get(BUTTON1_PIN)) {
        buzzer_enable(frequency);
        sleep_ms(20);
    }

    // Button released
    end_time = time_ms();
    timePressed = end_time - start_time;
    pause_start = time_ms(); // Mark pause start

    if (morse_input_index < 4) {
        if (timePressed < DOT_THRESHOLD) {
            strcat(morse_input, ".");
        } else {
            strcat(morse_input, "-");
        }
        morse_input_index++;
    } else {
        printf("Error: Input exceeds limits.\n");
        memset(morse_input, 0, sizeof(morse_input));
        morse_input_index = 0;
        seven_segment_show(27);
        show_rgb(255,0,0);
        errorSong();
        sleep_ms(400);
        show_rgb(0,0,0);
    }

        // Debug: Print current Morse input
    if(morse_input_index > 0){
        printf("Morse input: %s\n", morse_input);
    }
    if(timePressed > TOO_LONG){
        printf("Error: Button pressed for too long.\n");
        memset(morse_input, 0, sizeof(morse_input));
        morse_input_index = 0;
        seven_segment_show(27);
        show_rgb(255,0,0);
        errorSong();
        sleep_ms(400);
        show_rgb(0,0,0);
    }
}

void Letter() {
    // Decode and display the letter
    cancel_repeating_timer(&timer);
    decoder(morse_input);

    memset(morse_input, 0, sizeof(morse_input));
    morse_input_index = 0;
    pause_start = 0;
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
            char decoded_letter = 'A' + i;
            holdLetters(decoded_letter);
            return;
        }
    }

    printf("Error: This morse code does not exist.\n");
    show_rgb(255,0,0);
    seven_segment_show(27);
    errorSong();

    seven_segment_off(); // Turn off display for errors
    show_rgb(0,0,0);
}

void errorDisplay(){
    //letterTime_start = 0;
    seven_segment_init();
    show_rgb(255,0,0);
    errorSong();
    sleep_ms(1000);
    seven_segment_off();
    show_rgb(0,0,0);
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

void holdLetters(char letter){
        if (letter_count < 4){
        decoded_letters[letter_count++] = letter;
        decoded_letters[letter_count] = '\0';
        printf("decoded letters: %s\n" , decoded_letters);
        } 
        if(letter_count == 4){
            printf("Would you like to continue?\n"); 
            printf("Click right to continue, left otherwise: \n");
            
            while(1){
                if(gpio_get(BUTTON1_PIN) && !gpio_get(BUTTON2_PIN)) {
                    memset(morse_input, 0, sizeof(morse_input));
                    letter_count = 0;
                    printf("starting over\n");
                    show_rgb(0,255,0);
                    sleep_ms(1000);
                    show_rgb(0,0,0);
                    break;
                    
                }  else if(!gpio_get(BUTTON1_PIN) && gpio_get(BUTTON2_PIN)) {
                    printf("Leave me then...");
                    show_rgb(255,0,0);
                    sleep_ms(1000);
                    show_rgb(0,0,0);
                    exit(0);
                }
            }
        }
    }
        
void welcome_song() {
    unsigned int song[] = {G,AS4, A, C};
    unsigned int songLength = sizeof(song)/sizeof(song[0]);

    for (unsigned int i = 0; i < songLength; i++){
        buzzer_enable(song[i]);
        sleep_ms(1000);
        buzzer_disable();
        sleep_ms(500);
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

int main() { 
    timer_hw->dbgpause = 0;
    stdio_init_all();
    buzzer_init();
    potentiometer_init();
    //welcome_song();
    setup_rgb();
    show_rgb(0,0,0);

    printf("Welcome\n");
    seven_segment_init();
    sleep_ms(1000);
    seven_segment_off();
    potentiometerSettings();

    gpio_init(BUTTON1_PIN);
    gpio_set_dir(BUTTON1_PIN, GPIO_IN);
    gpio_pull_down(BUTTON1_PIN);

    while (true) {
        checkButton();
    }

    return 0;
}
