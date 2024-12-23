#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include <stdlib.h>
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
#define TOO_LONG 700 //if button is pressed more than 700ms display error message
#define DOT_THRESHOLD 250 // Less than 250ms = dot, more than 250ms = dash
#define INTERLETTER 400   // More than 400ms = new letter
#define BUTTON2_PIN 16    // RIGHT
#define BUTTON1_PIN 22    // LEFT
////////////////////

// Declare global variables
uint32_t start_time, end_time, timePressed, pause_start, pause_duration, letter_time_start;
char morse_input[69]; // Store up to 4 symbols + null terminator //array of morse input
int morse_input_index = 0; //index each dot and dash stored
int frequency = 500;
int letter_count = 0;
char decoded_letters[5];//array of letters decoded
unsigned int limit = 4000;
bool time_expired = false;
bool letter_time_started = false;


///////////declarations of functions
void errorSong();
void potentiometer_init();
unsigned int potentiometer_read();
int main();

// Morse code dictionary
// References
// printing Based: https://www.geeksforgeeks.org/printf-in-c/
//
const char morse_code[26][5] = {
    ".-",   // a
    "-...", // b
    "-.-.", // c
    "-..",  // d
    ".",    // e
    "..-.", // f
    "--.",  // g
    "....", // h
    "..",   // i
    ".---", // j
    "-.-",  // k
    ".-..", // l
    "--",   // m
    "-.",   // n
    "---",  // o
    ".--.", // p
    "--.-", // q
    ".-.",  // r
    "...",  // s
    "-",    // t
    "..-",  // u
    "...-", // v
    ".--",  // w
    "-..-", // x
    "-.--", // y
    "--.."  // z
};

// Utility Functions
// Based: https://stackoverflow.com/questions/48833976/failing-to-understand-what-the-expression-uint32-t-does#:~:text=uint32_t%20is%20for%20unsigned%2032,2%2C%202020%20at%203%3A13
uint32_t time_ms()
{
    return to_ms_since_boot(get_absolute_time());
}
///////////////////////////////////////////////////////////////
void show_rgb(int r, int g, int b)//Based on: Lab 8 worksheet
{
    pwm_set_gpio_level(R, ~(MAX_PWM_LEVEL * r / MAX_COLOUR_VALUE * BRIGHTNESS / 100));
    pwm_set_gpio_level(G, ~(MAX_PWM_LEVEL * g / MAX_COLOUR_VALUE * BRIGHTNESS / 100));
    pwm_set_gpio_level(B, ~(MAX_PWM_LEVEL * b / MAX_COLOUR_VALUE * BRIGHTNESS / 100));
}
///////////////////////////////////////////////////////////////
void errorDisplay() //this function is called when an error occurs due to user fault eg. button pressed too long
{
    memset(morse_input, 0, sizeof(morse_input)); //Based on: https://www.geeksforgeeks.org/memset-c-example/
    morse_input_index = 0;
    seven_segment_init();
    show_rgb(255, 0, 0);
    errorSong();
    sleep_ms(1000);
    seven_segment_off();
    show_rgb(0, 0, 0);
}

///////////////////////////////////////////////////////////////


void potentiometerSettings() //this function is to allow the user to set a time limit for each letter
{
    printf("(Left) Set your own time limit!\n(Right) Keep default time limit! (4 seconds)\n");

    while (true)
    {
        if (gpio_get(BUTTON1_PIN) && !gpio_get(BUTTON2_PIN)) //if the user chose to set time limit using potentiometer
        { // if button1 (left) IS and button2 (right) is NOT pressed
            printf("\nSet your potentiometer now, then press left button to confirm\nTime Limit:");
            sleep_ms(200);
            while (!gpio_get(BUTTON1_PIN))
            {
                unsigned int value = potentiometer_read(4);
                seven_segment_show_numbers(value);
                limit = value * 1000;
                sleep_ms(20);
            }
            printf(" %d seconds\n", limit / 1000);
            // printf("You can now enter your morse code!!");
            sleep_ms(200);
            letter_time_start = time_ms();
            seven_segment_init();
            sleep_ms(500);
            seven_segment_off();
            break;
        }
        else if (!gpio_get(BUTTON1_PIN) && gpio_get(BUTTON2_PIN))
        { // if button1 (left) is NOT and button2 (right) IS pressed
            limit = 4000;
            printf("Default limit: %d seconds \n", limit / 1000);
            break;//Based on: https://www.geeksforgeeks.org/c-break-statement/
        }
        else if (gpio_get(BUTTON1_PIN) && gpio_get(BUTTON2_PIN))
        { // if both pressed
            printf("Don't press both at the same time!!! >;( try again!!\n");
            errorDisplay();
        }
    }
}

