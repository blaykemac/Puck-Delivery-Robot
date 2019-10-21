
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
    changeHeightToPuck(3, NEITHER);  // Arm has to be lifted to highest position
        // THIS WILL be ensured by always returning the claw to the highest position after completing any action
        
    // Scanning puck:
    while (puck_correct == FALSE) 
    {
                                // robot moves forward towards puck
        changeHeightToPuck(1, NEITHER);  // arm lowers onto robot
        puck_scan = colourSensingOutput();  // colour sensor takes a scan
        changeHeightToPuck(3, NEITHER);  // arm returns to high position
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
    
    int speed = 70;
    
    int count_right;
    int count_left;
    int speed_left = speed;
    int speed_right = speed;
    
    Motor_Left_Control_Write(0);        // move FORWARD
    Motor_Right_Control_Write(0);
    
    Motor_Left_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(speed_left);
    Motor_Right_Driver_Wakeup();
    Motor_Right_Driver_WriteCompare(speed_right);
    
    armOpen();                                  // ensures the arm is open
    control_photodiode_Write(CLAW_SENSING);     //ENSURES that it is the claw that is sensing
    colour_sensing_algorithm = algorithm;       
    
    while (puck_detect == 0) 
    {
        count_right = Motor_Right_Decoder_GetCounter();        
        count_left = Motor_Left_Decoder_GetCounter();
        puck_detect = colourSensingOutput();
        CyDelay(10);
        sprintf(output, "%i \t", puck_detect);       
        UART_1_PutString(output);
        
        // DRIFT CORRECTION: 
        
        if (abs(count_left) > abs(count_right)) {
            speed_left = speed - ADJUST;
            speed_right = speed + ADJUST;
        }
        if (abs(count_right) > abs(count_left)) {
            speed_right = speed - ADJUST;             
            speed_left = speed + ADJUST;
        } 
        Motor_Left_Driver_WriteCompare(speed_left);
        Motor_Right_Driver_WriteCompare(speed_right);        
    }
    
    Motor_Left_Decoder_SetCounter(0);
    Motor_Right_Decoder_SetCounter(0);
    
    Motor_Left_Driver_Sleep();
    Motor_Right_Driver_Sleep();
    
    moveDynamic(37, 100, FALSE);
}



