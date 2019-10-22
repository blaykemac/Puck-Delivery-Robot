
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
    
    puck_construction_plan[1] = RED;  // this was a test
    
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
        if (puck_scan == puck_construction_plan[current_stage]) {puck_correct = TRUE;}
                                // if colour == true:
                                    // enter picking up puck from 
        else 
        {
                                // if colour == false
                                    // robot translates to side to see next puck, and performs scanning puck again
        }
    }
    
}

void checkHoldingPuck(void) {
    // This function will check if we are holding a puck after having attempted to pick up a puck,
    // if we are not holding a puck, it will translate to the left or right, and then attempt to pick up
    // the puck again 
    int colour_check = colourSensingOutput();
    
    if (colour_check == BLANK) {
        // Reattempt to pick up the puck
        //straightAdjustSensor(FRONT_MIDDLE);
        moveUntilPuck(CLAW_GROUND_ALGORITHM);       // This may be the wrong algorithm, 
                                                    // as the claw is fairly above the ground now
    }
    else {
        // still have a puck, can continue as normal
        return;
    }
    
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
    
    int old_count = -(SAFETY_MARGIN)*ENCODER_MULTIPLIER*0.75 - 1000;  // used in the old failsafe to check how much distance has passed 
                                                        // will enter the failsafe check straight away due to this
    int emergency_exit = FALSE;
    int adjust = 3;
    
    if (speed > 190) {
        adjust = 4;
    }
    if (speed < 100) {
        adjust = 1;
    }
    
    
    
    // Check if we are closer to or further away from the side wall: 
    /*
        int side_prox_sensor;
        int initial_side_distance;
        distanceSensor(SIDE_LEFT);
        CyDelay(50);
        distanceSensor(SIDE_RIGHT);
        CyDelay(50);
        if (sensor_distances[SIDE_LEFT] > sensor_distances[SIDE_RIGHT]) {
            side_prox_sensor = SIDE_RIGHT; 
            initial_side_distance = sensor_distances[SIDE_RIGHT];
        }
        else { side_prox_sensor = SIDE_LEFT; initial_side_distance = sensor_distances[SIDE_LEFT]; } ;   
    */
    // Setting the direction 
    if (direction == FORWARD) { 
        Motor_Left_Control_Write(0); 
        Motor_Right_Control_Write(0); 
    }
    if (direction == BACKWARD) {
        Motor_Left_Control_Write(1); 
        Motor_Right_Control_Write(1);
    }
    
    
    //if (ultrasonic_sensor == FRONT_SENSORS) {ultrasonic_sensor = FRONT_LEFT; front_sensor_flag = TRUE; }    // this destermines if we want to use both front sensors
    
    // This ensures that the initial sensing value is good 
    sensor_distances[ultrasonic_sensor] = ARENA_WIDTH + 100;             // Enters the while loop
    while (sensor_distances[ultrasonic_sensor] > ARENA_WIDTH) {          // protects against dodgy initial values
        distanceSensor(ultrasonic_sensor);
        CyDelay(50);
        distance_sensor = sensor_distances[ultrasonic_sensor];   // checks the distance measured by the   
        sprintf(output, "%d \t", distance_sensor);       
        UART_1_PutString(output);
    }
    
    //if (front_sensor_flag == TRUE) {ultrasonic_sensor = FRONT_SENSORS; front_sensor_flag = FALSE; } 
    
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
                speed_left = speed - adjust;
                speed_right = speed + adjust;
            }        
            if (abs(count_right) > abs(count_left)) {
                speed_right = speed - adjust;              
                speed_left = speed + adjust;
            }
            // end of drift correction 
            
            Motor_Left_Driver_WriteCompare(speed_left);         // updates the driver speed
            Motor_Right_Driver_WriteCompare(speed_right);
            
            // When you want to do both front sensors: 
            if (ultrasonic_sensor == FRONT_SENSORS && front_sensor_flag == TRUE) {
                distanceSensor(FRONT_LEFT);
                CyDelay(50);
                distance_sensor = sensor_distances[ultrasonic_sensor];   // checks the distance measured by the 
                front_sensor_flag = FALSE;
            }
            else if (ultrasonic_sensor == FRONT_SENSORS && front_sensor_flag == FALSE) {
                distanceSensor(FRONT_RIGHT);
                CyDelay(50);
                distance_sensor = sensor_distances[ultrasonic_sensor];   // checks the distance measured by the 
                front_sensor_flag = TRUE;
            }
            else {
                distanceSensor(ultrasonic_sensor);
                CyDelay(50);
                distance_sensor = sensor_distances[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
                sprintf(output, "%d \t", distance_sensor);       
                UART_1_PutString(output);
            }
            
            // FAILSAFES:
                // the failsafes will alternate between each other if failsafe is activated
                // they should both run within one safety margin
                // if you want to get rid of one of the failsafes, you will need to double the safety margin
            int failsafe_alternator = TRUE;
            
            // FAILSAFE forward & backward
            if (abs(count_left) > (old_count + (SAFETY_MARGIN)*ENCODER_MULTIPLIER*0.75 - 100) 
                                        && failsafe_alternator == TRUE 
                                        && activate_safety == TRUE){
                emergency_exit = failsafe(direction);
                old_count = count_left;
                //failsafe_alternator = FALSE;
            }
                        
            //  side sensors sensors
            /*
            if (abs(count_left) > (old_count + (SAFETY_MARGIN/2)*ENCODER_MULTIPLIER - 100)
                                        && failsafe_alternator == FALSE 
                                        && activate_safety == TRUE){
                failsafeSideSensors(side_prox_sensor, initial_side_distance);
                old_count = count_left;
                failsafe_alternator = TRUE;
            }
            */                            
        } 
    }
    
    if (less_or_great == LESS_THAN) {                                         // if distance_set is positive, it will move until the sensors are greater than set value 
        //UART_1_PutString("\n LESS THAN LOOP: \n");
        while (distance_sensor > abs(distance_set) && emergency_exit == FALSE){                           // This adjusts for drift 
            
            count_left = Motor_Left_Decoder_GetCounter();
            count_right = Motor_Right_Decoder_GetCounter();
            
            // DRIFT CORRECTION:
            if (abs(count_left) > abs(count_right)) {
                speed_left = speed - adjust;
                speed_right = speed + adjust;
            }        
            if (abs(count_right) > abs(count_left)) {
                    speed_right = speed - adjust;              // If the speeds are equal, we decrememnt within the specific 
                                                        // adjust tolerance 
                    speed_left = speed + adjust;
            }
            // end of drift correction 
            
            Motor_Left_Driver_WriteCompare(speed_left);         // updates the driver speed
            Motor_Right_Driver_WriteCompare(speed_right);
            
            distanceSensor(ultrasonic_sensor);
            CyDelay(50);
            distance_sensor = sensor_distances[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
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
                //failsafe_alternator = FALSE;
            }
                        
            //  side sensors sensors
            /*
            if (abs(count_left) > (old_count + (SAFETY_MARGIN/2)*ENCODER_MULTIPLIER - 100)
                                        && failsafe_alternator == FALSE 
                                        && activate_safety == TRUE){
                failsafeSideSensors(side_prox_sensor, initial_side_distance);
                old_count = count_left;
                failsafe_alternator = TRUE;
            }
            */                            
        } 
    }
    
    
    
    //sprintf(output, "left motor: %d \n", count_left);       
    //UART_1_PutString(output);
    //sprintf(output, "right motor: %d \n", count_right);      
    //UART_1_PutString(output);
        
    // final check of sensor:
    distanceSensor(ultrasonic_sensor);
    CyDelay(50);
    distance_sensor = sensor_distances[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
    sprintf(output, "\n fin: %d \n", distance_sensor);    
    UART_1_PutString(output);
    
    global_encoder = Motor_Left_Decoder_GetCounter();;                            
    global_distance = global_encoder/ENCODER_MULTIPLIER;        // tells us how far we travelled in this moveuntil block 
    
    sprintf(output, "count value: %d \n", global_encoder);
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

void translateUntil(int distance, float max_back, int left_or_right, int less_or_great){
    int current_distance;
    float angle_to_turn_at = 20; 
    int maxBack1 = round(max_back/cos((M_PI/180)*angle_to_turn_at))- 1;
    if (left_or_right == RIGHT){ 
        if (less_or_great == LESS_THAN) {
            distanceSensor(SIDE_RIGHT);
            current_distance = sensor_distances[SIDE_RIGHT];
            while (current_distance > distance){
                if ((current_distance - distance) > maxBack1*sin((M_PI/180)*angle_to_turn_at)){
                    translateMoveDynamic(maxBack1*sin((M_PI/180)*angle_to_turn_at),angle_to_turn_at,SPEED_LOW,FALSE);
                }else{
                    translateMoveDynamic((current_distance - distance),angle_to_turn_at,SPEED_LOW,FALSE);
                }
                distanceSensor(SIDE_RIGHT);
                current_distance = sensor_distances[SIDE_RIGHT];
            }
        }else if (less_or_great == GREATER_THAN){
            distanceSensor(SIDE_LEFT);
            current_distance = sensor_distances[SIDE_LEFT];
            while (current_distance < distance){
                if ((distance - current_distance) > maxBack1*sin((M_PI/180)*angle_to_turn_at)){
                    translateMoveDynamic(maxBack1*sin((M_PI/180)*angle_to_turn_at),angle_to_turn_at,SPEED_LOW,FALSE);
                }else{
                    translateMoveDynamic((distance - current_distance),angle_to_turn_at,SPEED_LOW,FALSE);
                }
                distanceSensor(SIDE_LEFT);
                current_distance = sensor_distances[SIDE_LEFT];
            }
        }
    }else if (left_or_right == LEFT){ 
        if (less_or_great == LESS_THAN) {
            distanceSensor(SIDE_LEFT); 
            current_distance = sensor_distances[SIDE_LEFT];
            while (current_distance > distance){
                if ((current_distance - distance) > maxBack1*sin((M_PI/180)*angle_to_turn_at)){
                    translateMoveDynamic(-maxBack1*sin((M_PI/180)*angle_to_turn_at),angle_to_turn_at,SPEED_LOW,FALSE);
                }else{
                    translateMoveDynamic(-(current_distance - distance),angle_to_turn_at,SPEED_LOW,FALSE);
                }
                distanceSensor(SIDE_LEFT);
                current_distance = sensor_distances[SIDE_LEFT];
            }
        }else if (less_or_great == GREATER_THAN){
            distanceSensor(SIDE_RIGHT);
            current_distance = sensor_distances[SIDE_RIGHT];
            while (current_distance < distance){
                if ((distance - current_distance) > maxBack1*sin((M_PI/180)*angle_to_turn_at)){
                    translateMoveDynamic(-maxBack1*sin((M_PI/180)*angle_to_turn_at),angle_to_turn_at,SPEED_LOW,FALSE);
                }else{
                    translateMoveDynamic(-(current_distance - distance),angle_to_turn_at,SPEED_LOW,FALSE);
                }
                distanceSensor(SIDE_RIGHT);
                current_distance = sensor_distances[SIDE_RIGHT];
            }
        }
    }
}

int failsafe(int direction) {
    int check_distance;
    
    // SAFETY OVERRIDE:
    if(safety_override == TRUE){
        return FALSE;
    }
    
    // detecting direction:    
    if (direction == BACKWARD) {
        distanceSensor(BACK_RIGHT);
        CyDelay(50);
        
        check_distance = sensor_distances[BACK_RIGHT];   // checks the distance measured by the ultrasonic
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
        
        check_distance = sensor_distances[FRONT_LEFT];   // checks the distance measured by the ultrasonic
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
    
    if (sensor_distances[side_sensing] > initial_value + drift_tolerance 
                || sensor_distances[side_sensing] < initial_value - drift_tolerance ){
        // If it has drifted more than 10mm either side from the initial value, we perform a straight adjust
        //translateUntil(initial_value, translate_direction, GREATER_THAN,side_sensing,SPEED);   // will take us back to our initial position
                                                                                // if this got tripped by a block, it would mess everything up
        //translateMoveDynamic(translate_direction, 30, SPEED, FALSE);          // these are not the correct values 
        straightAdjust(FRONT_SENSORS);                                          // straight adjust to correct any problems from translating
        
    }
}

void straightAdjust(int front_back) {
       
    int sensor_left; 
    int sensor_right;
    int difference = 300;
    int tolerance = 2;
    int max_difference = 800;
    int ultra_delay = 50;
    int speed = 25;
    int timer;
    Timer_straight_adjust_Start();      // Starts counting the timer 
    
    // Are we adjusting with the back or front sensors: 
    if (front_back == FRONT_SENSORS) {
        sensor_left = FRONT_LEFT;
        sensor_right = FRONT_RIGHT;
    }
    else {
        sensor_left = BACK_RIGHT;       // THEY ARE SWAPPED AROUND AS A FIX 
        sensor_right = BACK_LEFT;
    }
          
    // Ensure the initial readings are accurate: 
    while (abs(difference) > max_difference) {              
        distanceSensor(sensor_left);
        CyDelay(50);                        // 50ms might not be enough ???
        sprintf(output, "%d \t", sensor_distances[sensor_left]);
        UART_1_PutString(output);
        
        distanceSensor(sensor_right);
        CyDelay(50);
        sprintf(output, "%d \t", sensor_distances[sensor_right]);
        UART_1_PutString(output);
        
        difference = sensor_distances[sensor_left] - sensor_distances[sensor_right];
        sprintf(output, "difference = %d, \n", difference);       
        UART_1_PutString(output);
    }
        
    
    // Adjust against the wall: 
    int speed_left = speed;        // slow speed
    int speed_right = speed;
    
    
    Motor_Left_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(speed_left);
    Motor_Right_Driver_Wakeup();
    Motor_Right_Driver_WriteCompare(speed_right);
        
    while(abs(difference) > tolerance ) {
        
        if (difference > 0)             // This means we need to move it right
        {
            Motor_Left_Control_Write(0); Motor_Right_Control_Write(1); 
             
            while (abs(difference) > tolerance 
                && difference > 0
                && abs(difference) < max_difference)     // ensures working correctly
            {

            distanceSensor(sensor_left);
            CyDelay(ultra_delay);
            distanceSensor(sensor_right);
            CyDelay(ultra_delay);
                      
            difference = sensor_distances[sensor_left] - sensor_distances[sensor_right];
            }
           
        }
        else if (difference < 0 ){      // HERE we are turning left
            Motor_Left_Control_Write(1); Motor_Right_Control_Write(0);
            
            distanceSensor(sensor_left);
            CyDelay(ultra_delay);
            distanceSensor(sensor_right);
            CyDelay(ultra_delay);
                      
            difference = sensor_distances[sensor_left] - sensor_distances[sensor_right];
                       
            //sprintf(output, " %d , %d \t dif: %d, \n", sensor_distances[FRONT_LEFT], 
            //                                            sensor_distances[FRONT_RIGHT],
            //                                                                        difference);       
            //UART_1_PutString(output);
            
            while (abs(difference) > tolerance 
                        && difference < 0
                        && abs(difference) < max_difference) {
                distanceSensor(sensor_left);
                CyDelay(ultra_delay);
                distanceSensor(sensor_right);
                CyDelay(ultra_delay);
                          
                difference = sensor_distances[sensor_left] - sensor_distances[sensor_right];
                           
                sprintf(output, " %d , %d \t dif: %d, \n", sensor_distances[sensor_left], 
                                                            sensor_distances[sensor_right],
                                                                                        difference);       
                UART_1_PutString(output);
            
            }
            
        }
        
        // Comes out of the while loop if the max_difference is exceeded, but then the values might be wrong
        // if max_difference is exceeded, it should probably be forced to swivel the other way to 
        // that it was swivelling        
        
        
        // CHECKS one more time to ensure the difference is accurate: 
            // consider putting in a for loop to get new readings
        distanceSensor(sensor_left);
        CyDelay(ultra_delay);
        distanceSensor(sensor_right);
        CyDelay(ultra_delay);
                      
        difference = sensor_distances[sensor_left] - sensor_distances[sensor_right];
        sprintf(output, "%d, %d, ", sensor_distances[sensor_left], sensor_distances[sensor_right]);       
        UART_1_PutString(output);
        sprintf(output, "dif: %d, \n", difference);       
        UART_1_PutString(output);
        
        // Timer out failsafe: 
        

//        timer = Timer_straight_adjust_ReadCounter();
//        
//        if (timer < 35000) {
//            Motor_Left_Driver_Sleep();                  // puts it back to sleep
//            Motor_Right_Driver_Sleep();
//            moveDynamic(-30,SPEED,TRUE);                // calls movedynamic
//            moveDynamic(30, SPEED,TRUE);      
//            Motor_Left_Driver_Wakeup();
//            Motor_Left_Driver_WriteCompare(speed_left); // brings it back to continue the while loop
//            Motor_Right_Driver_Wakeup();
//            Motor_Right_Driver_WriteCompare(speed_right);
//            Control_Reset_Straight_Write(1);            // Resets the timer (I think)
//            CyDelay(10); 
//            Control_Reset_Straight_Write(0);
//            
//            sprintf(output, "timer count: %d", timer);
//            UART_1_PutString(output);
//        }
        
       
        
        
        
             // This ensures that the turning worked correctly
                                            // changed it from 10 to 2, this might change the way it works
    }
}
        
        

void straightAdjustBlayke(int front_back){
    int sensor_left; 
    int sensor_right;
    int difference = 300;
    int tolerance = 2;
    int max_difference = 800;
    int ultra_delay = 50;
    int speed = 25;
    
    // Are we adjusting with the back or front sensors: 
    if (front_back == FRONT_SENSORS) {
        sensor_left = FRONT_LEFT;
        sensor_right = FRONT_RIGHT;
    }
    else {
        sensor_left = BACK_RIGHT; // Mirrored from perspective of on the middle of robot looking outwards
        sensor_right = BACK_LEFT;
    }
    
    // Initialise
    distanceSensor(sensor_left);
    distanceSensor(sensor_right);
    difference = sensor_distances[sensor_left] - sensor_distances[sensor_right];

    
    // Adjust against the wall: 
    int speed_left = speed;        // slow speed
    int speed_right = speed;
    
    
    Motor_Left_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(speed_left);
    Motor_Right_Driver_Wakeup();
    Motor_Right_Driver_WriteCompare(speed_right);
        
    while(abs(difference) > tolerance ) {
        
        if (difference > 0)             // This means we need to move it right
        {
            Motor_Left_Control_Write(0); Motor_Right_Control_Write(1); 
             
            while (abs(difference) > tolerance 
                && difference > 0)     // ensures working correctly
            {

            distanceSensor(sensor_left);
            CyDelay(ultra_delay);
            distanceSensor(sensor_right);
            CyDelay(ultra_delay);
                      
            difference = sensor_distances[sensor_left] - sensor_distances[sensor_right];
            }
           
        }
        else if (difference < 0 ){      // HERE we are turning left
            Motor_Left_Control_Write(1); Motor_Right_Control_Write(0);
            
            distanceSensor(sensor_left);
            CyDelay(ultra_delay);
            distanceSensor(sensor_right);
            CyDelay(ultra_delay);
                      
            difference = sensor_distances[sensor_left] - sensor_distances[sensor_right];

        }
        
        // Comes out of the while loop if the max_difference is exceeded, but then the values might be wrong
        // if max_difference is exceeded, it should probably be forced to swivel the other way to 
        // that it was swivelling        
        
        
        // CHECKS one more time to ensure the difference is accurate: 
            // consider putting in a for loop to get new readings
        distanceSensor(sensor_left);
        CyDelay(ultra_delay);
        distanceSensor(sensor_right);
        CyDelay(ultra_delay);
                      
        difference = sensor_distances[sensor_left] - sensor_distances[sensor_right];
        sprintf(output, "%d, %d, ", sensor_distances[sensor_left], sensor_distances[sensor_right]);       
        UART_1_PutString(output);
        sprintf(output, "dif: %d, \n", difference);       
        UART_1_PutString(output);
    }
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
        
    distanceSensor(BACK_RIGHT);
    CyDelay(DELAY);
    
    Motor_Left_Driver_Wakeup();
    Motor_Right_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(25);
    Motor_Right_Driver_WriteCompare(25);
    
    distance_check = sensor_distances[BACK_RIGHT];
    minimum_distance = distance_check;
    
    while(1) {
        if (direction == LEFT) { Motor_Left_Control_Write(0); Motor_Right_Control_Write(1); }
        else {Motor_Left_Control_Write(1); Motor_Right_Control_Write(0); }
        
        while (distance_previous > distance_check) {
            // this will run while:
                // new distance values are getting smaller           
            
            // Updating the distance values: 
            distance_previous = distance_check;
            distanceSensor(BACK_RIGHT);
            CyDelay(DELAY);
            distance_check = sensor_distances[BACK_RIGHT];
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
    distanceSensor(BACK_RIGHT);
    CyDelay(60);
    
    while (sensor_distances[FRONT_LEFT] + sensor_distances[BACK_RIGHT] + tolerance < ARENA_WIDTH 
           || sensor_distances[FRONT_LEFT] + sensor_distances[BACK_RIGHT] + tolerance < ARENA_WIDTH) { 
            // this checks if the ultrasonic distances being recorded are accurate
            // if it is not within this threshold, there must be something up.  
                
    distanceSensor(FRONT_LEFT);
    CyDelay(60);
    distanceSensor(FRONT_LEFT);
    CyDelay(60);
    distanceSensor(BACK_RIGHT);
    CyDelay(60);
        
    }
    
}

void locatePucks(void)
{
    
    
    
    
}

void moveForwardThenBackward(int distance, int less_great, int sensor, int speed, int activate_safety){
   moveUntil(distance, FORWARD, less_great, sensor, speed ,activate_safety); 
   moveUntil(distance, BACKWARD, -1*less_great, sensor, speed ,activate_safety);     
}