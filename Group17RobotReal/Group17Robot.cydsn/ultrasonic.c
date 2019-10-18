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
#include "mishamotor.h"
#include "customMath.h"
#include "navigation.h"


/* Declare global variables */

int i = 0; //what is i and count used for? We might need to name them more descriptively
int count = 0;      
int distance_measured = 0; 
float ultrasonic_distances[TOTAL_SONIC_SENSORS] = {0,0,0,0,0};      // array fills up depending on position of sensor
int ultrasonic_distances_mm[5] = {0,0,0,0,0};
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
    //ultrasonic_distances[ultrasonic_mux_control] = distanceFromCount(count);
    //ultrasonic_distances[ultrasonic_mux_control] = 0.172413*(65536-count) / 10;     // This is in cm
    ultrasonic_distances_mm[ultrasonic_mux_control] = 0.172413*(65536-count);          // This is in mm
    //ultrasonic_flag = FALSE;  
    
    //*distance = 0.172413*(65536-count); 
    
    
    // PRINTING FOR DEBUGGUNG:
    //printSensorToUART(ultrasonic_mux_control, ultrasonic_distances_mm[ultrasonic_mux_control]);    // Used to debug the ultrasonics 
    
    //if (ultrasonic_mux_control == 4) { UART_1_PutString("\n"); } 
    
    /*
    if (ultrasonic_mux_control == 4) {
        
        for (int i = 0; i < 5; i++)
        {
            sprintf(output, "%d \t", ultrasonic_distances_mm[i]);       
            UART_1_PutString(output);
        }
        UART_1_PutString("\n");       
    }
    */
    
    
    // STUFF using old DC motor code:
    /*
    //This bit has been added by Nidhin. It will stop the robot moving if it has gotten too close to 
    //any obstacle (we may have to modify this to not stop motion when we are trying to pick up a puck)
    if (((ultrasonic_distances[LEFT_FRONT] < COLLISION_THRESHOLD)||(ultrasonic_distances[RIGHT_FRONT] < COLLISION_THRESHOLD))&&(drivingForwardFlag == TRUE)){
        stopMotor1AndUpdate();
        stopMotor2AndUpdate();
    } //We need to add further clauses like this - the above only checks for the front wall being too close
    //when moving forward.
    
    // In this state we are scanning each puck on the rack
    if (state == STATE_SCAN_PLAN){
        
        float front_average = averageSensor(ultrasonic_distances[LEFT_FRONT], ultrasonic_distances[RIGHT_FRONT]);
        
        if (initialisation){
            
            if ( front_average > puckRackOffsetsFromWest[currentPuckRackScanningIndex] - OFFSET_COLOUR_SENSOR_FROM_FRONT + 15) { // Reverse until colour sensor is past left most puck on rack
                stopMotor1AndUpdate();
                stopMotor2AndUpdate();
                moving = 0;
            
                // Take colour measurement against black wall for reference initialisation
                
                // Also take colour measurement of the white ground for the front claw colour sensor reference
                
                initialisation = 0; // initialisation done on black wall so now we check the 5 puck rack slots
            }
        }
        
        else if ( front_average < puckRackOffsetsFromWest[currentPuckRackScanningIndex] - OFFSET_COLOUR_SENSOR_FROM_FRONT) {
            stopMotor1AndUpdate();
            stopMotor2AndUpdate(); 
            moving = 0;
            
            currentPuckRackScanningIndex++;
            if (currentPuckRackScanningIndex == 4) {state = STATE_LOCATE_BLOCK_AND_PUCKS;}

        }

    }

    if (state == STATE_LOCATE_BLOCK_AND_PUCKS){
        
        
        // Only care about detecting block when we have turned around and are ready to do a full sweep of the arena
    	if (sweeping){
    		if (ultrasonic_distances[LEFT_SIDE] < ARENA_LENGTH - ultrasonic_distances[RIGHT_SIDE] - SIDE_SENSORS_WIDTH - BLOCK_TOLERANCE){ // Then we have discrepancy
    			if (!block_start) {block_start = 1; block_edge_location[WEST] = ultrasonic_distances[BACK] + SIDE_SENSOR_OFFSET_FROM_BACK ;} // We know where west side of block is
    	    }
            
    		else if (block_start){ // We already know the west edge, drive until we find east edge
    			block_start = 0; // We have found the east edge of the block
    			block_edge_location[EAST] = ultrasonic_distances[BACK] + SIDE_SENSOR_OFFSET_FROM_BACK;
    			sweeping = 0;
            }
            
            if ( (ultrasonic_distances[LEFT_SIDE] < ARENA_LENGTH - ultrasonic_distances[RIGHT_SIDE] - SIDE_SENSORS_WIDTH - PUCK_TOLERANCE) && (ultrasonic_distances[LEFT_SIDE] > ARENA_LENGTH - ultrasonic_distances[RIGHT_SIDE] - SIDE_SENSORS_WIDTH - PUCK_TOLERANCE) ){ // Then we have discrepancy
    			puckPileLocation = ultrasonic_distances[BACK] + SIDE_SENSOR_OFFSET_FROM_BACK;
    	    } 
            
    	}
        
        
	}

    
     THIS IS A SAMPLE OF THE PRELIM CODE, KEEPING IT TO REFERENCE THE MOTOR SLEEP FUNCTIONS
    
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


void printSensorToUART(int sensorNumber, int distanceMeasured){
    
    // Print distance to UART
    //sprintf(output, "Ultrasonic Sensor %d - distance: %dmm\n", sensorNumber, distanceMeasured);
    //sprintf(output, "%d: %fcm\n", sensorNumber, distanceMeasured);
    sprintf(output, "%d: %dmm\n", sensorNumber, distanceMeasured);
    //sprintf(output, "%dmm \t", distanceMeasured);   
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
    
    //ultrasonic_flag = TRUE;
    Ultrasonic_Mux_Control_Write(currentSensorIndex);
    
    // I think i need to change ultrasonic_mux_control to the correct value 
    ultrasonic_mux_control = currentSensorIndex;    // This will be used when the interrupt occurs to put in the correct array
    
    
    Timer_1_Start();            // starts the timer
            
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
            sprintf(output, "%d \t", ultrasonic_distances_mm[i]);
            UART_1_PutString(output);
        }
        UART_1_PutString("\n\n");
}



/* [] END OF FILE */
