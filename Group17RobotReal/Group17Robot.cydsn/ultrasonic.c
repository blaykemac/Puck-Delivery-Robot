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

/* Declare global variables */

int i = 0; //what is i and count used for? We might need to name them more descriptively
int count = 0;      
int distance_measured = 0; 
int sensor_distances[TOTAL_SONIC_SENSORS] = {0,0,0,0,0,0};
int ultrasonic_mux_control;       // THIS is a global variable that will be taken 
int block_start = 0;
int puck_start = 0;
int initialisation = 1;




void ultrasonicInterruptHandler(){
    //UART_1_PutString("Interrupt \n");
    Timer_1_ReadStatusRegister();   // CLEARS INTERRUPTS 
    count = Timer_1_ReadCounter();
    Timer_1_Stop();
    
    // Calculate distances based off timer counts
    sensor_distances[ultrasonic_mux_control] = 0.172413*(65536-count);          // This is in mm
    
}

void distanceSensor(int current_sensor_index) {
    
    //ultrasonic_flag = TRUE;
    Ultrasonic_Mux_Control_Write(current_sensor_index);
    
    // I think i need to change ultrasonic_mux_control to the correct value 
    ultrasonic_mux_control = current_sensor_index;    // This will be used when the interrupt occurs to put in the correct array
    
    
    Timer_1_Start();            // starts the timer
            
    switch(current_sensor_index){
        
        case 0:
        
            //while (Echo_1_Read() == 0)
            {
                
                Trigger_1_Write(1);     
                CyDelayUs(10);          // 10microsecond pulse
                Trigger_1_Write(0);     
            }
        
        break;
        
        case 1:
        
           //while (Echo_2_Read() == 0)
            {
                Trigger_2_Write(1);
                CyDelayUs(10);          // 10microsecond pulse
                Trigger_2_Write(0);
            }
        
        break;
        
        case 2:
        
            //while (Echo_3_Read() == 0)
            {
                Trigger_3_Write(1);
                CyDelayUs(10);          // 10microsecond pulse
                Trigger_3_Write(0);
            }
        
        break;
        
        case 3:
        
           //while (Echo_4_Read() == 0)
            {
                Trigger_4_Write(1);
                CyDelayUs(10);          // 10microsecond pulse
                Trigger_4_Write(0);
            }
        
        break;
        
        case 4:
        
            //while (Echo_5_Read() == 0)
            {
                Trigger_5_Write(1);
                CyDelayUs(10);          // 10microsecond pulse
                Trigger_5_Write(0);
            }
        
        break;
            
        case 5:
        
            //while (Echo_5_Read() == 0)
            {
                Trigger_6_Write(1);
                CyDelayUs(10);          // 10microsecond pulse
                Trigger_6_Write(0);
            }
        
        break;   

        default:
            UART_1_PutString("sensor is faulty");
        break;
            
    }
    //while(ultrasonic_flag == TRUE) {}
}

void distanceCheck()
{
   
  for (ultrasonic_mux_control = 0; ultrasonic_mux_control < TOTAL_SONIC_SENSORS; ultrasonic_mux_control++) {

        distanceSensor(ultrasonic_mux_control);
        CyDelay(SENSOR_PULSE_DELAY_MS);                // delay of 100 milliseconds or adequate between different sensor pulses
  
    }
}

void safetyDistanceCheck() {
    UART_1_PutString("Ultrasonic safety check: \n");
    for (int i = 0; i < 5; i++)
        {
            distanceSensor(i);
            CyDelay(60);
            sprintf(output, "%d \t", sensor_distances[i]);
            UART_1_PutString(output);
        }
        UART_1_PutString("\n\n");
}



/* [] END OF FILE */
