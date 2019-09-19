/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "colour.h"
#include "project.h"

// * C LIBRARIES * // 
#include <stdio.h>
#include <math.h>



// * CONSTANT VALUES * //
                                            // Note: make sure to update these values, if you change them in the top design
const float trans = 120000;    // 20k ogms

                    
// * VARIABLES * //

// Colour Sensing: 
float pd_voltage;
float input_current;
float stored[9];
float sum = 0;
char output[32];
float ambient_current;      // used to remove the ambient light from the photo diode measurements 


float ambient_white;        // ambient values when white LED on (no puck)
float ambient_red;          // ambient values when red LED on   (no puck)
float ambient_green;        // ambient values when green LED on (no puck)
float ambient_blue;         // ambient values when blue LED on  (no puck)

float test_white;           
float test_red;
float test_green;
float test_blue;

extern int colour_flag;

int puck_all[4] = {0,0,0,0};          
                            // no puck detection = puck_all[0]
                            // red puck detection = puck_all[1]
                            // green puck detection = puck_all[2]
                            // blue puck detection = puck_all[3]
int puck_output = 0;        // detects the overall amount of times a puck has been detected

float colour_output;

// RED DECTECTION:
//int red_flag = 0;       // starts off as no red detected

void RGB1Initialise(void)
{
    
    control_photodiode_Write(0);       // Drives photodiode 1
    
    // Red Light: 
    LED_Red_Write(1);
    CyDelay(10);
    ambient_red = ADC_Read32();
    CyDelay(10);
    LED_Red_Write(0);
    
    CyDelay(20);
    
    // Blue Light: 
    LED_Blue_Write(1);
    CyDelay(10);
    ambient_blue = ADC_Read32();
    CyDelay(10);
    LED_Blue_Write(0);
    
    CyDelay(20);
    
    // Green Light: 
    LED_Green_Write(1);
    CyDelay(10);
    ambient_green = ADC_Read32();
    CyDelay(10);
    LED_Green_Write(0);
    
    CyDelay(20);
    
    sprintf(output, "ambient red = %f \n", round(ambient_red));       
    UART_1_PutString(output);
    
    sprintf(output, "ambient green = %f \n", round(ambient_green));       
    UART_1_PutString(output);
    
    sprintf(output, "ambient blue = %f\n", round(ambient_blue));       
    UART_1_PutString(output);
    
    
}

void RGB2Initialise(void)
{
    
    control_photodiode_Write(1);       // Drives photodiode 2
       
        // Red Light: 
    RGB_Red_Write(0);
    CyDelay(10);
    ambient_red = ADC_Read32();
    CyDelay(10);
    RGB_Red_Write(1);
    
    CyDelay(20);
    
    // Blue Light: 
    RGB_Blue_Write(0);
    CyDelay(10);
    ambient_blue = ADC_Read32();
    CyDelay(10);
    RGB_Blue_Write(1);
    
    CyDelay(20);
    
    // Green Light: 
    RGB_Green_Write(0);
    CyDelay(10);
    ambient_green = ADC_Read32();
    CyDelay(10);
    RGB_Green_Write(1);
    
    CyDelay(20);
    
    sprintf(output, "ambient red = %f \n", round(ambient_red));       
    UART_1_PutString(output);
    
    sprintf(output, "ambient green = %f \n", round(ambient_green));       
    UART_1_PutString(output);
    
    sprintf(output, "ambient blue = %f\n", round(ambient_blue));       
    UART_1_PutString(output);
    
    UART_1_PutString("test no led: \t");
    UART_1_PutString("test red: \t");
    UART_1_PutString("test green: \t");
    UART_1_PutString("test blue: \t");
    UART_1_PutString("\n");
    
}


