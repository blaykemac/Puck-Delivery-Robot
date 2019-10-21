
/* ========================================
 *
 * Copyright Group 17, 2019
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Blayke, Misha, Nidhin.
 *
 * ========================================
*/

// * PSOC LIBRARY * //
#include "project.h"

// * C LIBRARIES * // 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// * OUR LIBRARIES * //
#include "main.h"
#include "colour.h"
#include "ultrasonic.h" 
#include "servo.h"
#include "motor.h"
#include "customMath.h"
#include "navigation.h"
#include "debugging.h"


// * CONSTANT VALUES * //
                                            // Note: make sure to update these values, if you change them in the top design
const float trans = 120000;    // 20k ogms

                    
// * VARIABLES * //

// Colour Sensing: 
float pd_voltage;
float input_current;
float stored[9];            
float sum = 0;

float ambient_current;      // used to remove the ambient light from the photo diode measurements 

float ambient_white;        // ambient values when white LED on (no puck)
float ambient_red;          // ambient values when red LED on   (no puck)
float ambient_green;        // ambient values when green LED on (no puck)
float ambient_blue;         // ambient values when blue LED on  (no puck)

float test_white;           
float test_red;
float test_green;
float test_blue;

extern int colour_sensing_algorithm;      // determines which coloursensing algorithm to use 
extern int colour_flag;

int puck_all[4] = {0,0,0,0};          
                            // no puck detection = puck_all[0]
                            // red puck detection = puck_all[1]
                            // green puck detection = puck_all[2]
                            // blue puck detection = puck_all[3]
int puck_output = 0;        // detects the overall amount of times a puck has been detected

float colour_output;

// LOGICAL STEPS OF COLOUR SENSING: 
/*
1. Initialise against the black wall, or white ground.
2. perhaps initialise both sensors initially. 


3. Need some failsafe to determine if the colours have been initialised correctly? 
4. Can we determine somehow that the colour sensing isn't working as intended? 

Writing these values to control_led_Write(x):
    x = 0: BLANK
    x = 1: RED
    x = 2: GREEN
    x = 3: BLUE

*/

void colourSensingInitialise (void)
{
    float test[4] = {0, 0, 0, 0};           // resets the test array to 
    
    for (int i = 0; i < 4; i++) 
    {
        control_led_Write(i);
        CyDelay(10);
        test[i] = ADC_Read32();             // Grabs the value and places it in the test array
    }
    
    control_led_Write(0);                   // Turns LED off
    
    ambient_white = test[0];   
    ambient_red = test[1];     
    ambient_green = test[2];   
    ambient_blue = test[3];    
}


void colourSensingDebug(void)
{
    float test[4] = {0, 0, 0, 0};           // resets the test array to 0
    
    for (int i = 0; i < 4; i++) 
    {
        CyDelay(10);
        control_led_Write(i);
        test[i] = ADC_Read32();             // Grabs the value and places it in the test array
    }
    
    control_led_Write(0);            // Turns the LED off
    
    test_white = test[0];   
    test_red = test[1];     
    test_green = test[2];   
    test_blue = test[3];    
    
    switch(ColourSensingAlgorithm) 
    {
        case 0:                         // WALL ALGORITHM
            if (test_red < (ambient_red + 2000) && test_green < (ambient_green + 2000) && test_blue < (ambient_blue + 2000))
            {
                UART_1_PutString("N");      // No puck detected
            }
            else if (test_red > test_green && test_red > test_blue)
            {
                UART_1_PutString("R");      // Red puck detected 
            }
             else if (test_green > test_blue && test_green > test_red)
            {
                UART_1_PutString("G");      // Green puck detected 
            }
             else if (test_blue > test_green && test_blue > test_red && test_green > test_red)
            {
                UART_1_PutString("B");      // Blue puck detected
            }   
            break;
        case 1:
            break;
        case 2:
            break;
    }    

}


