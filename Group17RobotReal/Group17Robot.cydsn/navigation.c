
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

void armMoving(void){
    
    int puck_correct = FALSE;   // A flag to determine if the correct puck has been picked up
    int puck_scan;
    
    puckConstructionPlan[1] = RED;  // this was a test
    
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

void moveUntilPuck(int algorithm) {
      
    int puck_detect = 0;
    Motor_Left_Control_Write(0);        // move FORWARD
    Motor_Right_Control_Write(0);
    
    Motor_Left_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(70);
    Motor_Right_Driver_Wakeup();
    Motor_Right_Driver_WriteCompare(70);
    
    armOpen();                                  // ensures the arm is open
    control_photodiode_Write(CLAW_SENSING);     //ENSURES that it is the claw that is sensing
    colour_sensing_algorithm = algorithm;       
    
    while (puck_detect == 0) 
    {
        puck_detect = colourSensingOutput();
        CyDelay(10);
        sprintf(output, "%i \t", puck_detect);       
        UART_1_PutString(output);
    }
       
    Motor_Left_Decoder_SetCounter(0);
    Motor_Right_Decoder_SetCounter(0);
    
    Motor_Left_Driver_Sleep();
    Motor_Right_Driver_Sleep();
    
    moveDynamic(37, 100);
}



void moveUntil(int distance_set, int direction, int less_or_great, int ultrasonic_sensor, int speed) {
       
    
    // distance_set is in millimeteres
    // will check the front two ultrasonics 
    // will move until distance specified in function 
    
    //safetyDistanceCheck(); // CALL this to recalibrate
    
    int count_left;                 // counts the encoder values
    int count_right;
    int speed_left = speed;         // the set speed of the motors
    int speed_right = speed;
    int compare;                    
                                  // the ultrasonic sensor we will be using 
    int distance_sensor;            // the measured distance of the sensor
    
    int old_count = -SAFETY_MARGIN*ENCODER_MULTIPLIER;  // used in the old failsafe to check how much distance has passed 
                                                        // will enter the failsafe check straight away due to this
    int emergency_exit = FALSE;
    
    // The distance is in millimetres 
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
        
    
    if (less_or_great == GREATER_THAN) {                                         // if distance_set is negatve, it will move until the sensors are less than set value
        UART_1_PutString("\n GREATER THAN LOOP: \n");
        while (distance_sensor < abs(distance_set) && emergency_exit == FALSE) {               // This adjusts for drift 
            
            count_left = Motor_Left_Decoder_GetCounter();
            count_right = Motor_Right_Decoder_GetCounter();
            if (count_left > count_right) {
            speed_left -= ADJUST;
            }
            if (count_right > count_left) {
            speed_right -= ADJUST; 
            }
            
            Motor_Left_Driver_WriteCompare(speed_left);         // updates the driver speed
            Motor_Right_Driver_WriteCompare(speed_right);
            
            distanceSensor(ultrasonic_sensor);
            CyDelay(50);
            //distanceCheckOne(ultrasonic_sensor);                            // checks the distance
            distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
            sprintf(output, "%d \t", distance_sensor);       
            UART_1_PutString(output);
            
            // FAILSAFE:
            if (abs(count_left) > (old_count + SAFETY_MARGIN*ENCODER_MULTIPLIER - 100)){
                emergency_exit = failsafe(direction);
                old_count = count_left;
            }
        } 
    }
    
    if (less_or_great == LESS_THAN) {                                         // if distance_set is positive, it will move until the sensors are greater than set value 
        UART_1_PutString("\n LESS THAN LOOP: \n");
        while (distance_sensor > abs(distance_set) && emergency_exit == FALSE){                           // This adjusts for drift 
            
            count_left = Motor_Left_Decoder_GetCounter();
            count_right = Motor_Right_Decoder_GetCounter();
            if (count_left > count_right) {
            speed_left -= ADJUST;
            }
            if (count_right > count_left) {
            speed_right -= ADJUST; 
            }
            
            Motor_Left_Driver_WriteCompare(speed_left);         // updates the driver speed
            Motor_Right_Driver_WriteCompare(speed_right);
            
            distanceSensor(ultrasonic_sensor);
            CyDelay(50);
            distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
            sprintf(output, "%d \t", distance_sensor);       
            UART_1_PutString(output);
            
            // FAILSAFE:
            if (abs(count_left) > (old_count + SAFETY_MARGIN*ENCODER_MULTIPLIER - 100)){
                emergency_exit = failsafe(direction);
                old_count = count_left;
            }

        } 
    }
    
    
    
    //sprintf(output, "left motor: %d \n", count_left);       
    //UART_1_PutString(output);
    //sprintf(output, "right motor: %d \n", count_right);      
    //UART_1_PutString(output);
        
    // final check of sensor:
    distanceSensor(ultrasonic_sensor);
    CyDelay(50);
    distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
    sprintf(output, "\n fin: %d \n", distance_sensor);    
    UART_1_PutString(output);
    
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
        degree_change = degree_change/3;
        degree_change = -1*degree_change;
    }
    
    moveSwivel(degree_change, speed);
    internal_orientation = orientation_change;  
}