///////////////////////////////////////////////////////////////

void correctSong() //this is called when correct morse code is entered
{
    unsigned int song[] = {NOTE_A4, NOTE_C5, NOTE_E5, NOTE_A5, NOTE_C5, NOTE_A4};
    unsigned int songLength = sizeof(song) / sizeof(song[0]);

    for (unsigned int i = 0; i < songLength; i++)
    {
        buzzer_enable(song[i]);
        sleep_ms(100);
        buzzer_quiet();
        sleep_ms(50);
    }
    buzzer_quiet();
}

///////////////////////////////////////////////////////////////


void holdLetters(char letter) //this method is to hold the decoded letters of the morse code entered by user in the decoded_letters array
{
    if (letter_count < 4) //if the array is not full the decoded letter will be added
    {
        decoded_letters[letter_count++] = letter;
        decoded_letters[letter_count] = '\0';
    }
    if (letter_count == 4) //if array is full display the letters added so far (4 characters)
    {
        // make a song here
        
        printf("Your message: %s\n\n", decoded_letters);
        correctSong();
        show_rgb(0, 255, 0);
        sleep_ms(500);
        show_rgb(0, 0, 0);
        sleep_ms(200);
        printf("Would you like to continue?\n");
        printf("(Yes) Left\n");
        printf(" (No) Right\n");

        while (1)
        {
            if (gpio_get(BUTTON1_PIN) && !gpio_get(BUTTON2_PIN)) //if left button is pressed user starts from the beginning
            { // if left button pressed
            //every component is resetted including the array of letters and morse code
                memset(morse_input, 0, sizeof(morse_input));
                letter_count = 0;
                time_expired = false;
                pause_start = 0;
                printf("Starting over!\n");
                sleep_ms(200);
                show_rgb(0, 255, 0);
                sleep_ms(1000);
                show_rgb(0, 0, 0);
                morse_input_index = 0;
                letter_time_started = false;
                main();
            }
            else if (!gpio_get(BUTTON1_PIN) && gpio_get(BUTTON2_PIN))//is the right button is pressed the program is terminated
            { // if right button pressed
                printf("Goodbye!");
                show_rgb(255, 0, 0);
                sleep_ms(1000);
                show_rgb(0, 0, 0);
                exit(0);//Based on: https://www.geeksforgeeks.org/exit0-vs-exit1-in-c-c-with-examples/
            }
        }
    }
}

///////////////////////////////////////////////////////////////


void decoder(const char *input)//decoder function translates morse code entered to alphabatical letters
{
    for (int i = 0; i < 26; i++)
    {//the morse code entered gets compared to each morse code in the morse_code array, if there is a match the index i is taken and the letter is displayed on the seven segment
    //this works because the index of each morse code letter matches the alphabetical letter display in values array in seven_segment.h
        if (strcmp(input, morse_code[i]) == 0) //Based on: https://www.freecodecamp.org/news/strcmp-in-c-how-to-compare-strings-in-c/
        {
            seven_segment_init();
            seven_segment_show(i);
            show_rgb(0, 255, 0);
            sleep_ms(1000);
            seven_segment_off();
            show_rgb(0, 0, 0);
            char decoded_letter = 'A' + i; // Based on: https://www.geeksforgeeks.org/c-ascii-value/
            printf("Letter: %c\n", decoded_letter);
            holdLetters(decoded_letter); //the decoded letter gets added to the decoded_letters array
            return;
        }
    }
    //if the input does not match any morse code error message gets displayed along with red light
    printf("Error: This morse code does not exist.\n");
    show_rgb(255, 0, 0);
    seven_segment_init();
    errorSong();
    sleep_ms(500);
    seven_segment_off(); // Turn off display for errors
    show_rgb(0, 0, 0);
}

///////////////////////////////////////////////////////////////

