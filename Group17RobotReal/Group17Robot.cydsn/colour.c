
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
#include <stdio.h>
#include <math.h>

// * OUR LIBRARIES * //
#include "main.h"
#include "colour.h"
#include "ultrasonic.h" 
#include "dcmotor.h"
#include "servo.h"
#include "customMath.h"



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

<<<<<<< Updated upstream

=======
extern int ColourSensingAlgorithm;      // determines which coloursensing algorithm to use 
extern int colour_flag;
>>>>>>> Stashed changes

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


void ColourSensingInitialise (void)
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


void ColourSensingDebug(void)
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
    
    
    switch(ColourSensingAlgorithm) 
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
        case 1:
            if (test_red > 40000)
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
        case 2:
            break;    
    }
    
    
    return value;
}


void ColourSensingCalibration(void)
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


/* [] END OF FILE */
