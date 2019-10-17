
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

void armMoving(void){
    
    int puck_correct = FALSE;   // A flag to determine if the correct puck has been picked up
    int puck_scan;
    
    puckConstructionPlan[1] = RED;
    
    // States
    // moving
    changeHeightToPuck(3);  // Arm has to be lifted to highest position
        // THIS WILL be ensured by always returning the claw to the highest position after completing any action
        
    // Scanning puck:
    while (puck_correct == FALSE) 
    {
                                // robot moves forward towards puck
        changeHeightToPuck(1);  // arm lowers onto robot
        puck_scan = colourSensingOutput();  // colour sensor takes a scan
        changeHeightToPuck(3);  // arm returns to high position
                                // robot moves backwards
        if (puck_scan == puckConstructionPlan[current_stage]) {puck_correct = TRUE;}
                                // if colour == true:
                                    // enter picking up puck from 
        else 
        {
                                // if colour == false
                                    // robot translates to side to see next puck, and performs scanning puck again
        }
    }
    
    /*
    
    // Picking Up puck from pile:
    changeHeightToPuck(0);      // arm lowers to ground
                                // robot moves forward
    armClose();                 // claw closes on puck
    changeHeightToPuck(3);      // arm lifts up to highest position
                                // robot moves back away from puck area   
      
    
    // Deposition puck in construction zone:
                                // arm will be in highest position from moving
        changeHeightToPuck(current_stage-1);    // arm moves to position of stacking pucks          
                                                // stage 1st = ground/0
                                                // stage 2nd = 1
                                                // stage 3rd = 2
        armOpen();              // gripper releases puck
        changeHeightToPuck(3);  // arm moves back to highest position availabe
                                // robot moves away 
    */
    
}

void moveUntilPuck(void) {
    int puck_detect = 0;
    Motor_Left_Control_Write(0);
    Motor_Right_Control_Write(0);
    
    Motor_Left_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(SPEED);
    Motor_Right_Driver_Wakeup();
    Motor_Right_Driver_WriteCompare(SPEED);
    
    colour_sensing_algorithm = 2;       
    
    while (puck_detect == 0) 
    {
        puck_detect = colourSensingOutput();
        CyDelay(10);
        //sprintf(output, "%i \t", puck_detect);       
        //UART_1_PutString(output);
    }
       
    Motor_Left_Decoder_SetCounter(0);
    Motor_Right_Decoder_SetCounter(0);
    
    Motor_Left_Driver_Sleep();
    Motor_Right_Driver_Sleep();
    
    mishaMoveDynamic(30, SPEED);
    
}