void Letter()
{
    if (morse_input_index > 0)
    {                         // Ensure there's input to decode
        decoder(morse_input); //calls decoder function to translate
    }

    // Reset for the next letter
    memset(morse_input, 0, sizeof(morse_input));
    morse_input_index = 0;
    pause_start = 0;
    letter_time_started = false;
}

///////////////////////////////////////////////////////////////

void checkLetterTime() // to check if its over the limit yet
{   if(time_expired || morse_input_index == 0){
        return; //Skip if time_expired or no input yet
    }   
    if ((time_ms() - letter_time_start) > limit)
    { // if longer than limit
        printf("Time limit expired\n");
        seven_segment_init();
        show_rgb(255, 0, 0);
        errorSong();
        sleep_ms(500);
        seven_segment_off();
        show_rgb(0, 0, 0);
        sleep_ms(200);
        Letter();
        memset(morse_input, 0, sizeof(morse_input)); 
        morse_input_index = 0;
        time_expired = true;
        letter_time_started = false;
        return;
    }
    else
    {
        time_expired = false;
    }
}

///////////////////////////////////////////////////////////////

void checkButton()
{
    while (!gpio_get(BUTTON1_PIN))
    {
        buzzer_quiet();
        if (letter_time_started == true)
        {
            checkLetterTime();
            if (time_expired == true)
            {
                return;
            }
        }
        // button not pressed, check for inter-letter pause
        if (pause_start > 0)
        {
            pause_duration = time_ms() - pause_start;
            if (pause_duration > INTERLETTER && morse_input_index > 0)
            {
                Letter();
                return;
            }
        }
        sleep_ms(20);
    }

    // button pressed
    start_time = time_ms();
    if (morse_input_index == 0)
    { // if index is 0 therefore first letter
        letter_time_start = time_ms();
        letter_time_started = true;
        time_expired = false;
    }
    // checkLetterTime();

    while (gpio_get(BUTTON1_PIN))
    { // while button is pressed
        buzzer_enable(frequency);
        sleep_ms(20);
    }

    end_time = time_ms();
    timePressed = end_time - start_time;
    pause_start = time_ms(); // pause starts

    if (morse_input_index < 4)
    {
        if (timePressed > TOO_LONG)
        { // if the buttton was pressed for too long
            printf("Error: Button pressed for too long.\n");
            errorDisplay();
            letter_time_started = false;
            pause_start = 0;
            time_expired = false;
        }
        else if (timePressed < DOT_THRESHOLD)
        {
            strcat(morse_input, ".");
            morse_input_index++;
        }
        else
        {
            strcat(morse_input, "-");
            morse_input_index++;
        }
    }
    else
    {
        printf("Error: Input exceeds limits.\n");
        errorDisplay();
         letter_time_started = false;
         pause_start = 0;
         time_expired = false;
    }

    // Debug: Print current Morse input
    if (morse_input_index > 0)
    {
        printf("Morse input: %s\n", morse_input);
    }
    if (letter_time_started = true){
        checkLetterTime();
    if (time_expired == true)
    {
        return;
    }}

}

///////////////////////////////////////////////////////////////

void setup_rgb() //Based on: Lab 8 Worksheet
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

///////////////////////////////////////////////////////////////

void errorSong() //this method is called when there is a user caused error eg. wrong morse code
{
    unsigned int song[] = {A4,B4,A4,B4,A4,B4,E3};
    unsigned int songLength = sizeof(song) / sizeof(song[0]);

    for (unsigned int i = 0; i < songLength; i++)
    {
        buzzer_enable(song[i]);
        sleep_ms(100);
        buzzer_quiet();
        sleep_ms(50);
    }
    buzzer_quiet();
}

///////////////////////////////////////////////////////////////

int main()
{
    timer_hw->dbgpause = 0;
    stdio_init_all();
    buzzer_init();
    potentiometer_init();
    setup_rgb();
    show_rgb(0, 0, 0);

    gpio_init(BUTTON1_PIN);
    gpio_set_dir(BUTTON1_PIN, GPIO_IN);
    gpio_pull_down(BUTTON1_PIN);
    printf("----------------------------------------------------------\n");

    printf("Welcome!\n");
    seven_segment_init();
    sleep_ms(1000);
    seven_segment_off();
    

    potentiometerSettings();
    letter_time_start = false;

    while (true)
    {
        checkButton();
    }

    return 0;
}