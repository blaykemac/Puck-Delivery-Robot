
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
#include "stdlib.h"

// * OUR LIBRARIES * //
#include "main.h"
#include "colour.h"
#include "ultrasonic.h" 
#include "dcmotor.h"
#include "servo.h"
#include "customMath.h"


/* Declare global variables */

int i = 0; //what is i and count used for? We might need to name them more descriptively
int count = 0;
int distance_measured = 0; 
float ultrasonic_distances[TOTAL_SONIC_SENSORS] = {0,0,0,0,0};
uint8 ultrasonic_mux_control = 0;
int block_start = 0;
int puck_start = 0;
int initialisation = 1;
float sensorDistance = 0;



void ultrasonicInterruptHandler(float * distance){
    //UART_1_PutString("Interrupt \n");
    Timer_1_ReadStatusRegister();   // CLEARS INTERRUPTS 
    count = Timer_1_ReadCounter();
    Timer_1_Stop();
    
    // Calculate distances based off timer counts
    //ultrasonic_distances[ultrasonic_mux_control] = distanceFromCount(count);
    //ultrasonic_distances[ultrasonic_mux_control] = 0.172413*(65536-count) / 10;
    *distance = 0.172413*(65536-count) / 10;
    //printSensorToUART(ultrasonic_mux_control, ultrasonic_distances[ultrasonic_mux_control]);    // Used to debug the ultrasonics 
    printSensorToUART(ultrasonic_mux_control, *distance);
    
    /* THIS IS A SAMPLE OF THE PRELIM CODE, KEEPING IT TO REFERENCE THE MOTOR SLEEP FUNCTIONS
    
    if (driveStraightEnable && ( ultrasonic_distances[LEFT_FRONT] < 24 || ultrasonic_distances[RIGHT_FRONT] < 24 ) && ( ultrasonic_distances[LEFT_FRONT] > 1 && ultrasonic_distances[RIGHT_FRONT] > 1 )) {
    //if (ultrasonic_distances[RIGHT_FRONT] < 30 ) { // Used for when left front sensor playing up
        if(state == STATE_GO_WEST){
            driveStraightEnable = 0;
            westWall = 1;
           // state = STATE_GO_NORTH_CAREFUL;
            Motor_1_driver_Sleep();
            Motor_2_driver_Sleep();
            
        }
    }
    
    */
    
}


void printSensorToUART(int sensorNumber, float distanceMeasured){
    // Print distance to UART
    //sprintf(output, "Ultrasonic Sensor %d - distance: %dmm\n", sensorNumber, distanceMeasured);
    sprintf(output, "%d: %fmm\n", sensorNumber, distanceMeasured);
    UART_1_PutString(output);
}

/*
float distanceFromCount(int timerCount){

    //int result = (int) (0.1777*(65536-timerCount)+13.013); // distance measured to nearest mm
    float result = (float) (0.1777*(65536-timerCount)+13.013) / 10 -10;                   // distance in mm
    //float result = (
    //float result = (65536-timerCount)/58;                    // distance in cm
    return result;

}
*/

void distanceSensor(int currentSensorIndex) {
    Ultrasonic_Mux_Control_Write(currentSensorIndex);
    Timer_1_Start();   
    switch(currentSensorIndex){
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
    }
}

void distanceCheck(int * sensorNumberPointer)
{
    UART_1_PutString("CHECK BEING DONE");
    ultrasonic_mux_control = *sensorNumberPointer;
    distanceSensor(ultrasonic_mux_control);
    CyDelay(SENSOR_PULSE_DELAY_MS);
}

/* [] END OF FILE */