int colourSensingOutput(void)
{
    int value = 0;
    
        float test[4] = {0, 0, 0, 0};           // resets the test array to 0
    
    for (int i = 0; i < 4; i++) 
    {
        control_led_Write(i);
        CyDelay(10);
        test[i] = ADC_Read32();             // Grabs the value and places it in the test array
    }
    
    control_led_Write(0);            // Turns the LED off
    
    test_white = test[0];   
    test_red = test[1];     
    test_green = test[2];   
    test_blue = test[3];    
    
    
    switch(colour_sensing_algorithm) 
    {
        case 0:                         // WALL ALGORITHM
            if (test_red < (ambient_red + 2000) && test_green < (ambient_green + 2000) && test_blue < (ambient_blue + 2000))
            {
                value = BLANK;      // No puck detected
            }
            else if (test_red > test_green && test_red > test_blue)
            {
                value = RED;      // Red puck detected 
            }
             else if (test_green > test_blue && test_green > test_red)
            {
                value = GREEN;      // Green puck detected 
            }
             else if (test_blue > test_green && test_blue > test_red && test_green > test_red)
            {
                value = BLUE;      // Blue puck detected
            }   
            break;
        case 1:                     // 1 PUCK ABOVE GROUND ALGORITHM (CLAW)
            if (test_red < 8000 && test_green < 8000 && test_blue < 8000)
            {
                value = BLANK;      // No puck detected, not detecting no pucks, so will never be valid
            }
            else if (test_red > test_green && test_red > test_blue)
            {
                value = RED;      // Red puck detected 
            }
             else if (test_green > test_blue && test_green > test_red)
            {
                value = GREEN;      // Green puck detected 
            }
             else                   // have no detection paramters 
            {
                value = BLUE;      // Blue puck detected
            }      
            break;
        case 2:                     // AT GROUND LEVEL ALGORITHM (CLAW)
            if (test_red > 16000 && test_green > 16000 && test_blue < 12000)
            {
                value = BLANK;      // No puck detected, not detecting no pucks, so will never be valid
            }
            else if (test_red > 25000)
            {
                value = RED;      // Red puck detected 
            }
            else if (test_green > 27000)
            {
                value = GREEN;      // Green puck detected 
            }
            else                   //  have no detection paramters 
            {
                value = BLUE;      // Blue puck detected
            }      
            break;
        case 3:
            if (test_red > 18000 && test_green > 18000 && test_blue < 18000 && test_white > 4000)
            {
                value = BLANK;      // No puck detected, not detecting no pucks, so will never be valid
            }
            else if (test_red > 25000)
            {
                value = RED;      // Red puck detected 
            }
            else if (test_green > 25000)
            {
                value = GREEN;      // Green puck detected 
            }
            else                   //  have no detection paramters 
            {
                value = BLUE;      // Blue puck detected
            }
            break;
        case 4:                     // BLACK PUCK ALGORITHM
            if (test_red < 5000 && test_blue < 5000 && test_green < 5000 && test_white < 5000) {
                value = RED;
            }
            else {
                value = BLANK;
            }
            break;
        case 5:                     // new ground sensing algorithm
            if (test_white > 2000 && test_red < 20000 && test_green < 20000 && test_blue < 14000) {
                value = BLANK;
            }
            else {
                value = RED;
            }
            break;
        default:
            UART_1_PutString("INVALID COLOUR SENSING ALGORITHM \n");
            CyDelay(3000);
            break;    
    }
    
    
    return value;
}


void colourSensingCalibration(void)
{
    float calibrate[4] = {0, 0, 0, 0};           // resets the test array to 0
                                            // this may not be necessary 
   
    for (int i = 0; i < 4; i++) 
    {
        control_led_Write(i);
        CyDelay(10);
        calibrate[i] = ADC_Read32();             // Grabs the value and places it in the test array
    }
    
    control_led_Write(0);            // Turns the LED off
        
    for (int i = 0; i < 4; i++) 
    {
        sprintf(output, "%f \t", round(calibrate[i]));       
        UART_1_PutString(output);
    }
    
}


void blinkLED(int colour, int time_ms) {
    int old_photodiode = control_photodiode_Read(); // reads which colour sensor is current active
    control_photodiode_Write(WALL_SENSING);         // changes it to the wall sensor
    
    control_led_Write(colour);
    CyDelay(time_ms);              
    control_led_Write(0);
    CyDelay(300);
    
    control_photodiode_Write(old_photodiode);   // returns to the previous colour sensor
}


/* [] END OF FILE */
