/*	Author: Artip Nakchinda
 *	Assignment: Custom Project
 *	Exercise Description: Oscillating DC Fan
 *                      - Project similar to your everyday fan.
 *                      - Will implement Oscillation (Servo motor), actual fan (DC motor)
 *                      - Fan status (LCD displays) + animation (LCD matrix)
 *                      - Temperature Mode (Temperature module)
 *                      - IR Remote (Remote-operated fan, including buttons)
 * 
 *  Video Demo: https://drive.google.com/file/d/1oZcxA3mG6k_JgHkmsC2MGWG3xBk2Dn1h/view?usp=sharing
 *      - Current progress: Basic button functionality and display status.
 *      - Other display that will be used for animation still in progress.
 *      - Goal was to make the user interface function properly.
 *      - Of course, this week's progression did not implement new complexities yet.
 *      - Next goals: Implement second LCD screen (fan animation)
 *              - May need to look into implementing two atmega microcontrollers
 * 
 *      - Might re-implement status display with a nokia display instead
 *          - Fan animation will still be used for second screen.
 *              (have two nokia screens. will try to implement both in.)
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


/* To Do: (mark x in the parentheses when completed)
 *  -( ) Implement LCD screens
 *      - Display fan status (Can be regular LCD or nokia screen)
 *      - Fan Animation (nokia screen. More control over pixels) (kinda tricky)
 *          - Static fan image when the system says the fan is off.
 *          - Moving fan animation when the system says the fan is on.
 *              - This one may be the tricky part. Need at least two frames of fan image
 *                  with their blades in different positions to show the fan animation.
 *  -( ) Implement Fan oscillator
 *      - Program the servo motor so when a button is pressed, it can start oscillating
 *      - Let it turn all the way right, wait for 3 seconds, then go the other way.
 *  -( ) Implement DC motor for the main fan function
 *      - Program the motor so that it turns on when the user presses a button
 *      - button toggles the fan on/off
 *      - one other button toggles through its speeds.
 *           - There will be 4 preset speeds that the fan can cycle through
 *           - Each press of the button will cycle up each speed.
 *           - When the max setting has been reached and the button is pressed,
 *              the speed will be set to the lowest preset and cycle through the speeds again
 *  -( ) Implement Temperature Sensor
 *      - The temperature sensor will sense how warm the room is.
 *      - It will tell the fan to turn on if sensor senses that the room temp > preset temp
 *        Otherwise, off.
 *          (English: Too hot, turn fan on. Too cold, turn fan off. That's it.)
 *      - It will only work when the user has it toggled on (button says temp mode is ON)
 *  -( ) Implement IR Remote
 *      - This one will be a little tricky. Need to figure out what signals the remote sends to the receiver
 *      - Then have the microcontroller intrepret which signal turns on/off the fan, oscillates it,
 *          change speeds on it, or have it toggle the temperature sensor mode.
*/

#include <avr/io.h>
#include "io.h"
#include "timer.h"
// #include "scheduler.h"

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char tempB = 0x00;
unsigned char tempD = 0x00;

#define maxSpeed 4 
unsigned char speeds[maxSpeed] = {1, 2, 3, 4};
unsigned char pos_speed = 0;
unsigned char tempThreshold = 0x00;
unsigned char tempCurrent = 0x00;

unsigned char fanOn = 0x00; // DC motor status variable
unsigned char oscillateOn = 0x00; // oscillator status variable
unsigned char tempMode = 0x00; // temperature mode status variable