void translateUntil(int distance_set, int direction, int less_or_great, int ultrasonic_sensor, int speed){
    // distance_set is in millimeteres
    // will check the front two ultrasonics 
    // will move until distance specified in function 
    
    int count_left;                 // counts the encoder values
    int count_right;
    int old_count = -SAFETY_MARGIN*ENCODER_MULTIPLIER;   // used in the old failsafe to check how much distance has passed 
                                                        // will enter the failsafe straight away due to this
    int speed_left = speed;         // the set speed of the motors
    int speed_right = speed;
    int compare;
    int emergency_exit = FALSE;     // BREAKS out of the code in an emergency 
                                  // the ultrasonic sensor we will be using 
    int distance_sensor;            // the measured distance of the sensor
    
    // The distance is in millimetres 
    // Is the distance negative or positive? 
    if (direction == LEFT) { 
        Motor_Left_Control_Write(0); 
        Motor_Right_Control_Write(0); 
        //ultrasonic_sensor = FRONT_LEFT;       // use the back ultrasonic to test our distance
                                        // THIS CHECKS THE FRONT LEFT SENSOR, for some reason?
        compare = 32000;                // max compare value
    }
    if (direction == RIGHT) {
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
        while (distance_sensor > abs(distance_set) && emergency_exit == FALSE) {                // This adjusts for drift 
            count_left = Motor_Left_Decoder_GetCounter();
            count_right = Motor_Right_Decoder_GetCounter();
            if (count_left > count_right) {
            speed_left -= ADJUST;
            }
            if (count_right > count_left) {
            speed_right -= ADJUST; 
            }
            
            Motor_Left_Driver_WriteCompare(speed_left);     // update the drivers
            Motor_Right_Driver_WriteCompare(speed_right);
            
            distanceSensor(ultrasonic_sensor);
            CyDelay(50);
            //distanceCheckOne(ultrasonic_sensor);                            // checks the distance
            distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
            sprintf(output, "%d \t", distance_sensor);       
            UART_1_PutString(output);
            
            // FAILSAFE:
            if (abs(count_left) > (old_count + SAFETY_MARGIN*ENCODER_MULTIPLIER - 100)){
                emergency_exit = failsafe(direction);
                old_count = count_left;
            }
            
            
        }
    }
    
    if (less_or_great == GREATER_THAN){                                         // if distance_set is negatve, it will move until the sensors are less than set value
        while (distance_sensor < abs(distance_set) && emergency_exit == FALSE) {                // This adjusts for drift 
            count_left = Motor_Left_Decoder_GetCounter();
            count_right = Motor_Right_Decoder_GetCounter();
            if (count_left > count_right) {
            speed_left -= ADJUST;
            }
            if (count_right > count_left) {
            speed_right -= ADJUST; 
            }
            
            Motor_Left_Driver_WriteCompare(speed_left);         // update the drivers
            Motor_Right_Driver_WriteCompare(speed_right);
            
            distanceSensor(ultrasonic_sensor);
            CyDelay(50);
            
            distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
            sprintf(output, "%d \t", distance_sensor);       
            UART_1_PutString(output);
            
            // FAILSAFE:
            if (abs(count_left) > (old_count + SAFETY_MARGIN*ENCODER_MULTIPLIER - 100)){
                emergency_exit = failsafe(direction);
                old_count = count_left;
            }
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

int failsafe(int direction) {
    int check_distance;
    
    // SAFETY OVERRIDE:
    if(safety_override == TRUE){
        return FALSE;
    }
    
    if (direction == BACKWARD) {
        distanceSensor(BACK);
        CyDelay(50);
        
        check_distance = ultrasonic_distances_mm[BACK];   // checks the distance measured by the ultrasonic
        if (check_distance < SAFETY_MARGIN) { UART_1_PutString("SAFETY MARGIN ACTIVATED\n"); return TRUE; }
        else { return FALSE; }
    }
    if (direction == FORWARD) {
        distanceSensor(FRONT_LEFT);
        CyDelay(50);
        //distanceSensor(FRONT_RIGHT);      // May need to add this in of front left sensor isnt enough
        //CyDelay(50);
        
        check_distance = ultrasonic_distances_mm[FRONT_LEFT];   // checks the distance measured by the ultrasonic
        if (check_distance < SAFETY_MARGIN) { UART_1_PutString("SAFETY MARGIN ACTIVATED\n"); return TRUE; }
        else { return FALSE; }
    }
    // Considerations:
        // we may not want this failsafe enabled in some cases
    
    
}


void puckZoneFinding(void) {
    
    
    
    
}


void straightAdjust(void) {
       
    int front_left;
    int front_right;
    int difference = 300;
    int tolerance = 3;
    int max_difference = 800;
       
    while (abs(difference) > max_difference) {              // ensures the initial readings are accurate
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
            while (abs(difference) > tolerance 
                && difference > 0
                && abs(difference) < max_difference)     // ensures working correctly
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
        else {      // HERE we are turning right
            Motor_Left_Control_Write(1); Motor_Right_Control_Write(0);
            while (abs(difference) > tolerance 
                        && difference > 0
                        && abs(difference) < max_difference) 
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
            // consider putting in a for loop to get new readings
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
       
}


void straightAdjustBack(void) {
    // Move left, if number gets bigger, then move right
    // keep moving left & right until a minimum distance is found 
        // should we add another sensor to the back of the robot??? 
        //  will adding another sensor affect anything? 
    int distance_check;
    int distance_previous = 0;  // ensures we will enter the while loop
    int direction = LEFT;       // start off turning left first 
                                
        
    distanceSensor(BACK);
    CyDelay(DELAY);
    
    distance_check = ultrasonic_distances_mm[BACK];
    
    while(distance_previous != distance_check) {
        if (direction == LEFT) { Motor_Left_Control_Write(0); Motor_Right_Control_Write(1); }
        else {Motor_Left_Control_Write(1); Motor_Right_Control_Write(0); }
        
        while (distance_previous < distance_check) {
            // this will run while:
                // new distance values are getting smaller           
            
            // Updating the distance values: 
            distance_previous = distance_check;
            distanceSensor(BACK);
            CyDelay(DELAY);
            distance_check = ultrasonic_distances_mm[BACK];
        }
        if (direction == LEFT) {direction = RIGHT; }        // This changes the directions
        else {direction = LEFT; }
        
        // This may enter an infinite loop where it can't decide if it is correct 
    }
    
    
}

void locatePucks(void)
{
    
    
    
    
}



