 
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


extern char output[32];   

// * STATE VARIABLES * //
int state = STATE_PRE_RUN;
float block_edge_location[4] = {0,0,0,0}; // N E S W edge positions respectively

// * PUCK CONSTRUCTION VARIABLES * //
int current_puck_stack_size = 0; // How many pucks are currently on the construction plate
int puck_rack_scans[5] = {BLANK,BLANK,BLANK,BLANK,BLANK}; // 5 slots in puck rack.
int puck_construction_plan[3] = {BLANK,BLANK,BLANK};
int puck_rack_west_offsets[5] = {PUCK_RACK_0_WEST_DISTANCE,
                                PUCK_RACK_1_WEST_DISTANCE,
                                PUCK_RACK_2_WEST_DISTANCE,
                                PUCK_RACK_3_WEST_DISTANCE,
                                PUCK_RACK_4_WEST_DISTANCE };

int puck_temp_pile[2] = {BLANK,BLANK}; // Index 0 refers to east home base edge, 1 refers to west home base edge

// * NAVIGATION AND POSITION VARIABLES * //

int begin_navigation = 0; // Allow us to break out of the intial phase when powered up
int block_location[4] = {0,0,0,0};      // The block location values
                                        //#define NORTH 0
                                        //#define EAST 1
                                        //#define SOUTH 2
                                        //#define WEST 3
int puck_location[4] = {0,0,0,0};       // The block location values
                                        //#define NORTH 0
                                        //#define EAST 1
                                        //#define SOUTH 2
                                        //#define WEST 3

int safety_override = FALSE;            

// These block clearance variables are only true if we have enough room either side of the block to fully fit the robot through within a safety tolerance
// And only true for the puck clearance variables if there is enough room to be able to turn at the far wall to face the pucks.
short int block_east_clearance = 0;
short int block_west_clearance = 0;
short int puck_east_clearance = 0;
short int puck_west_clearance = 0;

// * COLOUR VARIABLES * //

int idac_value = 0;  
int colour_flag = 1;                    // sets which photodiode to use 
int colour_sensing_algorithm = 0;      // Determines which colour sensing algorithm to use:
                                                // 0: Wall algorithm
                                            // 1: Claw algorithm
                                                // 2: old algorithm

int moving = 0; // Temp to stop while loop from repeated runs of scanning plan code

const int PLAN_SCAN_VERTICAL = 10; // Set to ultrasonic distance at the home base 

int current_stage = 1;      // There are 3 stages, we start at 1


// GLOBAL VARIABLE
int distance_returned_global;


// * INTERRUPT HANDLING * // 
CY_ISR(TIH)                             // Ultrasonic ISR Definition
{
    ultrasonicInterruptHandler();
}

CY_ISR(StartIH)                             // Ultrasonic ISR Definition
{
    CyDelay(100);
    if (begin_navigation){
        UART_1_PutString("Beginning Navigation \n");
        //state = STATE_SCAN_PLAN; // Put to this for competition
        state = STATE_LOCATE_BLOCK_AND_PUCKS;
        //state = STATE_FIND_REQUIRED_PUCK;
        //state = STATE_DEPOSIT_PUCK;
        //state = STATE_PARK_HOME;
        //preferential_route = WEST;
        
    }
}