void ColourSensingRGB1(void)
{
        // No Light: 
    CyDelay(10);
    test_white = ADC_Read32();
    CyDelay(10);
        
        // Red Light: 
    LED_Red_Write(1);
    CyDelay(10);
    test_red = ADC_Read32();
    CyDelay(10);
    LED_Red_Write(0);
    
    CyDelay(20);
    
    // Blue Light: 
    LED_Blue_Write(1);
    CyDelay(10);
    test_blue = ADC_Read32();
    CyDelay(10);
    LED_Blue_Write(0);
    
    CyDelay(20);
    
    // Green Light: 
    LED_Green_Write(1);
    CyDelay(10);
    test_green = ADC_Read32();  
    CyDelay(10);
    LED_Green_Write(0);
    
    CyDelay(20);
    
    /*
    sprintf(output, "test red = %f \n", round(test_red));       
    UART_1_PutString(output);
    
    sprintf(output, "test green = %f \n", round(test_green));       
    UART_1_PutString(output);
    
    sprintf(output, "test blue = %f\n", round(test_blue));      
    UART_1_PutString(output);
    */
    
    
    
    
    
    // TEST PRINTING //
    
    /*
    
    UART_1_PutString("\n");
    
    sprintf(output, "test red = %f \n", round(test_red));       
    UART_1_PutString(output);
    
    sprintf(output, "test green = %f \n", round(test_green));       
    UART_1_PutString(output);
    
    sprintf(output, "test blue = %f\n", round(test_blue));       
    UART_1_PutString(output);
    
    */
    
}


void ColourSensingRGB2(void)
{
    // No Light: 
    CyDelay(10);
    test_white = ADC_Read32();
    CyDelay(10);
        
    // Red Light: 
    RGB_Red_Write(0);
    CyDelay(10);
    test_red = ADC_Read32();
    CyDelay(10);
    RGB_Red_Write(1);
    
    CyDelay(20);
    
    // Blue Light: 
    RGB_Blue_Write(0);
    CyDelay(10);
    test_blue = ADC_Read32();
    CyDelay(10);
    RGB_Blue_Write(1);
    
    CyDelay(20);
    
    // Green Light: 
    RGB_Green_Write(0);
    CyDelay(10);
    test_green = ADC_Read32();
    CyDelay(10);
    RGB_Green_Write(1);
    
    CyDelay(20);

    
}


void ColourIntialiseViaHardware (void)
{
    control_photodiode_Write(colour_flag);       // Drives photodiode 2
    PWM_colour_Wakeup();                    // Wakes PWM up from sleep
    PWM_colour_WriteCompare(255);           // ensures the PWM is driving a continuous pulse
    
    float test[4] = {0, 0, 0, 0};           // resets the test array to 0
   
    for (int i = 0; i < 4; i++) 
    {
        CyDelay(10);
        control_led_Write(i);
        test[i] = ADC_Read32();             // Grabs the value and places it in the test array
        CyDelay(10);
        control_led_Write(0);            // Turns the LED off
        CyDelay(20);
    }
    ambient_white = test[0];
    ambient_red = test[1];
    ambient_green = test[2];
    ambient_blue = test[3];   
    
    PWM_colour_WriteCompare(10);            // sets back to initial pulse
    PWM_colour_Sleep();                     // Sets PWM back to sleep

}


void ColourSensingViaHardware(void)
{
    float test[4] = {0, 0, 0, 0};           // resets the test array to 0
    
    PWM_colour_Wakeup();
    PWM_colour_WriteCompare(255);           // ensures the PWM is driving a continuous pulse
    
    for (int i = 0; i < 4; i++) 
    {
        CyDelay(10);
        control_led_Write(i);
        test[i] = ADC_Read32();             // Grabs the value and places it in the test array
        CyDelay(10);
        control_led_Write(0);            // Turns the LED off
        CyDelay(20);
    }
    test_white = test[0];   
    test_red = test[1];     
    test_green = test[2];   
    test_blue = test[3];    
    
    PWM_colour_WriteCompare(10);            // sets back to initial pulse
    PWM_colour_Sleep();                     // Sets PWM back to sleep
    
}

