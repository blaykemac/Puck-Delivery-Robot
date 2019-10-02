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

// * Custom Libraries * //
#include "main.h"
#include "ultrasonic.h"
#include "project.h"
#include "dcmotor.h"
#include "customMath.h"

// * C LIBRARIES * // 
#include "stdlib.h"
#include <stdio.h>
#include <math.h>


/* Declare global variables */

int i = 0;
int count = 0;
int distance_measured = 0; 
float ultrasonic_distances[TOTAL_SONIC_SENSORS] = {0,0,0,0,0};
uint8 ultrasonic_mux_control = 0;
int block_start = 0;



//int distance;
char output[40];

void ultrasonicInterruptHandler(){
    //UART_1_PutString("Interrupt \n");
    Timer_1_ReadStatusRegister();   // CLEARS INTERRUPTS 
    count = Timer_1_ReadCounter();
    Timer_1_Stop();
    
    // Calculate distances based off timer counts
    //ultrasonic_distances[ultrasonic_mux_control] = distanceFromCount(count);
    ultrasonic_distances[ultrasonic_mux_control] = 0.172413*(65536-count) / 10;
    
    printSensorToUART(ultrasonic_mux_control, ultrasonic_distances[ultrasonic_mux_control]);    // Used to debug the ultrasonics 
    
    // In this state 
    if (state == STATE_SCAN_PLAN){
        
        float front_average = averageSensor(ultrasonic_distances[LEFT_FRONT], ultrasonic_distances[RIGHT_FRONT]);
        
        if (currentPuckRackScanningIndex == 0){
            if ( front_average > puckRackOffsetsFromWest[currentPuckRackScanningIndex] - OFFSET_COLOUR_SENSOR_FROM_FRONT){
                puckRackColours[currentPuckRackScanningIndex] = takeColourMeasurement();
                currentPuckRackScanningIndex++;
            }
        }
        else {
            if (front_average < puckRackOffsetsFromWest[currentPuckRackScanningIndex] - OFFSET_COLOUR_SENSOR_FROM_FRONT){
                puckRackColours[currentPuckRackScanningIndex] = takeColourMeasurement();
                currentPuckRackScanningIndex++;
            }
        }
        
        
        
    }
    
    /*
    
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
    
    if (driveStraightEnable && ( ultrasonic_distances[LEFT_FRONT] < 30 || ultrasonic_distances[RIGHT_FRONT] < 30 ) && ( ultrasonic_distances[LEFT_FRONT] > 1 && ultrasonic_distances[RIGHT_FRONT] > 1 )) {
    //if (ultrasonic_distances[RIGHT_FRONT] < 30 ) { // Used for when left front sensor playing up
        if(state == STATE_GO_NORTH_CAREFUL && ultrasonic_mux_control == 4){
            driveStraightEnable = 0;
            northWall = 1;
            //state = STATE_GO_NORTH_CARELESS;
            Motor_1_driver_Sleep();
            Motor_2_driver_Sleep();
            
        }
    }
    
    if (driveStraightEnable && ( ultrasonic_distances[LEFT_FRONT] < NW_PUCK_POSITION_FROM_NORTH - 1 || ultrasonic_distances[RIGHT_FRONT] < NW_PUCK_POSITION_FROM_NORTH - 1) && ( ultrasonic_distances[LEFT_FRONT] > 1 && ultrasonic_distances[RIGHT_FRONT] > 1 )) {
    //if (ultrasonic_distances[RIGHT_FRONT] < 30 ) { // Used for when left front sensor playing up
        if(state == STATE_GO_NORTH_CARELESS && ultrasonic_mux_control == 4){
            driveStraightEnable = 0;
            Motor_1_driver_Sleep();
            Motor_2_driver_Sleep();
            
        }
    }
    
    if (driveStraightEnable && ( ultrasonic_distances[BACK] > NW_PUCK_POSITION_FROM_WEST - 31 - 10   ) && ( ultrasonic_distances[BACK] > 1 )) {
    //if (ultrasonic_distances[RIGHT_FRONT] < 30 ) { // Used for when left front sensor playing up
        if(state == STATE_FORWARD_TO_PUCK && ultrasonic_mux_control == 4){
            driveStraightEnable = 0;
           // northWall = 1;
            //state = STATE_GO_NORTH_CARELESS;
            Motor_1_driver_Sleep();
            Motor_2_driver_Sleep();
            
        }
    }
    
    if (driveStraightEnable && ( ultrasonic_distances[BACK] < 13   ) && ( ultrasonic_distances[BACK] > 1 )) {
    //if (ultrasonic_distances[RIGHT_FRONT] < 30 ) { // Used for when left front sensor playing up
        if(state == STATE_RETURN_TO_NW_FROM_PUCK && ultrasonic_mux_control == 4){
            driveStraightEnable = 0;
           // northWall = 1;
            //state = STATE_GO_NORTH_CARELESS;
            Motor_1_driver_Sleep();
            Motor_2_driver_Sleep();
            
        }
    }
    
    if (driveStraightEnable && ( ultrasonic_distances[BACK] < 10 ) && ( ultrasonic_distances[BACK] > 1 )) {
    //if (ultrasonic_distances[RIGHT_FRONT] < 30 ) { // Used for when left front sensor playing up
        if(state == STATE_RETURN_HOME_SIDE && ultrasonic_mux_control == 4){
            driveStraightEnable = 0;
            Motor_1_driver_Sleep();
            Motor_2_driver_Sleep();
            
        }
    }
    
    if (driveStraightEnable && ( ultrasonic_distances[RIGHT_FRONT] < FINISH_CENTER_FROM_E - 5 ) && ( ultrasonic_distances[RIGHT_FRONT] > 1 )) {
    //if (ultrasonic_distances[RIGHT_FRONT] < 30 ) { // Used for when left front sensor playing up
        if(state == STATE_LAND_ON_HOME_BASE && ultrasonic_mux_control == 4){
            driveStraightEnable = 0;
            Motor_1_driver_Sleep();
            Motor_2_driver_Sleep();
            
        }
    }
    
    if (driveStraightEnable && ( ultrasonic_distances[RIGHT_FRONT] > FINISH_CENTER_FROM_E - 5 ) && ( ultrasonic_distances[RIGHT_FRONT] > 1 )) {
    //if (ultrasonic_distances[RIGHT_FRONT] < 30 ) { // Used for when left front sensor playing up
        if(state == STATE_FINISH_LANDING && ultrasonic_mux_control == 4){
            driveStraightEnable = 0;
            Motor_1_driver_Sleep();
            Motor_2_driver_Sleep();
            
        }
    }
    
    */
	
    if (state == STATE_LOCATE_BLOCK){
    	if (sweeping){
    		if (ultrasonic_distances[LEFT_SIDE] < ARENA_LENGTH - ultrasonic_distances[RIGHT_SIDE] - SIDE_SENSORS_WIDTH - BLOCK_TOLERANCE){ // Then we have discrepancy
    			if (!block_start) {block_start = 1; block_location[BLOCK_WEST_EDGE] = ultrasonic_distances[BACK] + SIDE_SENSOR_OFFSET_FROM_BACK ;}
    	    }
            
    		else if (block_start){
    			block_start = 0; // We have found the east edge of the block
    			block_location[BLOCK_EAST_EDGE] = ultrasonic_distances[BACK] + SIDE_SENSOR_OFFSET_FROM_BACK;
    			sweeping = 0;
    			// state = 

    			
    			
    	    }
    	}
	}
    
    if (state == STATE_FIND_PUCKS){
        if (ultrasonic_distances[LEFT_SIDE] < ARENA_LENGTH - ultrasonic_distances[RIGHT_SIDE] - SIDE_SENSORS_WIDTH - PUCK_TOLERANCE){ // Then we have discrepancy
    			
    	    }   
    }
    
}

int takeColourMeasurement(void){ // Return an integer from 0-3 indicating the guess of colour - Null, Red, Green, Blue
    // Complete
    int prediction = 0; // Edit this with correct integer
    return prediction;
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

void distanceCheck()
{
   
  for (ultrasonic_mux_control = 0; ultrasonic_mux_control < TOTAL_SONIC_SENSORS; ultrasonic_mux_control++) {

        distanceSensor(ultrasonic_mux_control);
        CyDelay(SENSOR_PULSE_DELAY_MS);                // delay of 100 milliseconds or adequate between different sensor pulses
  
    }
}







/* [] END OF FILE */
