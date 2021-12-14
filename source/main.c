/*	Author: Artip Nakchinda
 *	Assignment: Custom Project
 *	Exercise Description: Oscillating DC Fan
 *                      - Project similar to your everyday fan.
 *                      - Will implement Oscillation (Servo motor), actual fan (DC motor)
 *                      - Fan status (LCD displays) + animation (LCD matrix)
 *                      - Temperature Mode (Temperature module)
 *                      - IR Remote (Remote-operated fan, including buttons)
 * 
 *  Video Demos available in PDF Report
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
#include "ADC.h"
#include "nokia5110.h"
#include "fanbitmaps.h"

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char tempA = 0x00;
unsigned char tempB = 0x00;
unsigned char tempC = 0x00;
unsigned char tempD = 0x00;

#define maxSpeed 4 
unsigned char speeds[maxSpeed] = {1, 2, 3, 4};
unsigned char motorSpeeds[maxSpeed] = {11, 20, 40, 100};
unsigned char pos_speed = 0;
unsigned char tempThreshold = 0x00;
unsigned char tempCurrent = 0x00;

unsigned char fanOn = 0x00; // fan status variable
unsigned char oscillateOn = 0x00; // oscillator status variable
unsigned char tempMode = 0x00; // temperature mode status variable

enum fanStates{F_start, F_wait, F_off, F_on, F_setSpeed, F_tempMode, F_oscillate, F_press} F_state;
void F_Tick() {
    tempA = ~PINA;
    switch(F_state) { // transitions
        case F_start:
            F_state = F_wait;
            break;
        case F_wait:
            if((tempA & 0x0F) == 0x08 && fanOn == 0x00) {
                F_state = F_on;
            }
            else if((tempA & 0x0F) == 0x08 && fanOn == 0x01) {
                F_state = F_off;
            }
            else if((tempA & 0x0F) == 0x04) {
                F_state = F_setSpeed;
            }
            else if((tempA & 0x0F) == 0x02) {
                F_state = F_oscillate;
            }
            else if((tempA & 0x0F) == 0x01) {
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
            if((tempA & 0x0F) != 0x00) {
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

}

unsigned char motor = 0x00;
unsigned char motorEnable = 0x00;
unsigned char motorDir = 0x02; // 1 for fwd, 2 for bkwd
enum motorStates {M_start, M_off, M_on} M_state;
void M_Tick() {
    switch(M_state) { // transitions
        case M_start:
            M_state = M_off;
            break;
        case M_off:
            if(fanOn == 0x00)
                M_state = M_off;
            else if(fanOn == 0x01)
                M_state = M_on;
            break;
        case M_on:
            if(fanOn == 0x00)
                M_state = M_off;
            else if(fanOn == 0x01)
                M_state = M_on;
            break;
        default:
            M_state = M_start;
            break;
    }
    switch(M_state) { // state actions
        case M_start:
            break;
        case M_off:
            motorEnable = 0x00;
            break;
        case M_on:
            if(motor <= 10) 
                motorEnable = 0x00;
            else if (motor <= motorSpeeds[pos_speed]) 
                motorEnable = 0x01;
            else
                motor = 0;
            motor++;

            // motorEnable = 0x01;

            break;
        default:
            break;
    }
}

unsigned char servoMotor = 0x00;
unsigned char servoTime = 0x00;
static unsigned char servoWait = 0x00;
unsigned char left = 2;
unsigned char right = 1;
enum oscillatorStates{osc_start, osc_off, osc_wait, osc_left, osc_right} osc_state;
void osc_Tick() {
    switch(osc_state) { // transitions
        case osc_start:
            osc_state = osc_off;
            break;
        case osc_off:
            if(oscillateOn == 0x00) {
                osc_state = osc_off;
            }
            else if(oscillateOn == 0x01) {
                osc_state = osc_left;
            }
            break;
        case osc_left:
            if(oscillateOn == 0x00) {
                osc_state = osc_off;
            }
            else if(oscillateOn == 0x01 && servoWait <= 100) {
                osc_state = osc_left;
            }
            else if(oscillateOn == 0x01 && servoWait > 100) {
                osc_state = osc_wait;
            }
            servoWait++;
            break;
        case osc_wait:
            if(oscillateOn == 0x00) {
                osc_state = osc_off;
            }
            else if(oscillateOn == 0x01 && servoWait <= 1) {
                osc_state = osc_left;
            }
            else if(oscillateOn == 0x01 && servoWait <= 1000) {
                osc_state = osc_wait;
            }
            else if(oscillateOn == 0x01 && servoWait > 1000) {
                osc_state = osc_right;
            }
            servoWait++;
            break;
        case osc_right:
            if(oscillateOn == 0x00) {
                osc_state = osc_off;
            }
            else if(oscillateOn == 0x01 && servoWait <= 1100) {
                osc_state = osc_right;
            }
            else if(oscillateOn == 0x01 && servoWait > 1100) {
                osc_state = osc_wait;
                servoWait = 0;
            }
            servoWait++;
            break;
        default:
            break;
    }
    switch(osc_state) { // state actions
        case osc_start:
            break;
        case osc_off:
            servoMotor = 0x00;
            break;
        case osc_left:
            // Code for one oscillation
            // PWM period: 20ms
            // 1 ms => 0 degrees
            // 2 ms => 180 degrees
            // if(oscil_motor <= 0) 
            //     servoMotor = 0x01;
            // else if (oscil_motor <= 20) 
            //     servoMotor = 0x00;
            // else
            //     oscil_motor = 0;
            // oscil_motor++;
                if(servoTime <= left) {
                    servoMotor = 0x01;
                }
                else if (servoTime <= 20) { 
                    servoMotor = 0x00;
                }
                else { 
                    servoTime = 0x00;
                }
                servoTime++;
            break;
        case osc_wait:
            servoMotor = 0x00;
            break;
        case osc_right:
            if(servoTime <= right) {
                    servoMotor = 0x01;
                }
                else if (servoTime <= 20) { 
                    servoMotor = 0x00;
                }
                else { 
                    servoTime = 0x00;
                }
                servoTime++;
            break;
        default:
            break;
    }
}

unsigned char turn = 0x00;
enum display2_States{d2_start, d2_output, d2_pause} d2_state;
void d2_Tick() {
    switch(d2_state) { // transitions
        case d2_start:
            d2_state = d2_pause;
            break;
        case d2_output:
            if(fanOn == 0x01)
                d2_state = d2_output;
            else if(fanOn == 0x00)
                d2_state = d2_pause;
            break;
        case d2_pause:
            if(fanOn == 0x01)
                d2_state = d2_output;
            else if(fanOn == 0x00)
                d2_state = d2_pause;
            break;
        default:
            d2_state = d2_start;
            break;
    }
    switch(d2_state) { // state actions
        case d2_start:
            break;
        case d2_output:
            if(turn == 0x00) {
                nokia_lcd_clear();
                nokia_lcd_set_cursor(18,0);
                nokia_lcd_write_bitmap(fan02_45);
                nokia_lcd_render();
                turn = 0x01;
            }
            else if (turn == 0x01) {
                nokia_lcd_clear();
                nokia_lcd_set_cursor(18,0);
                nokia_lcd_write_bitmap(fan02);
                nokia_lcd_render();
                turn = 0x00;
            }
            break;
        case d2_pause:
            break;
        default:
            break;
    }
}

enum output_States{out_start, out_output} out_state;
void out_Tick() {
    switch(out_state) { // transitions
        case out_start:
            out_state = out_output;
            break;
        case out_output:
            out_state = out_output;
            break;
        default:
            out_state = out_start;
            break;
    }
    switch(out_state) { // state actions
        case out_start:
            break;
        case out_output:
            tempD = fanOn + (oscillateOn << 1) + (servoMotor << 2) + (motorEnable << 3) + (motorDir << 4);
            PORTD = tempD;
            break;
        default:
            break;
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // Input: Buttons, IR Receiver, Temperature Sensor
    DDRB = 0xFF; PORTB = 0x00; // Output: LCD2 (The fan animation)
    DDRC = 0xFF; PORTC = 0x00; // Output: LCD1 (Status Display)
    DDRD = 0xFF; PORTD = 0x00; // Output: Fan motor, oscillator, (and two status LEDs) + (LCD control)
    // DDRA = 0xFF; PORTA = 0x00; // LCD data lines
    // DDRD = 0xFF; PORTD = 0x00; // LCD control lines

    unsigned long F_elapsedTime = 0;
    unsigned long M_elapsedTime = 0;
    unsigned long osc_elapsedTime = 0;
    unsigned long d2_elapsedTime = 0;
    unsigned long out_elapsedTime = 0;
    const unsigned long timerPeriod = 1;

    tempA = ~PINA;
    // tempB = ~PINB;
    // tempC = ~PINC;
    // tempD = ~PIND;

    F_state = F_start;
    M_state = M_start;
    osc_state = osc_start;
    d2_state = d2_start;
    out_state = out_start;
    

    // ADC_init();

    LCD_init();
    LCD_ClearScreen();

    TimerSet(1);
    TimerOn();
    
    LCD_DisplayString(1, "Pwr:Off Osc:Off Spd:1          ");
    LCD_Cursor(0);

    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_set_cursor(18,0);
    nokia_lcd_write_bitmap(fan02);
    nokia_lcd_render();

    // unsigned char motor = 0;
    // unsigned char oscil_motor = 0;
    // LCD_DisplayString(17, "Oscillate: ");
    // "Pwr:    Osc:    Spd:           "

    while (1) {
        if(F_elapsedTime >= 10) {
            F_Tick();
            F_elapsedTime = 0;
        }
        if(M_elapsedTime >= 1) {
            M_Tick();
            M_elapsedTime = 0;
        }
        if(osc_elapsedTime >= 1) {
            osc_Tick();
            osc_elapsedTime = 0;
        }
        if(d2_elapsedTime >= 250) {
            d2_Tick();
            d2_elapsedTime = 0;
        }
        if(out_elapsedTime >= 1) {
            out_Tick();
            out_elapsedTime = 0;
        }

        // // Code testing
        // if(oscil_motor <= 0) 
        //     PORTD = 0x04;
        // else if (oscil_motor <= 20) 
        //     PORTD = 0x00;
        // else
        //     oscil_motor = 0;
        // oscil_motor++;
        // tempA = ADC;
        // PORTD = (char) (tempA);
        // PORTC = (char) (tempA >> 8);
        // if(servoTime <= right) {
        //             servoMotor = 0x01;
        //         }
        //         else if (servoTime <= 20) { 
        //             servoMotor = 0x00;
        //         }
        //         else { 
        //             servoTime = 0x00;
        //         }
        //         servoTime++;



        while(!TimerFlag) {}
        TimerFlag = 0;

        F_elapsedTime += timerPeriod;
        M_elapsedTime += timerPeriod;
        osc_elapsedTime += timerPeriod;
        d2_elapsedTime += timerPeriod;
        out_elapsedTime += timerPeriod;
    }
    return 1;
}