void straightAdjust(void) {
       
    int front_left;
    int front_right;
    int difference = 300;
    int tolerance = 3;
    
    //distanceCheck(); 
    
    while (abs(difference) > 200) {              // ensures the initial readings are accurate
        distanceSensor(FRONT_LEFT);
        CyDelay(50);                        // 50ms might not be enough ???
        sprintf(output, "%d \t", ultrasonic_distances_mm[FRONT_LEFT]);
        UART_1_PutString(output);
        
        distanceSensor(FRONT_RIGHT);
        CyDelay(50);
        sprintf(output, "%d \t", ultrasonic_distances_mm[FRONT_RIGHT]);
        UART_1_PutString(output);
        
        difference = ultrasonic_distances_mm[FRONT_LEFT] - ultrasonic_distances_mm[FRONT_RIGHT];
        sprintf(output, "difference = %d, \n", difference);       
        UART_1_PutString(output);
    }
    
    
    int speed_left = 25;        // slow speed
    int speed_right = 25;
    
    do {
        if (difference > 0)             // This means we need to move it right
        {
            Motor_Left_Control_Write(0); Motor_Right_Control_Write(1); 
            while (abs(difference) > tolerance && abs(difference) < 200)     // ensures working correctly
            {
            Motor_Left_Driver_Wakeup();
            Motor_Left_Driver_WriteCompare(speed_left);
            Motor_Right_Driver_Wakeup();
            Motor_Right_Driver_WriteCompare(speed_right);

            distanceSensor(FRONT_LEFT);
            CyDelay(50);
            distanceSensor(FRONT_RIGHT);
            CyDelay(50);
                      
            difference = ultrasonic_distances_mm[FRONT_LEFT] - ultrasonic_distances_mm[FRONT_RIGHT];
            
            sprintf(output, "%d, %d \n", ultrasonic_distances_mm[FRONT_LEFT], ultrasonic_distances_mm[FRONT_RIGHT]);       
            UART_1_PutString(output);
            
            sprintf(output, "difference = %d, \n", difference);       
            UART_1_PutString(output);
            
            
            }
            
        }
        else {
            Motor_Left_Control_Write(1); Motor_Right_Control_Write(0);
            while (abs(difference) > tolerance && abs(difference) < 200) 
            {
            Motor_Left_Driver_Wakeup();
            Motor_Left_Driver_WriteCompare(speed_left);
            Motor_Right_Driver_Wakeup();
            Motor_Right_Driver_WriteCompare(speed_right); 
            
            distanceSensor(FRONT_LEFT);
            CyDelay(50);
            distanceSensor(FRONT_RIGHT);
            CyDelay(50);
                      
            difference = ultrasonic_distances_mm[FRONT_LEFT] - ultrasonic_distances_mm[FRONT_RIGHT];
            
            sprintf(output, " %d , %d \t", ultrasonic_distances_mm[FRONT_LEFT], ultrasonic_distances_mm[FRONT_RIGHT]);       
            UART_1_PutString(output);
            
            sprintf(output, "dif: %d, \n", difference);       
            UART_1_PutString(output);
            
            }
            
        }
        
        
        // CHECKS one more time to ensure the difference is accurate: 
        distanceSensor(FRONT_LEFT);
        CyDelay(50);
        distanceSensor(FRONT_RIGHT);
        CyDelay(50);
                      
        difference = ultrasonic_distances_mm[FRONT_LEFT] - ultrasonic_distances_mm[FRONT_RIGHT];
        sprintf(output, "%d, %d \n", ultrasonic_distances_mm[FRONT_LEFT], ultrasonic_distances_mm[FRONT_RIGHT]);       
        UART_1_PutString(output);
        sprintf(output, "difference = %d, \n", difference);       
        UART_1_PutString(output);
        
    } while (abs(difference) > 10);        // This ensures that the turning worked correctly
    
    Motor_Left_Driver_Sleep();
    Motor_Right_Driver_Sleep();
    
    // I WANT TO CREATE A FUNCTION THAT TAKES A CONTINOUS POLL OF THE UTLRASONICS 
    
    
    /*

    while(abs(difference) > tolerance)
    {
        distanceCheck();         // updates the distances
        difference = ultrasonic_distances_mm[FRONT_LEFT] - ultrasonic_distances_mm[FRONT_RIGHT];
        sprintf(output, "%d, \t", ultrasonic_distances_mm[FRONT_LEFT]);       
        UART_1_PutString(output);
        sprintf(output, "%d \n", ultrasonic_distances_mm[FRONT_RIGHT]);      
        UART_1_PutString(output);
        
        if (ultrasonic_distances_mm[FRONT_LEFT] < ultrasonic_distances_mm[FRONT_RIGHT] 
                || ultrasonic_distances_mm[FRONT_LEFT] > 8000) 
        {
            mishaSwivel(-1);    // we want to swivel left
        }
        else if (ultrasonic_distances_mm[FRONT_LEFT] > ultrasonic_distances_mm[FRONT_RIGHT])
        {
            mishaSwivel(1);    // want to swivel right
        }
        
    }
    
    */

}