CY_ISR(SAIH)                             // 
{
    // Set a global variable 
    Timer_straight_adjust_ReadStatusRegister();   // CLEARS INTERRUPTS 
    Motor_Left_Driver_Sleep();                  // puts it back to sleep
    Motor_Right_Driver_Sleep();
    moveDynamic(-20,SPEED,TRUE);                // calls movedynamic
    moveDynamic(20, SPEED,TRUE); 
    Control_Reset_Straight_Write(1);
    CyDelay(10);
    Control_Reset_Straight_Write(0);
    Motor_Left_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(SPEED); // brings it back to continue the while loop
    Motor_Right_Driver_Wakeup();
    Motor_Right_Driver_WriteCompare(SPEED);
}
 

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    
    TIA_1_Start();
    UART_1_Start();
    UART_1_PutString("UART commencing... \n");
    ADC_Start();
    IDAC8_1_Start();
    
    // DEBUGGING:
        // Moved to debugging.c file, set it there
    
    
    // IDAC initialisation: 
    IDAC8_1_SetRange(IDAC8_1_RANGE_32uA);       // Sets the range between 0 and 32uA
    IDAC8_1_SetValue(idac_value);               // set a value between 0 and 255
    
    // Colour Sensing Initialisation & Debugging:
    colour_sensing_algorithm = 0;
    control_photodiode_Write(colour_sensing_algorithm);            // controls which photodiode is being used: 
                                                // 0: wall
                                                // 1: claw
    control_led_Write(1);   CyDelay(500);  // Ensures all the RGBs are working
    control_led_Write(2);   CyDelay(500);
    control_led_Write(3);   CyDelay(500);
    control_led_Write(0);   CyDelay(500);
    colourSensingInitialise();                  // Initialises the colour sensor
    

    //Servos for the arm instatination
    Rack_Servo_PWM_Start();
    Rack_Servo_PWM_WriteCompare(1000);          // BRINGs rack to highest position, so not to mess with ultrasonics
    CyDelay(500);
    Rack_Servo_PWM_Sleep();
    
    Gripper_Servo_PWM_Start();
    Gripper_Servo_PWM_WriteCompare(1120);       //This is the open gripper position
    CyDelay(500);
    Gripper_Servo_PWM_Sleep();
    
    // Motors Initialisation:
    Motor_Left_Driver_Start();
    Motor_Right_Driver_Start();
    Motor_Left_Driver_WriteCompare(0);          // sets the motors to 0 so it will be smooth next time they wake up
    Motor_Right_Driver_WriteCompare(0);
    Motor_Left_Driver_Sleep();                  // Puts motor to sleep
    Motor_Right_Driver_Sleep();                 // Puts motor to sleep
    
    Motor_Left_Decoder_Start();
    Motor_Right_Decoder_Start();
    
    internal_orientation = WEST;                // robot initial starts in the West direction
    //internal_orientation = SOUTH;                
    
    // Timer and ISR instantiation
    Timer_1_Start();                
    Timer_1_ReadStatusRegister();
    Sonic_StartEx(TIH);
    Start_StartEx(StartIH);
    Straight_StartEx(SAIH);
    begin_navigation = 1; // Set to true after it has run ISR at least once
    
    // Ultrasonic Initialisation & Calibration:
    // The ultrasonics take several test measurements to ensure they are working
        
    UART_1_PutString("Ultrasonic Calibration tests: \n");
    safetyDistanceCheck();
    safetyDistanceCheck();
    safetyDistanceCheck();
    
    // Straight adjust timer: 
    Timer_straight_adjust_Start();              
    Timer_straight_adjust_ReadStatusRegister();
    //Control_Reset_Straight_Write(0);
    
    // FORCING STATE:
    // Manual state set for testing
    state = STATE_PRE_RUN;
    puck_construction_plan[0] = RED;
    puck_construction_plan[1] = GREEN;
    puck_construction_plan[2] = BLUE;
    

    // Main Loop for States
    
    //while(1){}
    
    for(;;)
    {

        if (state == STATE_PRE_RUN){
        
        }

        while(0){
            distanceSensor(SIDE_LEFT);
            CyDelay(60);
            sprintf(output, "%d \t", sensor_distances[SIDE_LEFT]);
            UART_1_PutString(output);
        }

// *** 1. STATE SCAN PLAN: *** // 
//
        
      
        if (state == STATE_SCAN_PLAN) {              // colour sensing, while switch has not been pushed. change to if eventually


            // Debugging: 

            ultimateDebugging();        // If any of the debugging flags are activated, this will be called over the main code
            

            straightAdjust(FRONT_SENSORS);
            moveUntil(-100, BACKWARD, LESS_THAN, BACK_RIGHT, SPEED, TRUE);
            moveUntil(400, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
            moveDynamic(-450, SPEED, TRUE);
            CyDelay(500);   
            colourSensingInitialise();      // Initialises wall colour sensor against the black wall 
            CyDelay(500);
            moveDynamic(170, SPEED, TRUE);
            CyDelay(500);

            for (int i = 0; i < 5; i++) {                       // scan each of the pucks 
                puck_rack_scans[i] = colourSensingOutput();
                CyDelay(500);
                moveDynamic(61, SPEED, TRUE);
                blinkLED(puck_rack_scans[i],500);   // Will show us which coloured puck it thinks it scanned
            }
            
            //straightAdjust(FRONT_SENSORS);
            //moveUntil(-100, BACKWARD, LESS_THAN, BACK, SPEED, TRUE);
            //colourSensingInitialise();      // Initialises wall colour sensor against the black wall 
                          
            UART_1_PutString("Found Colours: \n");
            for (int i = 0; i < 5; i++) 
            {
                sprintf(output, "%i \n", puck_rack_scans[i]);    // updates UART showing values of scanned pucks    
                UART_1_PutString(output);
            }
            
            
            control_photodiode_Write(CLAW_SENSING);    // changes to claw photodiode
            colour_sensing_algorithm = CLAW_1_ALGORITHM;   // changes it to claw algorithm
                                 
            // THEN WE FILTER THE NO PUCK VALUES:
            int puck_construction_plan_index = 0; // Will be used to iterate through the 3 puck colours in construction plan
            
            // Iterate over all 5 rack slots and place the 3 colours into puck_construction_plan.
            for (int puck_rack_index = 0; puck_rack_index < 5; puck_rack_index++){
                if (puck_rack_scans[puck_rack_index] != BLANK && puck_construction_plan_index != 3){
                    puck_construction_plan[puck_construction_plan_index] = puck_rack_scans[puck_rack_index];
                    puck_construction_plan_index++;
                }
            }
                    
            // MOVE into the next state: 
            state = STATE_LOCATE_BLOCK_AND_PUCKS;

        }

//
// *** 2. STATE LOCATE BLOCKS AND PUCKS: *** // 
//
        
    	if (state == STATE_LOCATE_BLOCK_AND_PUCKS){
            
            ultimateDebugging();   

            // move away from home base:
            
            translateMoveDynamic(45, -35, 100, FALSE);
            //straightAdjust(FRONT_SENSORS);
    
            // Move until construction zone            
            moveUntil(130, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);  // Move to west wall
            straightAdjust(FRONT_SENSORS);                                      // straighten against west wall
            
            
            distanceSensor(SIDE_LEFT);  // takes how far we are away from home base wall
            CyDelay(50);
            while (sensor_distances[SIDE_LEFT] > (BLOCK_ZONE_SOUTH - WIDTH_SENSOR_TO_SENSOR)) {
                // stays in while loop until side left value taken is appropriate
                // E.g. the side left value should be less than BLOCK_ZONE_SOUTH - WIDTH_SENSOR_TO_SENSOR
                // So, it should be less than 500mm - 220mm = 280mm
                distanceSensor(SIDE_LEFT);
                CyDelay(50);
                } 
            
            int block_threshold = BLOCK_ZONE_NORTH - sensor_distances[SIDE_LEFT] - WIDTH_SENSOR_TO_SENSOR + (BLOCK_WIDTH/2);  
            
            
            // Check if we FIND the block: 
            
            moveUntil(block_threshold, BACKWARD, LESS_THAN, SIDE_RIGHT, SPEED, TRUE);  
                // We have hit the west side of the block
                //UART_1_PutString("BLOCK FIRST");
                
                blinkLED(GREEN,1000);   
                
                // Find the WEST clearance of the block with ultrasonics: 
                distanceSensor(FRONT_LEFT); 
                CyDelay(DELAY);
                
                block_location[WEST] = sensor_distances[FRONT_LEFT] + DISTANCE_FRONT_SENSOR_TO_SIDE_SENSOR;
                sprintf(output, "west block front sensor: %d, \n", sensor_distances[FRONT_LEFT] );       
                UART_1_PutString(output);
                
                // OR find the west clearance of the block with encoders: 
                //block_location[WEST] = 130 + global_distance + DISTANCE_WHEEL_TO_SENSOR;    //130 was initial starting position 
                
                // Stopping at the East side of the block:
                moveDynamic(-BLOCK_WIDTH/2,SPEED,TRUE);
                moveUntil(block_threshold, BACKWARD, GREATER_THAN, SIDE_RIGHT, SPEED, TRUE);    // this will move backwards until no longer hitting the block, or the wall
                blinkLED(GREEN,1000);                                                           // To show it ended at the correct spot
                
                distanceSensor(BACK_RIGHT);
                CyDelay(50);
                block_location[EAST] = sensor_distances[BACK_RIGHT] + DISTANCE_BACK_SENSOR_TO_SIDE_SENSOR;
                

                // determining if the block is facing east or west and then offsetting that value: 
                /*
                if (global_distance < (BLOCK_LENGTH/2)) {
                    // block is facing this way "|"
                    block_location[EAST] = ARENA_WIDTH - block_location[WEST] - BLOCK_WIDTH;
                }
                else {
                    // block is facing this way "---"
                    block_location[EAST] = ARENA_WIDTH - block_location[WEST] - BLOCK_LENGTH;
                    
                }

                */
                
                sprintf(output, "east block front sensor: %d, \n", sensor_distances[FRONT_LEFT] );       
                UART_1_PutString(output);

 
                
            // brings us to the east side WITH SAFETY margins: 
            moveDynamic(-100,SPEED,TRUE);
            changeOrientation(EAST,SPEED);
            moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT + SAFETY_MARGIN/2, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
            changeOrientation(SOUTH, SPEED);
            moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT + SAFETY_MARGIN/2, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);
            changeOrientation(EAST, SPEED);
            
            
            
            // DETERMINES THE CLEARANCES AVAILABLE FOR OUR ROBOT
            if (block_location[EAST] > WIDTH_WHEEL_TO_WHEEL + BLOCK_TOLERANCE + 100) {
                // the east clearance of block check
              block_east_clearance = 1;  
            }
            if (block_location[WEST] > WIDTH_WHEEL_TO_WHEEL + BLOCK_TOLERANCE + 100) {
                // the east clearance of block check
              block_west_clearance = 1;  
            }

            
            // DETERMINES WHICH PATH TO TAKE: 
            if (block_location[EAST] > block_location[WEST]) {
                preferential_route = EAST;
            }
            else {
                preferential_route = WEST;
            }
            // Middle route:
                // preferential_route = MIDDLE; 
            
                      
            
            
            sprintf(output, "west puck clearance: %d, \n", puck_west_clearance);       
            UART_1_PutString(output);
            sprintf(output, "east puck clearance: %d, \n\n", puck_east_clearance);       
            UART_1_PutString(output);
            
            sprintf(output, "west block clearance: %d, \n", block_west_clearance);       
            UART_1_PutString(output);
            sprintf(output, "east block clearance: %d, \n", block_east_clearance);       
            UART_1_PutString(output);
            
            sprintf(output, "west block distance: %d, \n", block_location[WEST]);       
            UART_1_PutString(output);
            sprintf(output, "east block distance: %d, \n", block_location[EAST]);       
            UART_1_PutString(output);
            
            
            
            
            
           // while(1) {} // just want to do the block and puck finding
            
            //changeOrientation(WEST,SPEED);
            //moveDynamic(-600, SPEED, TRUE);
            //moveUntil((puck_location[EAST]/2),FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
            //straightAdjust();
            //changeOrientation(WEST,SPEED);
            //moveUntil((PUCK_GRID_FROM_NORTH/2), FORWARD, LESS_THAN, FRONT_LEFT, SPEED,TRUE);
            //straightAdjust();
            //changeOrientation(EAST, SPEED);
            ///oveDynamic(1200, SPEED, TRUE);
            

            
            /*
            moveUntil(200, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);
            changeOrientation(SOUTH,SPEED);
            changeOrientation(EAST, SPEED);
            straightAdjust(FRONT_SENSORS);
            moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT + SAFETY_MARGIN/2, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
            translateUntil(CLEARANCE_RADIUS_CENTER_TO_BACK - SAFETY_MARGIN, 200, LEFT, GREATER_THAN);
            straightAdjust(FRONT_SENSORS);
            */
            
            // PUCK has now been found, enter the IF statements to locate and pick up pucks 
            state = STATE_GO_TO_PUCKS;
	    }     

//
// *** 3. STATE GO TO PUCKS: *** // 
//

        // Ensure that we are @ east wall facing east at a minimum verticaldistance so we can turn left without hitting bottom wall
        
        if (state == STATE_GO_TO_PUCKS){
            
            // The different route finding Algorithms for the different combinations: 
            
    // EAST ROUTE: 
            while (preferential_route == EAST) {
                UART_1_PutString("Taking east route");
                //moveUntil(WALL_CLEARANCE_FRONT, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);
                moveForwardThenBackward(150,LESS_THAN,FRONT_MIDDLE,SPEED_LOW,TRUE);
                straightAdjust(FRONT_SENSORS);
                //changeOrientation(SOUTH, SPEED);        // this should clear us of the wall
                //moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT + SAFETY_MARGIN/2, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);
                //changeOrientation(EAST, SPEED);
                changeOrientation(NORTH, SPEED);
                //straightAdjust(FRONT_SENSORS);    
                
                distanceSensor(BACK_LEFT);          // checking initial distance from the wall
                CyDelay(50);
                initial_tolerance = sensor_distances[BACK_LEFT];
                
                //moveUntil(PUCK_GRID_FROM_NORTH + DISTANCE_FRONT_SENSOR_FROM_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * current_puck_stack_size, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                        // move until corner wall
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                
                
                sprintf(output, "initial tolerance: %d\n", initial_tolerance);      
                UART_1_PutString(output); 
                
                sprintf(output, "global distance: %d\n", global_distance);      
                UART_1_PutString(output); 
                
                if (global_distance < ARENA_WIDTH - PUCK_GRID_FROM_NORTH - DISTANCE_BACK_SENSOR_FROM_CENTER - initial_tolerance - 100) {
                    // Has hit the pucks and must try the west route
                    // move until backwards to original starting position 
                    
                    moveUntil(initial_tolerance, BACKWARD, LESS_THAN, BACK_LEFT, SPEED, TRUE);  // moveuntil backwards to original position 
                    moveForwardThenBackward(initial_tolerance,GREATER_THAN,BACK_LEFT,SPEED_LOW,TRUE);
                    changeOrientation(EAST, SPEED);
                    if (block_west_clearance == 1) { preferential_route = WEST; } else {preferential_route = MIDDLE;}   // checks which route to swap  to 
                    break;  // breaks out of the route to change routes 
                }
                else {
                    // have moved into the corner, checking if enough tolerance in corner 
                    distanceSensor(SIDE_LEFT); 
                    CyDelay(50);
                    
                    if (sensor_distances[SIDE_LEFT] > CLEARANCE_RADIUS_CENTER_TO_BACK - (SIDE_SENSORS_WIDTH/2)) {
    // continue with the route MAIN CODE FOR ROUTE
                        straightAdjust(FRONT_SENSORS);
                        changeOrientation(WEST, SPEED);
                        straightAdjust(BACK_SENSORS);
        
                        if (current_stage == 1) {             
                            moveUntil(DISTANCE_MIDDLE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                            distanceSensor(BACK_LEFT);
                            CyDelay(50);
                            puck_grid_initial = sensor_distances[BACK_LEFT];
                        }
                        else {
                            moveUntil(puck_grid_initial, FORWARD, GREATER_THAN, BACK_LEFT, SPEED, TRUE);
                        }
                        
                        changeOrientation(NORTH, SPEED_LOW);
                        moveForwardThenBackward(255 - 52 * current_puck_stack_size, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE);
                        straightAdjust(FRONT_SENSORS);
                        changeOrientation(WEST, SPEED_LOW);
                        
                        state = STATE_FIND_REQUIRED_PUCK;       // CHANGE STATE
                        
                        break;
                    }   
                    else {
                        // Not enough distance to continue this route
                        moveUntil(initial_tolerance, BACKWARD, LESS_THAN, BACK_LEFT, SPEED, TRUE);  // moveuntil backwards to original position 
                        changeOrientation(EAST, SPEED);
                        if (block_west_clearance == 1) { preferential_route = WEST; } else {preferential_route = MIDDLE;}   // checks which route to swap  to 
                        break;  
                    }
                }
            }
            
    // WEST ROUTE: 
            while (preferential_route == WEST) {
                UART_1_PutString("Taking west route");
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK , BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);
                straightAdjust(FRONT_SENSORS); 
                changeOrientation(NORTH, SPEED);        // FASSE SOUTH TO ADJUST
                changeOrientation(WEST, SPEED);
                straightAdjust(BACK_SENSORS);           // fine up toll here
                moveUntil(150, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                changeOrientation(SOUTH, SPEED);       // about to BACK UP INTO WEST CORNER
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);
                changeOrientation(WEST, SPEED);
                changeOrientation(NORTH, SPEED);
                straightAdjust(BACK_SENSORS); 
                
                distanceSensor(BACK_LEFT);          // checking initial distance from the wall
                CyDelay(50);
                initial_tolerance = sensor_distances[BACK_LEFT];

                //moveUntil(PUCK_GRID_FROM_NORTH + DISTANCE_FRONT_SENSOR_FROM_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * current_puck_stack_size, FORWARD, LESS_THAN, FRONT_RIGHT, SPEED, TRUE);
                moveUntil(150, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                
                
                sprintf(output, "initial tolerance: %d\n", initial_tolerance);      
                UART_1_PutString(output); 
                
                sprintf(output, "global distance: %d\n", global_distance);      
                UART_1_PutString(output); 
                
                
                // move until corner wall
                if (global_distance < ARENA_WIDTH - PUCK_GRID_FROM_NORTH - DISTANCE_BACK_SENSOR_FROM_CENTER - initial_tolerance - 100) {
                    // Has hit the pucks and must try the west route
                    moveUntil(initial_tolerance, BACKWARD, LESS_THAN, BACK_LEFT, SPEED, TRUE);  // moveuntil backwards to original position 
                    changeOrientation(WEST, SPEED);
                    moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK,BACKWARD,GREATER_THAN,FRONT_LEFT,SPEED,TRUE);   
                    changeOrientation(NORTH, SPEED);
                    changeOrientation(EAST, SPEED);
                    moveUntil(150,FORWARD,LESS_THAN,FRONT_LEFT,SPEED,TRUE);   // bring us back to original place
                    if (block_east_clearance == 1) { preferential_route = EAST; } else {preferential_route = MIDDLE;}   // checks which route to swap  to 
                    break;  // breaks out of the route to change routes 
                }
                else {
                    // have moved into the corner, checking if enough tolerance in corner 
                    distanceSensor(SIDE_RIGHT); 
                    CyDelay(50);
                    if (sensor_distances[SIDE_RIGHT] > CLEARANCE_RADIUS_CENTER_TO_BACK - (SIDE_SENSORS_WIDTH/2))  {
    // continue with the route MAIN CODE FOR ROUTE
                        straightAdjust(FRONT_SENSORS);
                        changeOrientation(WEST, SPEED);
                        moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);
                        changeOrientation(NORTH, SPEED);
                        changeOrientation(EAST, SPEED);
                        straightAdjust(BACK_SENSORS);

                        if (current_stage == 1) {             
                            moveUntil(DISTANCE_MIDDLE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                            distanceSensor(BACK_LEFT);
                            CyDelay(50);
                            puck_grid_initial = sensor_distances[BACK_LEFT];
                        }
                        else {
                            moveUntil(puck_grid_initial, FORWARD, GREATER_THAN, BACK_LEFT, SPEED, TRUE);
                        }
                        changeOrientation(NORTH, SPEED_LOW);
                        moveForwardThenBackward(255 - 52* current_puck_stack_size, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE);
                        straightAdjust(FRONT_SENSORS);
                        changeOrientation(EAST, SPEED_LOW);       
                        
                        state = STATE_FIND_REQUIRED_PUCK;       // CHANGE STATE
                        
                        break;
                    }   
                    else {
                        // Not enough distance to continue this route
                        
                        moveUntil(initial_tolerance, BACKWARD, LESS_THAN, BACK_LEFT, SPEED, TRUE);  // moveuntil backwards to original position 
                        changeOrientation(EAST, SPEED);
                        moveUntil(150,FORWARD,LESS_THAN,FRONT_MIDDLE,SPEED,TRUE);   // bring us back to original place

                        if (block_east_clearance == 1) { preferential_route = EAST; } else {preferential_route = MIDDLE;}   // checks which route to swap  to 
                        break;
                    }
                }
                
                
            }
            
            
    // MIDDLE ROUTE:         
            if (preferential_route == MIDDLE) {
                UART_1_PutString("entering middle route");
                // check whether east or west clearence is clear, if thats the case, hug the side of that block
                    // if EAST clearance is clear, we hug east side of block, and move towards east for the pucks
                    // if WEST clearance is clear, we hug the west side of the block, and move towards east for the pucks 
                if (block_east_clearance == 1) {
                    moveUntil(ARENA_LENGTH - block_location[EAST] , BACKWARD, LESS_THAN, BACK_RIGHT, SPEED, TRUE);
                    straightAdjust(BACK_SENSORS); //? Remove if it fucks things up
                    changeOrientation(NORTH, SPEED);
                    straightAdjust(BACK_SENSORS); //? Remove if it fucks things up
                    moveUntil(255 - 52* current_puck_stack_size, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                    //translateMoveDynamic(CLEARANCE_RADIUS_CENTER_TO_BACK, 10, SPEED, FALSE); // Change safety to TRUE when implemented
                    straightAdjust(FRONT_SENSORS);
                    changeOrientation(EAST, SPEED);
                    straightAdjust(BACK_SENSORS);

                    moveUntil(DISTANCE_MIDDLE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);   
                    changeOrientation(NORTH, SPEED_LOW);
                    moveForwardThenBackward(255 - 52* current_puck_stack_size, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE);
                    straightAdjust(FRONT_SENSORS);
                    changeOrientation(EAST, SPEED_LOW);
                    
                }
                
                
                else {
                    moveUntil(ARENA_WIDTH - block_location[WEST] + 50, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);
                    straightAdjust(FRONT_SENSORS);
                    changeOrientation(NORTH, SPEED);    // gets it ready for going aslong the wall 
                    //straightAdjust();
                    //moveUntil(PUCK_GRID_FROM_NORTH + DISTANCE_MIDDLE_SENSOR_TO_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * current_puck_stack_size, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                    moveUntil(255 - 52* current_puck_stack_size, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                    straightAdjust(FRONT_SENSORS);
                    changeOrientation(WEST, SPEED);
                    straightAdjust(BACK_SENSORS);

                    
                    moveUntil(DISTANCE_MIDDLE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                    changeOrientation(NORTH, SPEED_LOW);
                    moveForwardThenBackward(255 - 52* current_puck_stack_size, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE);
                    straightAdjust(FRONT_SENSORS);
                    changeOrientation(WEST, SPEED_LOW);

                }
                
                state = STATE_FIND_REQUIRED_PUCK;       // CHANGE STATE
                
            }
            
            
            
    // OLD CODE: 
            /*
            if (block_east_clearance && puck_east_clearance){
                
                moveUntil(WALL_CLEARANCE_FRONT, BACKWARD, GREATER_THAN, FRONT_MIDDLE, SPEED, TRUE);
                straightAdjust(FRONT_SENSORS);
<<<<<<< HEAD
                changeOrientation(NORTH, SPEED);    // gets it ready for going aslong the wall 
                //straightAdjust();
                moveUntil(PUCK_GRID_FROM_NORTH - DISTANCE_FRONT_SENSOR_FROM_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * current_puck_stack_size, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
=======
                changeOrientation(NORTH, SPEED);
                straightAdjust(BACK_SENSORS);
                moveUntil(PUCK_GRID_FROM_NORTH - DISTANCE_MIDDLE_SENSOR_TO_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * current_puck_stack_size, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE); ////////////////
>>>>>>> 46d80350303a0caac89f9e1b47d05a20e237c383
                straightAdjust(FRONT_SENSORS);
                changeOrientation(WEST, SPEED);
                straightAdjust(BACK_SENSORS); /////////////////////////////////////

                
                moveUntil(DISTANCE_MIDDLE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                changeOrientation(NORTH, SPEED_LOW);
                straightAdjust(FRONT_SENSORS); /////////////////////////////////////
                moveForwardThenBackward(PUCK_GRID_FROM_NORTH - DISTANCE_MIDDLE_SENSOR_TO_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * current_puck_stack_size, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE);
                straightAdjust(FRONT_SENSORS);
                changeOrientation(WEST, SPEED_LOW);
                
            }
            
            else if (block_west_clearance && puck_west_clearance){
            
                //moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED); // Remove when displaceLeft is working
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK - DISTANCE_BACK_SENSOR_FROM_CENTER, BACKWARD, LESS_THAN, BACK_RIGHT, SPEED, TRUE);
<<<<<<< HEAD
                straightAdjust(BACK_SENSORS); //? Remove if it fucks things up
=======
                straightAdjust(FRONT_SENSORS); //? Remove if it fucks things up
>>>>>>> 46d80350303a0caac89f9e1b47d05a20e237c383
                changeOrientation(NORTH, SPEED);
                straightAdjust(BACK_SENSORS); //? Remove if it fucks things up
                moveUntil(PUCK_GRID_FROM_NORTH - DISTANCE_FRONT_SENSOR_FROM_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * current_puck_stack_size, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                //translateMoveDynamic(CLEARANCE_RADIUS_CENTER_TO_BACK, 10, SPEED, FALSE); // Change safety to TRUE when implemented
                straightAdjust(FRONT_SENSORS);
                changeOrientation(EAST, SPEED);
                straightAdjust(BACK_SENSORS);

                moveUntil(DISTANCE_MIDDLE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);   
                changeOrientation(NORTH, SPEED_LOW);
                moveForwardThenBackward(PUCK_GRID_FROM_NORTH - DISTANCE_FRONT_SENSOR_FROM_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * current_puck_stack_size, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE);
                straightAdjust(FRONT_SENSORS);
                changeOrientation(EAST, SPEED_LOW);
            }
            
            
            else if (block_east_clearance && puck_west_clearance){

                //moveUntil(block_and_puck_edge_midpoint - DISTANCE_BACK_SENSOR_FROM_CENTER, BACKWARD, LESS_THAN, BACK, SPEED, TRUE);
                moveUntil(block_location[EAST] - DISTANCE_FRONT_SENSOR_FROM_CENTER - WIDTH_WHEEL_TO_WHEEL / 2 - 10 , BACKWARD, GREATER_THAN, FRONT_MIDDLE, SPEED, TRUE);
                straightAdjust(FRONT_SENSORS);
                changeOrientation(NORTH, SPEED);
                //straightAdjust();
                moveUntil(PUCK_GRID_FROM_NORTH - DISTANCE_FRONT_SENSOR_FROM_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * current_puck_stack_size, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                straightAdjust(FRONT_SENSORS);
                changeOrientation(EAST, SPEED);
                straightAdjust(BACK_SENSORS);
                
                moveUntil(DISTANCE_MIDDLE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                changeOrientation(NORTH, SPEED_LOW);
                moveForwardThenBackward(PUCK_GRID_FROM_NORTH - DISTANCE_FRONT_SENSOR_FROM_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * current_puck_stack_size, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE);
                straightAdjust(FRONT_SENSORS);
                changeOrientation(EAST, SPEED_LOW);
            }
            
            else if (block_west_clearance && puck_east_clearance){
                
                moveUntil(block_location[WEST] - DISTANCE_FRONT_SENSOR_FROM_CENTER + WIDTH_WHEEL_TO_WHEEL / 2 + 10 , BACKWARD, GREATER_THAN, FRONT_MIDDLE, SPEED, TRUE);
                straightAdjust(FRONT_SENSORS);
                changeOrientation(NORTH, SPEED);
                moveUntil(PUCK_GRID_FROM_NORTH - DISTANCE_FRONT_SENSOR_FROM_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * current_puck_stack_size, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                straightAdjust(FRONT_SENSORS);
                changeOrientation(WEST, SPEED);
                straightAdjust(BACK_SENSORS);
                
                moveUntil(DISTANCE_MIDDLE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                changeOrientation(NORTH, SPEED_LOW);
                moveForwardThenBackward(PUCK_GRID_FROM_NORTH - DISTANCE_FRONT_SENSOR_FROM_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * current_puck_stack_size, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE);
                straightAdjust(FRONT_SENSORS);
                changeOrientation(WEST, SPEED_LOW);
            }
            */
            
            

        }
 
//        
// *** 4. STATE FIND REQUIRED PUCK: *** // 
//
        
        
        if (state == STATE_FIND_REQUIRED_PUCK){
        
            int puck_correct = FALSE;   // A flag to determine if the correct puck has been picked up
            int puck_scan;
            
        
            //Temp delete after teesting!!!!!!!!!!!!!!!!!!!!!
            puck_correct = TRUE;
            
                
            // Scanning puck:
                
            safety_override = TRUE;     // DEACTIVATES the safety override so pucks don't interfere  

            while (puck_correct == FALSE) 
            {
                moveDynamic(50, SPEED_LOW, TRUE);  // robot moves forward towards puck
                                        // could replace this with the distance gathered from the ultrasonic 
                changeHeightToPuck(ABOVE_1_PUCK, NEITHER);  // arm lowers onto robot
                puck_scan = colourSensingOutput();  // colour sensor takes a scan
                changeHeightToPuck(ABOVE_3_PUCK, NEITHER);  // arm returns to high position
                moveDynamic(-50, SPEED, TRUE);   // robot moves backwards
                if (puck_scan == puck_construction_plan[current_stage-1]) {puck_correct = TRUE;}                
                else 
                {
                    /*
                    // Bring us to the next puck along: 
                    moveSwivel(45, SPEED, TRUE);
                    moveDynamic(-30,SPEED, TRUE);
                    moveSwivel(-30, SPEED, TRUE);
                    */
                }
            }

            changeHeightToPuck(GROUND, NEITHER);      // arm lowers to ground
            //armClose();
            //changeHeightToPuck(1);
            moveUntilPuck(CLAW_GROUND_ALGORITHM);
            //moveDynamic(60);       // robot moves forward
            armClose();                 // claw closes on puck
            changeHeightToPuck(ABOVE_3_PUCK, CLOSE);      // arm lifts up to highest position
            //armCloseIndefinitely(TRUE);
            moveDynamic(-60, SPEED_LOW, TRUE);       // robot moves back away from puck area 
            //armCloseIndefinitely(FALSE);
            
            safety_override = FALSE;        // reactivate safety override
        
            //while(1) {}
            
            
            state = STATE_RETURN_TO_SOUTH;
       
        }
 
        
//
// *** 5. STATE RETURN TO SOUTH: *** // 
//      
        
        // Assume that we are currently in the north side of the arena facing the pucks.
        // This state will return us to the south side of the block and face east at the east wall.
        if (state == STATE_RETURN_TO_SOUTH){
            UART_1_PutString("entering return to south");
            if (preferential_route == EAST){
                UART_1_PutString("entering east ret to south");
                straightAdjust(BACK_SENSORS); /////////////////////////
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK - 190, BACKWARD, LESS_THAN, BACK_RIGHT, SPEED, TRUE);
                // straightAdjust() using back sensor?
                changeOrientation(NORTH,SPEED);
                straightAdjust(FRONT_SENSORS);
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK - 140, BACKWARD, LESS_THAN, BACK_RIGHT, SPEED, TRUE); // May need to stop sooner so as to avoid the potential pucks on back wall
                straightAdjust(BACK_SENSORS);
                changeOrientation(EAST,SPEED);
                straightAdjust(FRONT_SENSORS);
                moveUntil(WALL_CLEARANCE_FRONT, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                straightAdjust(FRONT_SENSORS);
                
            }
            
            else if (preferential_route == WEST){
                
                UART_1_PutString("enterint ret to south west");
                moveUntil(30, BACKWARD, LESS_THAN, BACK_RIGHT, SPEED, TRUE);   // backs us into the corner
                //moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK, FORWARD, GREATER_THAN, BACK_RIGHT, SPEED, TRUE);
                straightAdjust(BACK_SENSORS);
                changeOrientation(NORTH,SPEED);
                straightAdjust(FRONT_SENSORS);
                //moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK - DISTANCE_BACK_SENSOR_FROM_CENTER, BACKWARD, LESS_THAN, BACK_LEFT, SPEED, TRUE); // May need to stop sooner so as to avoid the potential pucks on back wall
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);
                straightAdjust(FRONT_SENSORS);
                changeOrientation(EAST,SPEED);
                straightAdjust(BACK_SENSORS);
                //moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK - DISTANCE_BACK_SENSOR_FROM_CENTER, BACKWARD, LESS_THAN, BACK_RIGHT, SPEED, TRUE);
                changeOrientation(SOUTH,SPEED);
                straightAdjust(BACK_LEFT);
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                
                // above moveUntil isn't stopping early enough i think (IT WAS CAUSE OF FRONT_MIDDLE BEING USED_
                
                changeOrientation(EAST,SPEED);
                straightAdjust(BACK_SENSORS);  // we should be in the east where we originally started 
                //moveUntil(WALL_CLEARANCE_FRONT, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                moveUntil(150, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
              
            }
            
            else if (preferential_route == MIDDLE) {
                UART_1_PutString("entering ret middle");
                if (block_east_clearance == 1) {
                    changeOrientation(NORTH, SPEED);
                    moveUntil(500,BACKWARD, LESS_THAN, BACK_LEFT, SPEED, TRUE);
                    changeOrientation(WEST,SPEED);
                    changeOrientation(SOUTH,SPEED);
                    moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK,FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                    straightAdjust(FRONT_SENSORS);
                    changeOrientation(EAST,SPEED);
                    moveUntil(150,FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                                        
                }
                else {
                    // know the block is far away, just travel straight down
                    changeOrientation(NORTH, SPEED);
                    moveUntil(500,BACKWARD, LESS_THAN, BACK_LEFT, SPEED, TRUE);
                    changeOrientation(EAST,SPEED);
                    changeOrientation(SOUTH,SPEED);
                    moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK,FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                    straightAdjust(FRONT_SENSORS);
                    changeOrientation(EAST,SPEED);
                    moveUntil(150,FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                }
                
            }
            
            
            
    // Old code         
            /*
            else if (block_east_clearance && puck_west_clearance){

                moveUntil(ARENA_WIDTH - (block_location[EAST] - DISTANCE_FRONT_SENSOR_FROM_CENTER - WIDTH_WHEEL_TO_WHEEL / 2 - 10) , BACKWARD, LESS_THAN, BACK_RIGHT, SPEED, TRUE);
                //straightAdjust();
                changeOrientation(NORTH, SPEED);
                straightAdjust(FRONT_SENSORS);
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK - 140, BACKWARD, LESS_THAN, BACK_RIGHT, SPEED, TRUE); // May need to stop sooner so as to avoid the potential pucks on back wall
                //displaceLeft(); Repeatedly call this if below function not implemented
                //displaceLeftUntil(CLEARANCE_RADIUS_CENTER_TO_BACK,RIGHT);
                changeOrientation(EAST, SPEED);
                //displaceLeft();
                //displaceLeftUntil(DISTANCE_PUCKS_FROM_NORTH + WIDTH_SENSOR_TO_CENTER ,RIGHT);
                moveUntil(WALL_CLEARANCE_FRONT, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                
            }
            
            else if (block_west_clearance && puck_east_clearance){
                
                moveUntil(ARENA_WIDTH - (block_location[WEST] - DISTANCE_FRONT_SENSOR_FROM_CENTER + WIDTH_WHEEL_TO_WHEEL / 2 + 10 ), BACKWARD, GREATER_THAN, FRONT_MIDDLE, SPEED, TRUE);
                changeOrientation(NORTH, SPEED);
                straightAdjust(FRONT_SENSORS);
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK - 140, BACKWARD, LESS_THAN, BACK_RIGHT, SPEED, TRUE); // May need to stop sooner so as to avoid the potential pucks on back wall
                //displaceLeft(); Repeatedly call this if below function not implemented
                //displaceLeftUntil(CLEARANCE_RADIUS_CENTER_TO_BACK,RIGHT);
                changeOrientation(EAST, SPEED);
                //displaceLeft();
                //displaceLeftUntil(DISTANCE_PUCKS_FROM_NORTH + WIDTH_SENSOR_TO_CENTER ,RIGHT);
                moveUntil(WALL_CLEARANCE_FRONT, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                
            }   
            */
            
            
            state = STATE_DEPOSIT_PUCK;
        }
        
//        
// *** 6. STATE DEPOSIT PUCK: *** // 
//
        
        // We choose where we put the puck. Depending if its needed now, later or never.
        if (state == STATE_DEPOSIT_PUCK){
                
            
            // Temp grab puck
            control_photodiode_Write(CLAW_SENSING);    // changes to claw photodiode
            colour_sensing_algorithm = CLAW_GROUND_ALGORITHM;   // changes it to claw algorithm
            
            /*
            armOpen();
            colourSensingInitialise();
            CyDelay(2000);
            armClose();
            */
            
            
            
            int heldColour = colourSensingOutput();
            //blinkLED(heldColour,500);
            
            
            // TO DELETE
            heldColour = current_stage; // temp checking it can stack with a fixed rgb order for simplicity
            //armOpen();
            //CyDelay(1000);
            //armClose();
            
            
            if (heldColour == puck_construction_plan[current_puck_stack_size] ) { // The currently held puck should go on the construction pile now
                translateUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT + 10,200,LEFT,GREATER_THAN);
                changeOrientation(SOUTH,SPEED);
                moveForwardThenBackward(CLEARANCE_RADIUS_CENTER_TO_BACK + DISTANCE_MIDDLE_SENSOR_TO_CENTER,LESS_THAN,FRONT_MIDDLE,SPEED_LOW,TRUE);
                changeOrientation(EAST,SPEED);
                straightAdjust(FRONT_SENSORS);
                moveUntil(HOME_MIDPOINT - DISTANCE_MIDDLE_SENSOR_TO_CENTER, BACKWARD, GREATER_THAN, FRONT_MIDDLE, SPEED, TRUE);  
                changeOrientation(NORTH,SPEED);
                changeOrientation(WEST,SPEED);
                translateUntil(120,200,LEFT,LESS_THAN);
                moveUntil(CONSTRUCTION_MIDPOINT - DISTANCE_FRONT_SENSOR_FROM_CENTER - 30, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE); //Change 30 to constant
                //moveForwardThenBackward(CONSTRUCTION_MIDPOINT - DISTANCE_FRONT_SENSOR_FROM_CENTER - 30, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE);
                moveForwardThenBackward(190, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE);
                straightAdjust(FRONT_SENSORS);
                
                //changeOrientation(SOUTH, SPEED);
                //moveUntil(CONSTRUCTION_MIDPOINT - DISTANCE_FRONT_SENSOR_FROM_CENTER, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, FALSE);
                
                
                
            }
            /*
            else if ( heldColour == puck_construction_plan[1] || heldColour == puck_construction_plan[2]){ // Then put this puck in the home base for grabbing later
                moveUntil(ARENA_WIDTH - FIRST_TEMP_DROPOFF + DISTANCE_FRONT_SENSOR_FROM_CENTER, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE); // Check if already droppd off, go to second_temp_dropoff
                straightAdjust();
            }
            
            else {
                moveUntil(ARENA_WIDTH-FIRST_DISCARD_ZONE + DISTANCE_FRONT_SENSOR_FROM_CENTER, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE); // Check if already droppd off, go to second_temp_dropoff
            } // Discard the puck so drop it in rubbish pile
            
            */

            changeOrientation(SOUTH,SPEED);    
            changeHeightToPuck(ABOVE_3_PUCK, CLOSE); // Lift claw above stack to avoid hitting the stack  
            
            if (current_puck_stack_size == 0){
                straightAdjust(FRONT_SENSORS);
                //moveUntil(CONSTRUCTION_DISTANCE_FROM_WALL, FORWARD, LESS_THAN, FRONT_LEFT, SPEED,TRUE); // This function is being triggered by the stack. Only use this for non stacking part (ie. home base dropping) 
                moveForwardThenBackward(150, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE);
            }
            else {
                moveUntil(DISTANCE_BEFORE_STACK_MIDDLE_SENSOR, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, FALSE); // This function is being triggered by the stack. Only use this for non stacking part (ie. home base dropping)
                //moveDynamic(140, SPEED, FALSE);            
            }
            
            //moveDynamic(CONSTRUCTION_DISTANCE_CLEAR_FROM_STACK - CONSTRUCTION_DISTANCE_FROM_WALL,SPEED);
            
            //armCloseIndefinitely(FALSE);
            
            changeHeightToPuck(current_puck_stack_size, CLOSE);
            armOpen();
            changeHeightToPuck(ABOVE_3_PUCK, OPEN); // Move higher than stack to avoid hitting it.
            

            if (current_stage >= 3){state = STATE_PARK_HOME;}        // Returns to home 
            else {current_stage++; current_puck_stack_size++;
            // Need to go back to the east wall facing east.
                //moveUntil(CONSTRUCTION_DISTANCE_CLEAR_FROM_STACK, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, FALSE);
                moveDynamic(-130,SPEED,TRUE);
                changeOrientation(WEST,SPEED);
                straightAdjust(FRONT_SENSORS);
                moveUntil(HOME_MIDPOINT - DISTANCE_FRONT_SENSOR_FROM_CENTER, BACKWARD, GREATER_THAN, FRONT_MIDDLE, SPEED, TRUE);  
                straightAdjust(FRONT_SENSORS);
                changeOrientation(NORTH,SPEED);
                changeOrientation(EAST,SPEED);
                moveUntil(DISTANCE_FRONT_SENSOR_FROM_CENTER + SAFETY_MARGIN + 50, FORWARD, LESS_THAN, FRONT_MIDDLE, SPEED, TRUE);
                straightAdjust(FRONT_SENSORS);
                
                
                //Usually we will check if the next colour we need is in home base already
                //
                // If in east edge of home base
                if ( puck_construction_plan[current_puck_stack_size] == puck_temp_pile[0]){
                    
                }
                //If in west edge of home base
                else if(puck_construction_plan[current_puck_stack_size] == puck_temp_pile[1]){
                         
                }
                
                state = STATE_GO_TO_PUCKS; // Go grab the next puck from the pile since it isn't in the home base
            }
            
            

        }
 
//        
// *** 7. STATE PARK HOME: *** // 
//        
        
        if (state == STATE_PARK_HOME){
            
            // When we enter this state we can assume that we have just deposited the final puck onto the stack.
            // Thus we are facing the stack.
            
            
            moveUntil(CONSTRUCTION_DISTANCE_CLEAR_FROM_STACK, BACKWARD, GREATER_THAN, FRONT_MIDDLE, SPEED, FALSE);
            changeOrientation(WEST,SPEED);
            straightAdjust(FRONT_SENSORS);
            moveForwardThenBackward(HOME_MIDPOINT + DISTANCE_MIDDLE_SENSOR_TO_CENTER - 45, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE); // Absorb the 50 into some constant later
            straightAdjust(FRONT_SENSORS);
            changeOrientation(NORTH,SPEED);
            moveUntil(HOME_PARKING_DISTANCE , BACKWARD, LESS_THAN, BACK_RIGHT, SPEED_LOW, TRUE);

            // END Congratulations:
            changeHeightToPuck(GROUND, NEITHER);

            while(1) {} // Finish inside infinite loop
            
            // Run code to stop robot entirely.
        }
        
        
    }    
}