void ColourOutput(void)
{
    if (test_red > test_blue && test_red > test_green)          // DETERMINING IF IT IS A RED PUCK
    {
        UART_1_PutString("R");      // red puck
        puck_all[1]++;
    }
    else if (test_green > test_blue && ambient_red > ambient_white && test_green > ambient_green)                            // DETERMINING IF GREEN PUCK                                                           
    {                                                           // This isn't full proof
        UART_1_PutString("G");      // green puck
        puck_all[2]++;
    }
    else if (test_blue > test_green && test_red < ambient_red)
    {
        UART_1_PutString("B");      // BLUE puck
        puck_all[3]++;
    }
    else 
    {
        UART_1_PutString("N");
        puck_all[0]++;
    }
    
    puck_output++;
    sprintf(output, "%i\t", puck_output);      
    UART_1_PutString(output);
    
    if (puck_output == 11)
    {
        UART_1_PutString("\nPuck detected: ");
        int max = 0;
        int colour = 0;
        for (int i = 0; i < 4; i++)             // Finds the puck that was detected the most (so we are accounting for outliers)
        {
            if (puck_all[i] > max)
            {
                max = puck_all[i];
                colour = i;
                                
            }           
        }
        
        switch(colour)
        {
            case 0: UART_1_PutString("\n"); break;
            case 1: 
                UART_1_PutString("Red \n");
                control_led_Write(colour);
                PWM_colour_Wakeup();
                CyDelay(2000);
                PWM_colour_Sleep();
                CyDelay(1000);
                break;
            case 2:
                UART_1_PutString("Green \n");
                control_led_Write(colour);
                PWM_colour_Wakeup();
                CyDelay(2000);
                PWM_colour_Sleep();
                CyDelay(1000);
                break;
            case 3:
                UART_1_PutString("Blue \n");
                control_led_Write(colour);
                PWM_colour_Wakeup();
                CyDelay(2000);
                PWM_colour_Sleep();
                CyDelay(1000);
                break;
            default: break;
        }
        
        for (int i = 0; i < 4; i++) 
        {
            puck_all[i] = 0;                    // resets all the puck counts
        }
        puck_output = 0;
        
    }
    
}

void ColourDebug(void){
    
    // FINAL PRINTING //
    
    if (test_red > test_blue && test_red > test_green)          // DETERMINING IF IT IS A RED PUCK
    {
        UART_1_PutString("R");       // RED puck                   
        puck_all[1]++;
    }
    else if (test_green > test_blue && ambient_red > ambient_white && test_green > ambient_green)                            // DETERMINING IF GREEN PUCK                                                           
    {                                                           // This isn't full proof
        UART_1_PutString("G");      // GREEN puck
        puck_all[2]++;
    }
    else if (test_blue > test_green && test_red < ambient_red)
    {
        UART_1_PutString("B");      // BLUE puck
        puck_all[3]++;
    }
    else 
    {
        UART_1_PutString("N");
        puck_all[0]++;
    }
    
    puck_output++;
    
    // This next part outputs the number of times a certain puck was detected over the previous 10 detections
    if (puck_output == 10)
    {
        UART_1_PutString("\n");
        for(int i = 0; i < 4; i++)
        {
            sprintf(output, "%i\t", puck_all[i]);      
            UART_1_PutString(output);
            puck_all[i] = 0;
        }
        UART_1_PutString("\n");
        puck_output = 0;
    }
    
    /*
    
    if (puck_output == 20)
    {
        int max = 0;
        int colour = 0;
        for (int i = 0; i < 4; i++)
        {
            if (puck_all[i] > max)
            {
                max = puck_all[i];
                colour = i;
                sprintf(output, "%i\t", puck_all[i]);      
                UART_1_PutString(output);
                UART_1_PutString("\n");
                
            }           
        }
        if (colour == 0)
        {
            UART_1_PutString("No puck detected\n");
        }
        else if (colour == 1)
        {
            UART_1_PutString("Red puck\n");    
        }
        else if (colour == 2)
        {
            UART_1_PutString("Green puck\n");    
        }
        else if (colour == 3)
        {
            UART_1_PutString("Blue puck\n");    
        }
        
        for (int i = 0; i < 4; i++) 
        {
            puck_all[i] = 0;                    // resets all the puck counts
        }
    }
    
    */
    
}