void moveUntil(int distance_set, int direction, int less_or_great, int ultrasonic_sensor, int speed) {
    // distance_set is in millimeteres
    // will check the front two ultrasonics 
    // will move until distance specified in function 
    
    int count_left;                 // counts the encoder values
    int count_right;
    int speed_left = speed;         // the set speed of the motors
    int speed_right = speed;
    int compare;                    
                                  // the ultrasonic sensor we will be using 
    int distance_sensor;            // the measured distance of the sensor
    
    // The distance is in millimetres 
    // Is the distance negative or positive? 
    if (direction == FORWARD) { 
        Motor_Left_Control_Write(0); 
        Motor_Right_Control_Write(0); 
        //ultrasonic_sensor = FRONT_LEFT;       // use the back ultrasonic to test our distance
                                        // THIS CHECKS THE FRONT LEFT SENSOR, for some reason?
        compare = 32000;                // max compare value
    }
    if (direction == BACKWARD) {
        Motor_Left_Control_Write(1); 
        Motor_Right_Control_Write(1);
        //ultrasonic_sensor = BACK; // We just use one ultrasonic front sensor
        compare = -32000;               // min compare value
    }
    
    ultrasonic_distances_mm[ultrasonic_sensor] = ARENA_WIDTH + 100;             // So it will enter the while loop
    while (ultrasonic_distances_mm[ultrasonic_sensor] > ARENA_WIDTH) {          // protects against dodgy initial values
        distanceSensor(ultrasonic_sensor);
        CyDelay(50);
        distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the   
        sprintf(output, "%d \t", distance_sensor);       
        UART_1_PutString(output);
    }
    
    Motor_Left_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(speed_left);
    Motor_Right_Driver_Wakeup();
    Motor_Right_Driver_WriteCompare(speed_right);
        
    if (less_or_great == LESS_THAN) {                                         // if distance_set is positive, it will move until the sensors are greater than set value 
        while (distance_sensor > abs(distance_set)) {                // This adjusts for drift 
            count_left = Motor_Left_Decoder_GetCounter();
            count_right = Motor_Right_Decoder_GetCounter();
            if (count_left > count_right) {
            speed_left -= ADJUST;
            }
            if (count_right > count_left) {
            speed_right -= ADJUST; 
            }
            distanceSensor(ultrasonic_sensor);
            CyDelay(50);
            //distanceCheckOne(ultrasonic_sensor);                            // checks the distance
            distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
            sprintf(output, "%d \t", distance_sensor);       
            UART_1_PutString(output);
        }
    }
    
    if (less_or_great == GREATER_THAN){                                         // if distance_set is negatve, it will move until the sensors are less than set value
        while (distance_sensor < abs(distance_set)) {                // This adjusts for drift 
            count_left = Motor_Left_Decoder_GetCounter();
            count_right = Motor_Right_Decoder_GetCounter();
            if (count_left > count_right) {
            speed_left -= ADJUST;
            }
            if (count_right > count_left) {
            speed_right -= ADJUST; 
            }
            distanceSensor(ultrasonic_sensor);
            CyDelay(50);
            //distanceCheckOne(ultrasonic_sensor);                            // checks the distance
            distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
            sprintf(output, "%d \t", distance_sensor);       
            UART_1_PutString(output);
        }
    }
    
    //sprintf(output, "left motor: %d \n", count_left);       
    //UART_1_PutString(output);
    //sprintf(output, "right motor: %d \n", count_right);      
    //UART_1_PutString(output);
        
    Motor_Left_Decoder_SetCounter(0);                       // RESET the decoders to 0
    Motor_Right_Decoder_SetCounter(0);   
    
    Motor_Left_Driver_Sleep();              // Puts motors to sleep 
    Motor_Right_Driver_Sleep();
   
    
}

void changeOrientation(int orientation_change, int speed) {
    //define NORTH 0
    //define EAST 1
    //define SOUTH 2
    //define WEST 3
    int degree_change;
    
    // Incorporate some failsafes so that it turns in a direction that won't hit anything:
        // failsafes:
    
    int change = orientation_change - internal_orientation;
    degree_change = 90*change;                                  // converts the amount of degrees we need to change
    
    if (degree_change == 270 || change == -270)
    {
        degree_change = degree_change/270;
        degree_change = -1*degree_change;
    }
    
    mishaSwivel(degree_change, speed);
    internal_orientation = orientation_change;  
}



void locatePucks(void)
{
    
    
    
    
}