enum fanStates{F_start, F_wait, F_off, F_on, F_setSpeed, F_tempMode, F_oscillate, F_press} F_state;
void F_Tick() {
    tempD = ~PIND;
    switch(F_state) { // transitions
        case F_start:
            F_state = F_wait;
            break;
        case F_wait:
            if((tempD & 0x0F) == 0x08 && fanOn == 0x00) {
                F_state = F_on;
            }
            else if((tempD & 0x0F) == 0x08 && fanOn == 0x01) {
                F_state = F_off;
            }
            else if((tempD & 0x0F) == 0x04) {
                F_state = F_setSpeed;
            }
            else if((tempD & 0x0F) == 0x02) {
                F_state = F_oscillate;
            }
            else if((tempD & 0x0F) == 0x01) {
                F_state = F_tempMode;
            }
            else {
                F_state = F_wait;
            }
            break;
        case F_off:
            F_state = F_press;
            break;
        case F_on:
            F_state = F_press;
            break;
        case F_setSpeed:
            F_state = F_press;
            break;
        case F_tempMode:
            F_state = F_press;
            break;
        case F_oscillate:
            F_state = F_press;
            break;
        case F_press:
            if((tempD & 0x0F) != 0x00) {
                F_state = F_press;
            }
            else {
                F_state = F_wait;
            }
            break;
        default:
            F_state = F_start;
            break;
    }

    switch(F_state) { // actions
        case F_start:
            break;
        case F_wait:
            // LCD_DisplayString(1, "Wait state"); // test line
            break;
        case F_off:
            fanOn = 0x00;
            LCD_Cursor(5);
            LCD_WriteData('O');
            LCD_WriteData('f');
            LCD_WriteData('f');
            LCD_Cursor(0);
            break;
        case F_on:
            fanOn = 0x01;
            LCD_Cursor(5);
            LCD_WriteData('O');
            LCD_WriteData('n');
            LCD_WriteData(' ');
            LCD_Cursor(0);
            break;
        case F_setSpeed:
            if(tempMode == 0x00) {
                pos_speed++;
                LCD_Cursor(21);
                if(pos_speed == maxSpeed) {
                    pos_speed = 0;
                    LCD_WriteData(speeds[pos_speed] + '0');
                } else {
                    LCD_WriteData(speeds[pos_speed] + '0');
                }
                LCD_Cursor(0);
            }
            break;
        case F_tempMode:
            if(tempMode == 0x00) {
                tempMode = 0x01;
                LCD_Cursor(21);
                LCD_WriteData('T');
                LCD_WriteData('e');
                LCD_WriteData('m');
                LCD_WriteData('p');
                LCD_Cursor(0);
            } else {
                tempMode = 0x00;
                LCD_Cursor(21);
                LCD_WriteData(' ');
                LCD_WriteData(' ');
                LCD_WriteData(' ');
                LCD_WriteData(' ');
                
                LCD_Cursor(21);
                LCD_WriteData(speeds[pos_speed] + '0');
                LCD_Cursor(0);
            }
            break;
        case F_oscillate:
            if(oscillateOn == 0x00) {
                oscillateOn = 0x01;
                LCD_Cursor(13);
                LCD_WriteData('O');
                LCD_WriteData('n');
                LCD_WriteData(' ');
                LCD_Cursor(0);
            } else {
                oscillateOn = 0x00;
                LCD_Cursor(13);
                LCD_WriteData('O');
                LCD_WriteData('f');
                LCD_WriteData('f');
                LCD_Cursor(0);
            }
            break;
        case F_press:
            break;
        default:
            break;
    }

    tempB = fanOn + (oscillateOn << 1) + (tempMode << 4);

    PORTB = tempB;

}

enum oscillatorStates{O_start, off, on} O_state;

enum display1_States{d1_start, d1_output} d1_state;
void d1_Tick() {
    switch(d1_state) { // transitions
        case d1_start:
            break;
        case d1_output:
        default:
            break;
    }
    switch(d1_state) { // state actions
        case d1_start:

            break;
        case d1_output:

            break;
        default:
            d1_state = d1_start;
            break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    // Port intializations are tentative.
    // May change the location of inputs/outputs around to figure out which one goes where.
    // 
    DDRA = 0xFF; PORTA = 0x00; // Output: LCD1 (Fan Status) (Data line)
    DDRB = 0xFF; PORTB = 0x00; // Output: Fan motor, oscillator, and LEDs (?)
    DDRC = 0x00; PORTC = 0xFF; // Input: Temp sensor
    DDRD = 0xF0; PORTD = 0x0F; // Input: Buttons and IR remote/receiver<?> (LCD control)
    // DDRA = 0xFF; PORTA = 0x00; // LCD data lines
    // DDRD = 0xFF; PORTD = 0x00; // LCD control lines

    // LCD 2? Not sure. Might have to implement a second microcontroller that receives signal
    //      from the main microcontroller
    unsigned long F_elapsedTime = 0;
    const unsigned long timerPeriod = 10;

    tempD = ~PIND;

    F_state = F_start;

    LCD_init();
    LCD_ClearScreen();

    TimerSet(100);
    TimerOn();
    
    LCD_DisplayString(1, "Pwr:Off Osc:Off Spd:1          ");
    LCD_Cursor(0);
    // LCD_DisplayString(17, "Oscillate: ");
    // "Pwr:    Osc:    Spd:           "

    /* Insert your solution below */
    while (1) {
        if(F_elapsedTime >= 10) {
            F_Tick();
            F_elapsedTime = 0;
        }

        while(!TimerFlag) {}
        TimerFlag = 0;

        F_elapsedTime += timerPeriod;
    }
    return 1;
}
