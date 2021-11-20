/*	Author: Artip Nakchinda
 *	Assignment: Custom Project
 *	Exercise Description: Oscillating DC Fan
 *                      - Project similar to your everyday fan.
 *                      - Will implement Oscillation (Servo motor), actual fan (DC motor)
 *                      - Fan status (LCD displays) + animation (LCD matrix)
 *                      - Temperature Mode (Temperature module)
 *                      - IR Remote (Remote-operated fan, including buttons)
 *  Video Demo: 
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

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0x00;
    DDRB = 0x00; PORTB = 0x00;
    DDRC = 0x00; PORTC = 0x00;
    DDRD = 0x00; PORTD = 0x00;

    /* Insert your solution below */
    while (1) {

    }
    return 1;
}