void ColourSensingCalibrationRGB1(void)
{
    // * Initialisation * //
    
    // Ambient Light:
    sum = 0;
        for(int i = 0; i < 10; i++)
            {
            stored[i] = ADC_Read32();
            sum = sum + stored[i];
            CyDelay(10);
            }
        ambient_white = sum/10;                    // will be the average of the ambient light voltages taken
    
    CyDelay(20);
    
    // Blue LED: 
    LED_Blue_Write(1);
    sum = 0; 
    for(int i = 0; i < 10; i++)
        {
        stored[i] = ADC_Read32();
        sum = sum + stored[i];
        CyDelay(10);
        }
    ambient_blue = sum/10;                    // will be the average of the ambient light voltages taken    
    LED_Blue_Write(0);
    
    CyDelay(20);
            
    // Red LED: 
    LED_Red_Write(1);
    sum = 0; 
    for(int i = 0; i < 10; i++)
        {
        stored[i] = ADC_Read32();
        sum = sum + stored[i];
        CyDelay(10);
        }
    ambient_red = sum/10;                    // will be the average of the ambient light voltages taken    
    LED_Red_Write(0);
    
    CyDelay(20);        
    
    // Green LED: 
    LED_Green_Write(1);
    sum = 0; 
    for(int i = 0; i < 10; i++)
        {
        stored[i] = ADC_Read32();
        sum = sum + stored[i];
        CyDelay(10);
        }
    ambient_green = sum/10;                    // will be the average of the ambient light voltages taken    
    LED_Green_Write(0);
    
    CyDelay(20);
        
    
    // printing ambient colours:
    
    sprintf(output, "%f \t", ambient_white);       
    UART_1_PutString(output);
    
    
    sprintf(output, "%f \t", ambient_red);       
    UART_1_PutString(output);
    
    
    sprintf(output, "%f \t", ambient_green);       
    UART_1_PutString(output);
    
    sprintf(output, "%f\t", ambient_blue);       
    UART_1_PutString(output);
    UART_1_PutString("\n");
    
    //UART_1_PutString("\n\n");
    
    
    
}

void ColourSensingCalibrationRGB2(void)
{
    // * Initialisation * //
    
    // Ambient Light:
    sum = 0;
        for(int i = 0; i < 10; i++)
            {
            stored[i] = ADC_Read32();
            sum = sum + stored[i];
            CyDelay(10);
            }
        ambient_white = sum/10;                    // will be the average of the ambient light voltages taken
    
    CyDelay(20);
    
    // Blue LED: 
    RGB_Blue_Write(0);
    sum = 0; 
    for(int i = 0; i < 10; i++)
        {
        stored[i] = ADC_Read32();
        sum = sum + stored[i];
        CyDelay(10);
        }
    ambient_blue = sum/10;                    // will be the average of the ambient light voltages taken    
    RGB_Blue_Write(1);
    
    CyDelay(20);
            
    // Red LED: 
    RGB_Red_Write(0);
    sum = 0; 
    for(int i = 0; i < 10; i++)
        {
        stored[i] = ADC_Read32();
        sum = sum + stored[i];
        CyDelay(10);
        }
    ambient_red = sum/10;                    // will be the average of the ambient light voltages taken    
    RGB_Red_Write(1);
    
    CyDelay(20);        
    
    // Green LED: 
    RGB_Green_Write(0);
    sum = 0; 
    for(int i = 0; i < 10; i++)
        {
        stored[i] = ADC_Read32();
        sum = sum + stored[i];
        CyDelay(10);
        }
    ambient_green = sum/10;                    // will be the average of the ambient light voltages taken    
    RGB_Green_Write(1);
    
    CyDelay(20);
        
    
    // printing ambient colours:
    
    sprintf(output, "%f \t", ambient_white);       
    UART_1_PutString(output);
    
    
    sprintf(output, "%f \t", ambient_red);       
    UART_1_PutString(output);
    
    
    sprintf(output, "%f \t", ambient_green);       
    UART_1_PutString(output);
    
    sprintf(output, "%f\t", ambient_blue);       
    UART_1_PutString(output);
    UART_1_PutString("\n");
    
    //UART_1_PutString("\n\n");
    
    
    
}


int redDetection(void)
{
    
    PWM_colour_Wakeup();                    // Wakes PWM up from sleep
    PWM_colour_WriteCompare(255);           // ensures the PWM is driving a continuous pulse
    
    float test[4] = {0, 0, 0, 0};           // resets the test array to 0
                                            // this may not be necessary 
   
    for (int i = 0; i < 4; i++) 
    {
        CyDelay(10);
        control_led_Write(i);
        test[i] = ADC_Read32();             // Grabs the value and places it in the test array
        CyDelay(10);
        control_led_Write(0);            // Turns the LED off
        CyDelay(20);
    }
    
    test_white = test[0];
    test_red = test[1];
    test_green = test[2];
    test_blue = test[3];   
    
    PWM_colour_WriteCompare(10);            // sets back to initial pulse
    PWM_colour_Sleep();                     // Sets PWM back to sleep
    
    if (test_red > test_blue && test_red > test_green)          // DETERMINING IF IT IS A RED PUCK
    {
        return 1;       
    }    
    return 0; 
}



/* [] END OF FILE */