void moveUntil(int distance_set, int direction, int less_or_great, int ultrasonic_sensor, int speed, int activate_safety) {
    // This function will move until given a set distance (in mm)
    // Can indicate the direction (FORWARD or BACKWARD)
    // whether you want it to move until it is greater or less than the set distance
    // which ultrasonic_sensor you want to use
    // the speed you want to move
    // can activate_safety (TRUE or FALSE) so that it will always stop before it hits a wall (moving forward or backward)
    
    // safetyDistanceCheck();        // CALL this if you want to recalibrate
    
    int count_left;                 // counts the encoder values
    int count_right;
    int speed_left = speed;         // the set speed of the motors
    int speed_right = speed;
    int front_sensor_flag = FALSE;
                                  // the ultrasonic sensor we will be using 
    int distance_sensor;            // the measured distance of the sensor
    
    int old_count = -SAFETY_MARGIN*ENCODER_MULTIPLIER;  // used in the old failsafe to check how much distance has passed 
                                                        // will enter the failsafe check straight away due to this
    int emergency_exit = FALSE;
    
    // Check if we are closer to or further away from the side wall: 
        int side_prox_sensor;
        int initial_side_distance;
        distanceSensor(SIDE_LEFT);
        CyDelay(50);
        distanceSensor(SIDE_RIGHT);
        CyDelay(50);
        if (ultrasonic_distances_mm[SIDE_LEFT] > ultrasonic_distances_mm[SIDE_RIGHT]) {
            side_prox_sensor = SIDE_RIGHT; 
            initial_side_distance = ultrasonic_distances_mm[SIDE_RIGHT];
        }
        else { side_prox_sensor = SIDE_LEFT; initial_side_distance = ultrasonic_distances_mm[SIDE_LEFT]; } ;   
    
    // Setting the direction 
    if (direction == FORWARD) { 
        Motor_Left_Control_Write(0); 
        Motor_Right_Control_Write(0); 
    }
    if (direction == BACKWARD) {
        Motor_Left_Control_Write(1); 
        Motor_Right_Control_Write(1);
    }
    
    
    if (ultrasonic_sensor == FRONT_SENSORS) {ultrasonic_sensor = FRONT_LEFT; front_sensor_flag = TRUE; }    // this destermines if we want to use both front sensors
    
    // This ensures that the initial sensing value is good 
    ultrasonic_distances_mm[ultrasonic_sensor] = ARENA_WIDTH + 100;             // Enters the while loop
    while (ultrasonic_distances_mm[ultrasonic_sensor] > ARENA_WIDTH) {          // protects against dodgy initial values
        distanceSensor(ultrasonic_sensor);
        CyDelay(50);
        distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the   
        sprintf(output, "%d \t", distance_sensor);       
        UART_1_PutString(output);
    }
    
    if (front_sensor_flag == TRUE) {ultrasonic_sensor = FRONT_SENSORS; front_sensor_flag = FALSE; } 
    
    Motor_Left_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(speed_left);
    Motor_Right_Driver_Wakeup();
    Motor_Right_Driver_WriteCompare(speed_right);
    
    if (less_or_great == GREATER_THAN) {                                         // if distance_set is negatve, it will move until the sensors are less than set value
        //UART_1_PutString("\n GREATER THAN LOOP: \n");
        while (distance_sensor < abs(distance_set) && emergency_exit == FALSE) {               // This adjusts for drift 
            
            count_left = Motor_Left_Decoder_GetCounter();
            count_right = Motor_Right_Decoder_GetCounter();
            
            // DRIFT CORRECTION:
            if (abs(count_left) > abs(count_right)) {
                speed_left = speed - ADJUST;
                speed_right = speed + ADJUST;
            }        
            if (abs(count_right) > abs(count_left)) {
                speed_right = speed - ADJUST;              
                speed_left = speed + ADJUST;
            }
            // end of drift correction 
            
            Motor_Left_Driver_WriteCompare(speed_left);         // updates the driver speed
            Motor_Right_Driver_WriteCompare(speed_right);
            
            // When you want to do both front sensors: 
            if (ultrasonic_sensor == FRONT_SENSORS && front_sensor_flag == TRUE) {
                distanceSensor(FRONT_LEFT);
                CyDelay(50);
                distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the 
                front_sensor_flag = FALSE;
            }
            else if (ultrasonic_sensor == FRONT_SENSORS && front_sensor_flag == FALSE) {
                distanceSensor(FRONT_RIGHT);
                CyDelay(50);
                distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the 
                front_sensor_flag = TRUE;
            }
            else {
                distanceSensor(ultrasonic_sensor);
                CyDelay(50);
                distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
                sprintf(output, "%d \t", distance_sensor);       
                UART_1_PutString(output);
            }
            
            // FAILSAFES:
                // the failsafes will alternate between each other if failsafe is activated
                // they should both run within one safety margin
                // if you want to get rid of one of the failsafes, you will need to double the safety margin
            int failsafe_alternator = TRUE;
            
            // FAILSAFE forward & backward
            if (abs(count_left) > (old_count + (SAFETY_MARGIN/2)*ENCODER_MULTIPLIER - 100) 
                                        && failsafe_alternator == TRUE 
                                        && activate_safety == TRUE){
                emergency_exit = failsafe(direction);
                old_count = count_left;
                failsafe_alternator = FALSE;
            }
                        
            //  side sensors sensors
            if (abs(count_left) > (old_count + (SAFETY_MARGIN/2)*ENCODER_MULTIPLIER - 100)
                                        && failsafe_alternator == FALSE 
                                        && activate_safety == TRUE){
                failsafeSideSensors(side_prox_sensor, initial_side_distance);
                old_count = count_left;
                failsafe_alternator = TRUE;
            }
        } 
    }
    
    if (less_or_great == LESS_THAN) {                                         // if distance_set is positive, it will move until the sensors are greater than set value 
        //UART_1_PutString("\n LESS THAN LOOP: \n");
        while (distance_sensor > abs(distance_set) && emergency_exit == FALSE){                           // This adjusts for drift 
            
            count_left = Motor_Left_Decoder_GetCounter();
            count_right = Motor_Right_Decoder_GetCounter();
            
            // DRIFT CORRECTION:
            if (abs(count_left) > abs(count_right)) {
                speed_left = speed - ADJUST;
                speed_right = speed + ADJUST;
            }        
            if (abs(count_right) > abs(count_left)) {
                    speed_right = speed - ADJUST;              // If the speeds are equal, we decrememnt within the specific 
                                                        // adjust tolerance 
                    speed_left = speed + ADJUST;
            }
            // end of drift correction 
            
            Motor_Left_Driver_WriteCompare(speed_left);         // updates the driver speed
            Motor_Right_Driver_WriteCompare(speed_right);
            
            distanceSensor(ultrasonic_sensor);
            CyDelay(50);
            distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
            sprintf(output, "%d \t", distance_sensor);       
            UART_1_PutString(output);
            
            // FAILSAFES:
                // the failsafes will alternate between each other if failsafe is activated
                // they should both run within one safety margin
                // if you want to get rid of one of the failsafes, you will need to double the safety margin
            int failsafe_alternator = TRUE;
            
            // FAILSAFE forward & backward
            if (abs(count_left) > (old_count + (SAFETY_MARGIN/2)*ENCODER_MULTIPLIER - 100) 
                                        && failsafe_alternator == TRUE 
                                        && activate_safety == TRUE){
                emergency_exit = failsafe(direction);
                old_count = count_left;
                failsafe_alternator = FALSE;
            }
                        
            //  side sensors sensors
            if (abs(count_left) > (old_count + (SAFETY_MARGIN/2)*ENCODER_MULTIPLIER - 100)
                                        && failsafe_alternator == FALSE 
                                        && activate_safety == TRUE){
                failsafeSideSensors(side_prox_sensor, initial_side_distance);
                old_count = count_left;
                failsafe_alternator = TRUE;
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
    
    if (degree_change == 270 || degree_change == -270)
    {
        degree_change = degree_change/3;        
        degree_change = -1*degree_change;       
    }
    
    moveSwivel(degree_change, speed, TRUE);
    internal_orientation = orientation_change;  
}

void translateUntil(int distance_set, int direction, int less_or_great, int ultrasonic_sensor, int speed){
    // distance_set is in millimeteres
    // will check the front two ultrasonics 
    // will move until distance specified in function 
    
    // direction can be left or right
    
    int count_left;                 // counts the encoder values
    int count_right;
    int old_count = -SAFETY_MARGIN*ENCODER_MULTIPLIER;   // used in the old failsafe to check how much distance has passed 
                                                        // will enter the failsafe straight away due to this
    int speed_left = speed;         // the set speed of the motors
    int speed_right = speed;
    //int emergency_exit = FALSE;     // BREAKS out of the code in an emergency 
                                  // the ultrasonic sensor we will be using 
    int distance_sensor;            // the measured distance of the sensor

    
    ultrasonic_distances_mm[ultrasonic_sensor] = ARENA_WIDTH + 100;             // So it will enter the while loop
    while (ultrasonic_distances_mm[ultrasonic_sensor] > ARENA_WIDTH) {          // protects against dodgy initial values
        distanceSensor(ultrasonic_sensor);
        CyDelay(50);
        distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the   
        sprintf(output, "%d \t", distance_sensor);       
        UART_1_PutString(output);
    }
    
    //Motor_Left_Driver_Wakeup();
    //Motor_Left_Driver_WriteCompare(speed_left);
    //Motor_Right_Driver_Wakeup();
    //Motor_Right_Driver_WriteCompare(speed_right);
        
    if (less_or_great == LESS_THAN) {                                         // if distance_set is positive, it will move until the sensors are greater than set value 
        while (distance_sensor > abs(distance_set)) {                // This adjusts for drift 
            
            UART_1_PutString("entering greater than\n");
            
            count_left = Motor_Left_Decoder_GetCounter();
            count_right = Motor_Right_Decoder_GetCounter();
            
            if (direction == LEFT) {
                translateMoveDynamic(-5, 7, speed, FALSE);      // This will translate to the left by one puck
            }
            
            if (direction == RIGHT) {
                translateMoveDynamic(5, -7, speed, FALSE);      // This will translate to the right by one puck
            }
                        
            //Motor_Left_Driver_WriteCompare(speed_left);     // update the drivers
            //Motor_Right_Driver_WriteCompare(speed_right);
            
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
            
            UART_1_PutString("entering less than\n");

            if (direction == LEFT) {
                translateMoveDynamic(-5, 7, speed, FALSE);      // This will translate to the left by one puck
            }
            
            if (direction == RIGHT) {
                translateMoveDynamic(5, -7, speed, FALSE);      // This will translate to the right by one puck
            }
            
            //Motor_Left_Driver_WriteCompare(speed_left);         // update the drivers
            //Motor_Right_Driver_WriteCompare(speed_right);
            
            distanceSensor(ultrasonic_sensor);
            CyDelay(50);
            
            distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
            sprintf(output, "%d \t", distance_sensor);       
            UART_1_PutString(output);
            
        }
    }
    
    //sprintf(output, "left motor: %d \n", count_left);       
    //UART_1_PutString(output);
    //sprintf(output, "right motor: %d \n", count_right);      
    //UART_1_PutString(output);
        
    UART_1_PutString("exited loop");
    
    Motor_Left_Decoder_SetCounter(0);                       // RESET the decoders to 0
    Motor_Right_Decoder_SetCounter(0);   
    
    //Motor_Left_Driver_Sleep();              // Puts motors to sleep 
    //Motor_Right_Driver_Sleep();
    
}

int failsafe(int direction) {
    int check_distance;
    
    // SAFETY OVERRIDE:
    if(safety_override == TRUE){
        return FALSE;
    }
    
    // detecting direction:    
    if (direction == BACKWARD) {
        distanceSensor(BACK);
        CyDelay(50);
        
        check_distance = ultrasonic_distances_mm[BACK];   // checks the distance measured by the ultrasonic
        if (check_distance < SAFETY_MARGIN && check_distance > 0) { 
            sprintf(output, "SAFETY MARGIN ACTIVATED @ %d\n", check_distance);      
            UART_1_PutString(output); 
            return TRUE; 
        }
        else { return FALSE; }
    }
    else if (direction == FORWARD) {
        distanceSensor(FRONT_LEFT);
        CyDelay(50);
        //distanceSensor(FRONT_RIGHT);      // May need to add this in of front left sensor isnt enough
        //CyDelay(50);
        
        check_distance = ultrasonic_distances_mm[FRONT_LEFT];   // checks the distance measured by the ultrasonic
        if (check_distance < SAFETY_MARGIN && check_distance > 0) { 
            sprintf(output, "SAFETY MARGIN ACTIVATED @ %d\n", check_distance);      
            UART_1_PutString(output); 
            return TRUE; 
        }
        else { return FALSE; }
    }
    
    // detecting side sensor usage: 
    
    
    
    // Considerations:
        // we may not want this failsafe enabled in some cases
    
}

void failsafeSideSensors(int side_sensing, int initial_value) {
    
    int drift_tolerance = 10;     // 10mm
    int translate_direction;
    
    if (side_sensing == SIDE_LEFT) {translate_direction = RIGHT;} else { translate_direction = LEFT; }
    
        // SAFETY OVERRIDE:
    if(safety_override == TRUE){
        return;
    }
    
    distanceSensor(side_sensing);
    CyDelay(50);
    
    if (ultrasonic_distances_mm[side_sensing] > initial_value + drift_tolerance 
                || ultrasonic_distances_mm[side_sensing] < initial_value - drift_tolerance ){
        // If it has drifted more than 10mm either side from the initial value, we perform a straight adjust
        //translateUntil(initial_value, translate_direction, GREATER_THAN,side_sensing,SPEED);   // will take us back to our initial position
                                                                                // if this got tripped by a block, it would mess everything up
        //translateMoveDynamic(translate_direction, 30, SPEED, FALSE);          // these are not the correct values 
        straightAdjust();                                                       // straight adjust to correct any problems from translating
        
    }
}


void straightAdjust(void) {
       
    int front_left;
    int front_right;
    int difference = 300;
    int tolerance = 1;
    int max_difference = 800;
    int ultra_delay = 50;
    int speed = 25;
       
    // Ensure the initial readings are accurate: 
    while (abs(difference) > max_difference) {              
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
        
    
    // Adjust against the wall: 
    int speed_left = speed;        // slow speed
    int speed_right = speed;
        
    while(abs(difference) > tolerance ) {
        
        if (difference > 0)             // This means we need to move it right
        {
            Motor_Left_Control_Write(0); Motor_Right_Control_Write(1); 
            
            Motor_Left_Driver_Wakeup();
            Motor_Left_Driver_WriteCompare(speed_left);
            Motor_Right_Driver_Wakeup();
            Motor_Right_Driver_WriteCompare(speed_right);
 
            while (abs(difference) > tolerance 
                        && difference > 0
                        && abs(difference) < max_difference) {    // ensures working correctly
                distanceSensor(FRONT_LEFT);
                CyDelay(ultra_delay);
                distanceSensor(FRONT_RIGHT);
                CyDelay(ultra_delay);
                          
                difference = ultrasonic_distances_mm[FRONT_LEFT] - ultrasonic_distances_mm[FRONT_RIGHT];
                
                sprintf(output, " %d , %d \t dif: %d, \n", ultrasonic_distances_mm[FRONT_LEFT], 
                                                            ultrasonic_distances_mm[FRONT_RIGHT],
                                                                                        difference);       
                UART_1_PutString(output);
            } 
        }
        else if (difference < 0 ){      // HERE we are turning left
            Motor_Left_Control_Write(1); Motor_Right_Control_Write(0);
            
            Motor_Left_Driver_Wakeup();
            Motor_Left_Driver_WriteCompare(speed_left);
            Motor_Right_Driver_Wakeup();
            Motor_Right_Driver_WriteCompare(speed_right);
            
            while (abs(difference) > tolerance 
                        && difference < 0
                        && abs(difference) < max_difference) {
                distanceSensor(FRONT_LEFT);
                CyDelay(ultra_delay);
                distanceSensor(FRONT_RIGHT);
                CyDelay(ultra_delay);
                          
                difference = ultrasonic_distances_mm[FRONT_LEFT] - ultrasonic_distances_mm[FRONT_RIGHT];
                           
                sprintf(output, " %d , %d \t dif: %d, \n", ultrasonic_distances_mm[FRONT_LEFT], 
                                                            ultrasonic_distances_mm[FRONT_RIGHT],
                                                                                        difference);       
                UART_1_PutString(output);
            
            }
            
        }
        
        // Comes out of the while loop if the max_difference is exceeded, but then the values might be wrong
        // if max_difference is exceeded, it should probably be forced to swivel the other way to 
        // that it was swivelling        
        
        
        // CHECKS one more time to ensure the difference is accurate: 
            // consider putting in a for loop to get new readings
        distanceSensor(FRONT_LEFT);
        CyDelay(ultra_delay);
        distanceSensor(FRONT_RIGHT);
        CyDelay(ultra_delay);
                      
        difference = ultrasonic_distances_mm[FRONT_LEFT] - ultrasonic_distances_mm[FRONT_RIGHT];
        sprintf(output, "%d, %d, ", ultrasonic_distances_mm[FRONT_LEFT], ultrasonic_distances_mm[FRONT_RIGHT]);       
        UART_1_PutString(output);
        sprintf(output, "dif: %d, \n", difference);       
        UART_1_PutString(output);
        
    }         // This ensures that the turning worked correctly
                                            // changed it from 10 to 2, this might change the way it works
    
    
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
    int minimum_distance;                           
        
    distanceSensor(BACK);
    CyDelay(DELAY);
    
    Motor_Left_Driver_Wakeup();
    Motor_Right_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(25);
    Motor_Right_Driver_WriteCompare(25);
    
    distance_check = ultrasonic_distances_mm[BACK];
    minimum_distance = distance_check;
    
    while(1) {
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
    
    Motor_Left_Driver_Sleep();
    Motor_Right_Driver_Sleep();   
    
}


void toleranceCheck(void) {
    
    // This function needs work

    int tolerance = 100;
    
    distanceSensor(FRONT_LEFT);
    CyDelay(60);
    distanceSensor(FRONT_LEFT);
    CyDelay(60);
    distanceSensor(BACK);
    CyDelay(60);
    
    while (ultrasonic_distances_mm[FRONT_LEFT] + ultrasonic_distances_mm[BACK] + tolerance < ARENA_WIDTH 
            
    
    
    
    || ultrasonic_distances_mm[FRONT_LEFT] + ultrasonic_distances_mm[BACK] + tolerance < ARENA_WIDTH) { 
            // this checks if the ultrasonic distances being recorded are accurate
            // if it is not within this threshold, there must be something up.  
                
    distanceSensor(FRONT_LEFT);
    CyDelay(60);
    distanceSensor(FRONT_LEFT);
    CyDelay(60);
    distanceSensor(BACK);
    CyDelay(60);
        
    }
    
}

void locatePucks(void)
{
    
    
    
    
}


void blockAndPuckZoneFinding(void) {
    // this function will scan while moving across, and depending on different thresholds, whill trigger
    // either for a block or a  puck
    
    int block_threshold; // the block toloerance will be anything less than 700mm - width of robot - opposite sensor
    
    int puck_threshold;     // the puck zone will be between 1200 - width of robot - opposite sensor 
                                                        //  & 1200 - grid_width - width of robot - opposite sensor 
    
    int distance_sensor;    // the value the ultrasonic sensors will give us
    
    // first check the south facing sensor's value, ensure that it is correct
    
    
    // need to scan the front or back sensors
    // This function can be generalised, but I'm going to first write it just assuming we are moving backwards, 
    // from the west wall towards the east wall
    
    // The following while loop will run while the sensor is above the puck threshold and inbetween
    // the puck threshold and the block threshold 
    // as soon as it moves outside of this threshold, it will trigger, and then we will determine whether it was
    // the puck or the block that triggere it
    
    // Then it will keep moving, looking to trigger only for the other object
    
    
    
    // Calculate the block threshold:
    block_threshold = 0;
    
    // Calulcate the puck threshold: 
    puck_threshold = 0;
    
    
    // Hardcoded values for both, assuming location of robot: (if dynamic versions don't work) 
    
    distance_sensor = (puck_threshold + block_threshold)/2;     // Used to initialise the while loop
    
    while (distance_sensor > puck_threshold) {
    
        
    
    } 
    if (distance_sensor < puck_threshold && distance_sensor > block_threshold) {
        // The puck sensor tripped the sensor
        
        
        
        // Enter while loop to find block: 
        while (distance_sensor > block_threshold) {
            
            
            
        }

    }
    else if (distance_sensor < block_threshold) {
        // The block tripped the sensor 
        
        
        
        // Enter while loop to find puck:         
        while (distance_sensor > puck_threshold && distance_sensor < block_threshold) {
            
            
        }
        
    }
    
}



/* [] END OF FILE */
