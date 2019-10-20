 
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
int running = 1;
int sweeping = 0;
float block_edge_location[4] = {0,0,0,0}; // N E S W edge positions respectively

// * PUCK CONSTRUCTION VARIABLES * //
int currentPuckStackSize = 0; // How many pucks are currently on the construction plate
int puckRackColours[5] = {BLANK,BLANK,BLANK,BLANK,BLANK}; // 5 slots in puck rack.
int puckConstructionPlan[3] = {BLANK,BLANK,BLANK};
int currentPuckRackScanningIndex = 0;
int puckRackOffsetsFromWest[5] = {PUCK_RACK_0_WEST_DISTANCE,
                                PUCK_RACK_1_WEST_DISTANCE,
                                PUCK_RACK_2_WEST_DISTANCE,
                                PUCK_RACK_3_WEST_DISTANCE,
                                PUCK_RACK_4_WEST_DISTANCE };

int puckColoursTempPile[2] = {BLANK,BLANK}; // Index 0 refers to east home base edge, 1 refers to west home base edge

float horizontalPuckGrid = 0;

// * NAVIGATION AND POSITION VARIABLES * //

int beginNavigation = 0; // Allow us to break out of the intial phase when powered up
int pathToPucks; // This will give us a corridor/ that we should initially take when trying to go to the pucks
int pathPastBlock;
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
short int blockEastClearance = 0;
short int blockWestClearance = 0;
short int puckEastClearance = 0;
short int puckWestClearance = 0;

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
    if (beginNavigation){
        UART_1_PutString("Beginning Navigation \n");
        //state = STATE_SCAN_PLAN; 
        state = STATE_LOCATE_BLOCK_AND_PUCKS;
    }
}
 
//Interrupt service routines for dcmotor function
/*
CY_ISR(Encoder_Counts_1_IH){
    stopMotor1AndUpdate();
}

CY_ISR(Encoder_Counts_2_IH){
    stopMotor2AndUpdate();
}

CY_ISR(Drift_Check_IH){    
    Drift_Check_Timer_ReadStatusRegister(); //Clears the interrupt
    Drift_Check_Timer_Stop(); //Stops the timer
    motor1EncoderCounts = Motor_1_Encoder_Counts_ReadCounter();
    motor2EncoderCounts = Motor_2_Encoder_Counts_ReadCounter();
    driftCorrect(); //Does checking
}
*/

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
    
    internal_orientation = WEST;                // robot initial starts in the East direction
    
    // Timer and ISR instantiation
    Timer_1_Start();                
    Timer_1_ReadStatusRegister();
    Sonic_StartEx(TIH);
    Start_StartEx(StartIH);
    beginNavigation = 1; // Set to true after it has run ISR at least once
    
    // Ultrasonic Initialisation & Calibration:
    // The ultrasonics take several test measurements to ensure they are working
        
    UART_1_PutString("Ultrasonic Calibration tests: \n");
    safetyDistanceCheck();
    safetyDistanceCheck();
    safetyDistanceCheck();
    
    /*  OLD CODE FROM NIDHIN:
    //Initialising DC motors
    Motor_1_driver_Start();
    Motor_2_driver_Start();
    motorDutyCycleUpdate(0,0,0,0);//Have the motors stand still
    
    
    //Initialising counters
    Motor_1_Encoder_Counts_Start();
    Motor_2_Encoder_Counts_Start();
    
    //Initialising timers
    Drift_Check_Timer_Start();
    
    //Starts and enables the interrupts for the motor encoder counts
    Encoder_Counts_1_Interrupt_StartEx(Encoder_Counts_1_IH); 
    Encoder_Counts_2_Interrupt_StartEx(Encoder_Counts_2_IH);
    Drift_Check_Interrupt_StartEx(Drift_Check_IH);		
    */

    // FORCING STATE:
    // Manual state set for testing
    state = STATE_PRE_RUN;
    
    //state = STATE_DEPOSIT_PUCK;
    currentPuckStackSize = 0;
    current_stage = 1;
    blockEastClearance = 0;
    blockWestClearance = 0;
    puckEastClearance = 0;
    puckWestClearance = 0;
    int block_and_puck_edge_midpoint = 500; // take the midpoint between inner edge between the pucks and the block
    //internal_orientation = EAST;
    puckConstructionPlan[0] = RED;
    puckConstructionPlan[1] = GREEN;
    puckConstructionPlan[2] = BLUE;
    

    // Main Loop for States

    
    
    
    for(;;)
    {

        if (state == STATE_PRE_RUN){
        
        }
        
        //changeOrientation(SOUTH, SPEED);
        //changeOrientation(NORTH, SPEED);
        //changeOrientation(WEST, SPEED);
        
        //while(1) {}
        

        
        
        while(0) {
                UART_1_PutString("turning left\n");
                moveSwivel(-25,20,FALSE);
                UART_1_PutString("turning right \n");
                moveSwivel(25,20,FALSE);
        }
        
        while(0){
            distanceSensor(SIDE_LEFT);
            CyDelay(60);
            sprintf(output, "%d \t", ultrasonic_distances_mm[SIDE_LEFT]);
            UART_1_PutString(output);
        }
        
        
        
        
        
        //changeHeightToPuck(GROUND,NEITHER);
        //moveUntilPuck(CLAW_BLACK_PUCK_ALGORITHM);
        //translateUntil(150, LEFT, LESS_THAN, SIDE_LEFT, SPEED);
        
        //while(1);
        
       
        
        /*
        for (int i = 0; i < 6; i++) {
            translateMoveDynamic(10, -15, 100, FALSE);      // This will translate to the right by one puck
        }
        
        for (int i = 0; i < 6; i++) {
            translateMoveDynamic(-10, 15, 100, FALSE);      // This will translate to the left by one puck
        }
        */
        
        
        // checks the distance measured by the ultrasonic
        //moveDynamic(move,SPEED,TRUE);
        //moveDynamic(-move,SPEED,TRUE);
        
        // wasnt moving for some of the tests 
            
        
    
        // Start button is pressed so quit sensing

        
        /*
        while (beginNavigation == 0) {
            distanceCheck();           
            UART_1_PutString("\n");
            CyDelay(1000); // Check distance once a second.
                
        }
        */
        
        
        
        
        
        // Enter picking up puck state for the moment FOR TESTING
        //colour_sensing_algorithm = 1;
        //control_photodiode_Write(1);
        //state = STATE_FIND_REQUIRED_PUCK;
        
        
     
        
        //state = STATE_FIND_REQUIRED_PUCK;
        
        //moveUntil(100,FORWARD,GREATER_THAN,SIDE_LEFT,SPEED);
        
        //while(1) {};
        
        
        // state = STATE_LOCATE_BLOCK_AND_PUCKS;
        
        
        //UART_1_PutString("hI");
        
        //changeOrientation(NORTH, SPEED);
        //changeOrientation(WEST, SPEED);
                
        //state = STATE_DEPOSIT_PUCK;
        //currentPuckStackSize = 2;
        //current_stage = 3;
         
//
// *** 1. STATE SCAN PLAN: *** // 
//
        

        
        
        
        if (state == STATE_SCAN_PLAN) {              // colour sensing, while switch has not been pushed. change to if eventually
            
            
            
            // Debugging: 
        
            ultimateDebugging();        // If any of the debugging flags are activated, this will be called over the main code

            
            
            // Checking translating function:
            
            //straightAdjust();
            //blinkLED(GREEN,1000);
            //moveSwivel(180, SPEED_LOW, FALSE);
            //straightAdjustBack();
            //blinkLED(GREEN, 1000);
            
            //while(1) {}
            
            // TRUE START:
                
            
            while(0){
                moveUntil(400, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                moveDynamic(-450, SPEED, TRUE);
                CyDelay(500);   
                colourSensingInitialise();      // Initialises wall colour sensor against the black wall 
                CyDelay(500);
                moveDynamic(170, SPEED, TRUE);
                CyDelay(500);
                
                
                
                for (int i = 0; i < 5; i++) {                       // scan each of the pucks 
                    puckRackColours[i] = colourSensingOutput();
                    CyDelay(500);
                    moveDynamic(61, SPEED, TRUE);
                    CyDelay(500);
                }
            
            }


            
            straightAdjust();
            moveUntil(-100, BACKWARD, LESS_THAN, BACK, SPEED, TRUE);
            colourSensingInitialise();      // Initialises wall colour sensor against the black wall 
                        
            
            
            for (int i = 0; i < 5; i++) { 
                // scan each of the pucks 
                //moveUntil(puckRackOffsetsFromWest[i]);
                puckRackColours[i] = colourSensingOutput();
                CyDelay(500);

            //straightAdjust();
            }
            
            UART_1_PutString("Found Colours: \n");
            for (int i = 0; i < 5; i++) 
            {
                sprintf(output, "%i \n", puckRackColours[i]);    // updates UART showing values of scanned pucks    
                UART_1_PutString(output);
            }
            
            
            control_photodiode_Write(CLAW_SENSING);    // changes to claw photodiode
            colour_sensing_algorithm = CLAW_1_ALGORITHM;   // changes it to claw algorithm
                                 
            // THEN WE FILTER THE NO PUCK VALUES:
            int puckConstructionPlanIndex = 0; // Will be used to iterate through the 3 puck colours in construction plan
            
            // Iterate over all 5 rack slots and place the 3 colours into puckConstructionPlan.
            for (int puckRackIndex = 0; puckRackIndex < 5; puckRackIndex++){
                if (puckRackColours[puckRackIndex] != BLANK && puckConstructionPlanIndex != 3){
                    puckConstructionPlan[puckConstructionPlanIndex] = puckRackColours[puckRackIndex];
                    puckConstructionPlanIndex++;
                }
            }
                    
            // MOVE into the next state: 
            state = STATE_LOCATE_BLOCK_AND_PUCKS;
            
            /*
            if (!moving){
                moving = 1;
            if (initialisation){moveBackwardIndefinitely(); initialisation = 0;}
            else {moveForwardIndefinitely();}        

            moveAndAngle(SCAN_INITIALISE_HORIZONTAL,PLAN_SCAN_VERTICAL,WEST_ANGLE); // Move beyond the puck rack to scan the black empty wall
            
            // Read the black wall
            
            for (currentPuckRackScanningIndex = 0; currentPuckRackScanningIndex <= 4; currentPuckRackScanningIndex++){
                moveAndAngle(puckRackOffsetsFromWest[currentPuckRackScanningIndex],PLAN_SCAN_VERTICAL,WEST_ANGLE); // Choose the plan vertical to be whatever Y value we start at
                puckRackColours[currentPuckRackScanningIndex] = colourSensingOutput();
            }
   
            */
        }

//
// *** 2. STATE LOCATE BLOCKS AND PUCKS: *** // 
//
        
    	if (state == STATE_LOCATE_BLOCK_AND_PUCKS){
            

            ultimateDebugging();   

            // move away from home base:
            
            translateMoveDynamic(25, -35, 100, FALSE);
            straightAdjust();
    
            // Move until construction zone            
            moveUntil(130, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);  // Move to west wall
            straightAdjust();                                       // straighten against west wall
            
            // SCAN FOR BLOCKS:
            
            distanceSensor(SIDE_LEFT);  // takes how far we are away from home base wall
            while (ultrasonic_distances_mm[SIDE_LEFT] > (BLOCK_ZONE_SOUTH - WIDTH_SENSOR_TO_SENSOR)) {
                // stays in while loop until side left value taken is appropriate
                // E.g. the side left value should be less than BLOCK_ZONE_SOUTH - WIDTH_SENSOR_TO_SENSOR
                // So, it should be less than 500mm - 220mm = 280mm
                distanceSensor(SIDE_LEFT);
               } 
            
            
            int block_check = BLOCK_ZONE_NORTH - ultrasonic_distances_mm[SIDE_LEFT] - WIDTH_SENSOR_TO_SENSOR + (BLOCK_WIDTH/2);    
                // TAKES our distance from north wall, 
                // takes distance from arena, takes away 
                // minus 50 is a tolerance
                // This equals 700MM - leftside - 220mm + 45 = 525mm maximum, but we'll be away from the south facing wall 
            
            sprintf(output, "distance from robot to block: %d \t", block_check);
            UART_1_PutString(output);
            
            //block_check = 350;  // This is a default length that isn't dynamic, the block check
            
            
            // Stopping at the West side of the block: 
            moveUntil(block_check, BACKWARD, LESS_THAN, SIDE_RIGHT, SPEED, TRUE);   
                                // this will move backwards until it hits the block or the wall
            blinkLED(GREEN,1000);      // To show it ended at the correct spot
            
            
            // Update the west clearance of the block: 
            
            //toleranceCheck();       // This checks if the values being polled are appropriate 
            
            straightAdjust();
            distanceSensor(FRONT_LEFT); // Use front_right instead? 
            CyDelay(DELAY);
            
            block_location[WEST] = ultrasonic_distances_mm[FRONT_LEFT] + DISTANCE_FRONT_SENSOR_TO_SIDE_SENSOR;
            
 
            //straightAdjust();                   // Ensure we are straight to take measurements
                                                  // may need to readjust this 
            
            
            // Move back a bit to find the left side and right side values (adds a bit of tolerance)
            //moveDynamic(-(BLOCK_WIDTH/2), SPEED, TRUE);   
            // Double check if the above is required
            
            //straightAdjust();                   // Ensure we are straight to take measurements
                                                // may need to readjust this 
            
            
            // Updating the South Clearances of the object: 
            
            distanceSensor(SIDE_RIGHT); 
            CyDelay(60); 
            distanceSensor(SIDE_LEFT); 
            CyDelay(60); 
            
            block_location[SOUTH] = ultrasonic_distances_mm[SIDE_LEFT] + WIDTH_SENSOR_TO_SENSOR + ultrasonic_distances_mm[SIDE_RIGHT];
            
            // Check that the value makes sense: 
            if (block_location[SOUTH] > block_check) {
                moveDynamic(-(BLOCK_WIDTH/2),SPEED,TRUE);
                
                distanceSensor(SIDE_RIGHT); 
                CyDelay(60); 
                distanceSensor(SIDE_LEFT); 
                CyDelay(60); 
            
            block_location[SOUTH] = ultrasonic_distances_mm[SIDE_LEFT] + WIDTH_SENSOR_TO_SENSOR + ultrasonic_distances_mm[SIDE_RIGHT]; 
            }
            
            // Stopping at the East side of the block: 
            moveUntil(block_check, BACKWARD, GREATER_THAN, SIDE_RIGHT, SPEED, TRUE);  
                                // this will move backwards until no longer hitting the block, or the wall
            blinkLED(GREEN,1000);      // To show it ended at the correct spot
            
            // Updating the East Clearance:
            block_location[EAST] = ultrasonic_distances_mm[BACK] + DISTANCE_BACK_SENSOR_TO_SIDE_SENSOR;
                                                        // double check the distance back sensor to side sensor value
                
            // Updating the North Clearance: 
            if (ARENA_WIDTH - block_location[EAST] - block_location[EAST] > BLOCK_WIDTH) {
                // block is " -- " this orientation
                block_location[NORTH] = ARENA_WIDTH - block_location[SOUTH] - BLOCK_WIDTH;                
            }
            else {
                // block is " | " this orientation
                block_location[NORTH] = ARENA_WIDTH - block_location[SOUTH] - BLOCK_LENGTH;
            }
            

            /*
            // If either of the following cases are true, we can just put in default values, 
                // as it will be easy to circumvent the block
                // This orientation will be unlikely 
            
            if (ultrasonic_distances_mm[BACK] < SAFETY_MARGIN + 50) {
                // block is " | " this orientation, and close to the side wall
                
                // Fill out all the details: 
                block_location[NORTH] = ARENA_WIDTH - ultrasonic_distances_mm[SIDE_LEFT] - ultrasonic_distances_mm[SIDE_RIGHT] - WIDTH_SENSOR_TO_SENSOR - BLOCK_LENGTH;
                block_location[EAST] = 0;
                block_location[SOUTH] = ultrasonic_distances_mm[SIDE_LEFT] + WIDTH_SENSOR_TO_SENSOR + ultrasonic_distances_mm[SIDE_RIGHT];
                block_location[WEST] = ARENA_WIDTH - BLOCK_WIDTH; 
                
            }
            if (ultrasonic_distances_mm[BACK] < BLOCK_LENGTH + 50) {
                // block is " -- " this orientation, and close to the side wall 
                
                // Fill out all the details: 
                block_location[NORTH] = ARENA_WIDTH - ultrasonic_distances_mm[SIDE_LEFT] - ultrasonic_distances_mm[SIDE_LEFT] - WIDTH_SENSOR_TO_SENSOR - BLOCK_LENGTH;
                block_location[EAST] = 0;
                block_location[SOUTH] = ultrasonic_distances_mm[SIDE_LEFT] + WIDTH_SENSOR_TO_SENSOR + ultrasonic_distances_mm[SIDE_RIGHT];
                block_location[WEST] = ARENA_WIDTH - BLOCK_LENGTH; 
            }
            

            // Block location values:
            block_location[SOUTH] = ultrasonic_distances_mm[SIDE_LEFT] + WIDTH_SENSOR_TO_SENSOR + ultrasonic_distances_mm[SIDE_RIGHT];
            block_location[WEST] = ultrasonic_distances_mm[FRONT_LEFT] + DISTANCE_FRONT_SENSOR_TO_SIDE_SENSOR; 
            CyDelay(1000);         
            
            */
            
            
            //PRINTING BLOCK LOCATIONS: 
            sprintf(output, "north: %d, \n", block_location[NORTH]);       
            UART_1_PutString(output);
            sprintf(output, "east: %d, \n", block_location[EAST]);       
            UART_1_PutString(output);
            sprintf(output, "south: %d, \n", block_location[SOUTH]);       
            UART_1_PutString(output);
            sprintf(output, "west: %d, \n", block_location[WEST]);       
            UART_1_PutString(output);
            
            
            // Start moving forward to find the location of the PUCKS:           
            changeOrientation(EAST, SPEED);                             // No matter where we are this should be okay
            moveUntil(150, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                    // above code could be inefficient if puck is closer to west side of arena 
            straightAdjust();
            
            distanceSensor(SIDE_RIGHT);
            CyDelay(50);
            
            sprintf(output, "%d \t", ultrasonic_distances_mm[SIDE_RIGHT]);
            UART_1_PutString(output);
            
            int puck_check = ARENA_LENGTH - PUCK_GRID_FROM_NORTH - ultrasonic_distances_mm[SIDE_RIGHT] - WIDTH_SENSOR_TO_SENSOR + 100;       
            puck_check = 700;
            
            sprintf(output, "puck check: %d, \n", puck_check);       
            UART_1_PutString(output);
                        
            // Check front and back sensors:
            moveUntil(puck_check, BACKWARD, LESS_THAN, SIDE_LEFT, SPEED, TRUE);   // this will move backwards until it hits the block 
            
            
            distanceSensor(FRONT_LEFT);
            CyDelay(DELAY);
            distanceSensor(BACK);
            CyDelay(DELAY);
            
            // checking if accidentally detected block:
            // if the distance we are at is less than the block, we can say we didnt detect the block
            if (ultrasonic_distances_mm[FRONT_LEFT] + DISTANCE_FRONT_SENSOR_TO_SIDE_SENSOR < block_location[EAST])
            {
                
                int offset = ultrasonic_distances_mm[SIDE_LEFT]*sin(18*M_PI/180);
                // puck detected
                // puck detectedDISTANCE_FRONT_SENSOR_TO_SIDE_SENSOR
                puck_location[EAST] = ultrasonic_distances_mm[FRONT_LEFT] + DISTANCE_FRONT_SENSOR_TO_SIDE_SENSOR + offset;
                puck_location[WEST] = ARENA_WIDTH - puck_location[EAST] - PUCK_GRID_WIDTH;
                
                blinkLED(GREEN,1000);   // puck location was succesfully found
            }
            else {
                // block detected & rescans from opposite side:
                
                blinkLED(RED,1000); // indicates we hit the block instead 
                
                moveUntil(200, BACKWARD, LESS_THAN, BACK, SPEED, TRUE);   // This should take us to the other side of the wall
                changeOrientation(WEST, SPEED);
                moveUntil(130, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                straightAdjust();   // adjusts against the wall
                moveUntil(puck_check, BACKWARD, LESS_THAN, SIDE_RIGHT, SPEED, TRUE);   // this will move backwards until it hits the 
                blinkLED(GREEN,1000);   // Find the edge of the puck zone 
                                
                distanceSensor(FRONT_LEFT);
                CyDelay(DELAY);
                distanceSensor(BACK);
                CyDelay(DELAY);
                
                int offset = ultrasonic_distances_mm[SIDE_RIGHT]*sin(18*M_PI/180);
                
                puck_location[WEST] = ultrasonic_distances_mm[FRONT_LEFT] + DISTANCE_FRONT_SENSOR_TO_SIDE_SENSOR + offset;
                puck_location[EAST] = ARENA_WIDTH - puck_location[WEST] - PUCK_GRID_WIDTH;
                
            }
            
            
            sprintf(output, "east puck: %d, \n", puck_location[EAST]);       
            UART_1_PutString(output);
            sprintf(output, "west puck: %d, \n", puck_location[WEST]);       
            UART_1_PutString(output);
            
            
            
            
            
            
            // Finding the different block & puck clearances: 
            
                    // there should be a threshold that allows the robot to travel through a clearance;
                    // E.g. robot width + 100mm
            
                    // the robot will travel to the chosen (block_clearance/2) and travel down there,
                    // so it is in the middle of the block clearance and the wall 
            
                    // check if between the edge of the block and the puck zone gives enough clearance 
                    
            
            if (block_location[EAST] > WIDTH_WHEEL_TO_WHEEL + BLOCK_TOLERANCE) {
                // the east clearance of block check
              blockEastClearance = 1;  
            }
            if (block_location[WEST] > WIDTH_WHEEL_TO_WHEEL + BLOCK_TOLERANCE) {
                // the east clearance of block check
              blockWestClearance = 1;  
            }
            if (puck_location[EAST] > WIDTH_WHEEL_TO_WHEEL + BLOCK_TOLERANCE) {
                // the east clearance of block check
              puckEastClearance = 1;  
            }
            if (puck_location[WEST] > WIDTH_WHEEL_TO_WHEEL + BLOCK_TOLERANCE) {
                // the east clearance of block check
              puckWestClearance = 1;  
            }
            
            
            
            //changeOrientation(WEST,SPEED);
            //moveDynamic(-600, SPEED, TRUE);
            //moveUntil((puck_location[EAST]/2),FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
            //straightAdjust();
            //changeOrientation(WEST,SPEED);
            //moveUntil((PUCK_GRID_FROM_NORTH/2), FORWARD, LESS_THAN, FRONT_LEFT, SPEED,TRUE);
            //straightAdjust();
            //changeOrientation(EAST, SPEED);
            ///oveDynamic(1200, SPEED, TRUE);
            
            moveUntil(200, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);
            changeOrientation(SOUTH,SPEED);
            changeOrientation(EAST, SPEED);
            straightAdjust();
            moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT + SAFETY_MARGIN/2, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
            straightAdjust();
            
            
            // PUCK has now been found, enter the IF statements to locate and pick up pucks 
            state = STATE_GO_TO_PUCKS;
	    }     

//
// *** 3. STATE GO TO PUCKS: *** // 
//

        // Ensure that we are @ east wall facing east at a minimum verticaldistance so we can turn left without hitting bottom wall
        
        if (state == STATE_GO_TO_PUCKS){
            

            // The different route finding Algorithms for the different combinations: 
            
            if (blockEastClearance && puckEastClearance){
                
                //moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED); // Remove when displaceLeft is working
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);
                straightAdjust();
                changeOrientation(NORTH, SPEED);
                //straightAdjust();
                moveUntil(PUCK_GRID_FROM_NORTH, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                translateUntil(PUCK_GRID_FROM_NORTH - WIDTH_SENSOR_TO_CENTER - PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS * currentPuckStackSize, RIGHT, LESS_THAN, SIDE_RIGHT, SPEED);
                //translateMoveDynamic(-100, 30, SPEED, FALSE); // Change safety to TRUE when implemented and use translateUntil
                //displaceLeft(); Repeatedly call this if below function not implemented
                //displaceLeftUntil(CLEARANCE_RADIUS_CENTER_TO_BACK,RIGHT);
                changeOrientation(WEST, SPEED);
                //displaceLeft();
                //displaceLeftUntil(DISTANCE_PUCKS_FROM_NORTH + WIDTH_SENSOR_TO_CENTER ,RIGHT);
                //moveUntil(DISTANCE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                
                
            }
            
            else if (blockWestClearance && puckWestClearance){
            
                //moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED); // Remove when displaceLeft is working
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK, BACKWARD, LESS_THAN, BACK, SPEED, TRUE);
                straightAdjust();
                changeOrientation(NORTH, SPEED);
                straightAdjust();
                moveUntil(PUCK_GRID_FROM_NORTH, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                //displaceLeft(); Repeatedly call this if below function not implemented
                //displaceLeftUntil(CLEARANCE_RADIUS_CENTER_TO_BACK,RIGHT);
                translateMoveDynamic(CLEARANCE_RADIUS_CENTER_TO_BACK, 10, SPEED, FALSE); // Change safety to TRUE when implemented
                changeOrientation(EAST, SPEED);
                //displaceLeft();
                //displaceLeftUntil(DISTANCE_PUCKS_FROM_NORTH + WIDTH_SENSOR_TO_CENTER ,RIGHT);
                //moveUntil(DISTANCE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                            
            }
            
            
            else if (blockEastClearance && puckWestClearance){

                moveUntil(block_and_puck_edge_midpoint - DISTANCE_BACK_SENSOR_FROM_CENTER, BACKWARD, LESS_THAN, BACK, SPEED, TRUE);
                straightAdjust();
                changeOrientation(NORTH, SPEED);
                straightAdjust();
                moveUntil(PUCK_GRID_FROM_NORTH, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                //displaceLeft(); Repeatedly call this if below function not implemented
                //displaceLeftUntil(CLEARANCE_RADIUS_CENTER_TO_BACK,RIGHT);
                changeOrientation(EAST, SPEED);
                //displaceLeft();
                //displaceLeftUntil(DISTANCE_PUCKS_FROM_NORTH + WIDTH_SENSOR_TO_CENTER ,RIGHT);
                //moveUntil(DISTANCE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                
            }
            
            else if (blockWestClearance && puckEastClearance){
                
                moveUntil(block_and_puck_edge_midpoint - DISTANCE_BACK_SENSOR_FROM_CENTER, BACKWARD, LESS_THAN, BACK, SPEED, TRUE);
                straightAdjust();
                changeOrientation(NORTH, SPEED);
                straightAdjust();
                moveUntil(PUCK_GRID_FROM_NORTH, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                //displaceLeft(); Repeatedly call this if below function not implemented
                //displaceLeftUntil(CLEARANCE_RADIUS_CENTER_TO_BACK,RIGHT);
                changeOrientation(WEST, SPEED);
                //displaceLeft();
                //displaceLeftUntil(DISTANCE_PUCKS_FROM_NORTH + WIDTH_SENSOR_TO_CENTER ,RIGHT);
                //moveUntil(DISTANCE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                
            }
            
            state = STATE_FIND_REQUIRED_PUCK;

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
            if (puck_scan == puckConstructionPlan[current_stage-1]) {puck_correct = TRUE;}                
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
        moveUntilPuck(CLAW_BLACK_PUCK_ALGORITHM);
        //moveDynamic(60);       // robot moves forward
        armClose();                 // claw closes on puck
        changeHeightToPuck(ABOVE_3_PUCK, CLOSE);      // arm lifts up to highest position
        moveDynamic(-60, SPEED, TRUE);       // robot moves back away from puck area 
        
        
        safety_override = FALSE;        // reactivate safety override
    
        state = STATE_RETURN_TO_SOUTH;
   
        }
 
        
//
// *** 5. STATE RETURN TO SOUTH: *** // 
//      
        
        // Assume that we are currently in the north side of the arena facing the pucks.
        // This state will return us to the south side of the block and face east at the east wall.
        if (state == STATE_RETURN_TO_SOUTH){
            if (blockEastClearance && puckEastClearance){
                
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK - 190, BACKWARD, LESS_THAN, BACK, SPEED, TRUE);
                // straightAdjust() using back sensor?
                changeOrientation(NORTH,SPEED);
                straightAdjust();
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK - 140, BACKWARD, LESS_THAN, BACK, SPEED, TRUE); // May need to stop sooner so as to avoid the potential pucks on back wall
                //straightAdjust();
                changeOrientation(EAST,SPEED);
                straightAdjust();
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                straightAdjust();
                
            }
            
            else if (blockWestClearance && puckWestClearance){
            
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK - 140, BACKWARD, LESS_THAN, BACK, SPEED, TRUE);
                // straightAdjust() using back sensor?
                changeOrientation(NORTH,SPEED);
                straightAdjust();
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK, BACKWARD, LESS_THAN, BACK, SPEED, TRUE); // May need to stop sooner so as to avoid the potential pucks on back wall
                straightAdjust();
                changeOrientation(EAST,SPEED);
                straightAdjust();
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                straightAdjust();

                
                //moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED); // Remove when displaceLeft is working
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_BACK, BACKWARD, LESS_THAN, BACK, SPEED, TRUE);
                straightAdjust();
                changeOrientation(NORTH, SPEED);
                straightAdjust();
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                //displaceLeft(); Repeatedly call this if below function not implemented
                //displaceLeftUntil(CLEARANCE_RADIUS_CENTER_TO_BACK,RIGHT);
                changeOrientation(EAST, SPEED);
                //displaceLeft();
                //displaceLeftUntil(DISTANCE_PUCKS_FROM_NORTH + WIDTH_SENSOR_TO_CENTER ,RIGHT);
                moveUntil(DISTANCE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                            
            }
            
            
            else if (blockEastClearance && puckWestClearance){

                moveUntil(block_and_puck_edge_midpoint - DISTANCE_BACK_SENSOR_FROM_CENTER, BACKWARD, LESS_THAN, BACK, SPEED, TRUE);
                straightAdjust();
                changeOrientation(NORTH, SPEED);
                straightAdjust();
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                //displaceLeft(); Repeatedly call this if below function not implemented
                //displaceLeftUntil(CLEARANCE_RADIUS_CENTER_TO_BACK,RIGHT);
                changeOrientation(EAST, SPEED);
                //displaceLeft();
                //displaceLeftUntil(DISTANCE_PUCKS_FROM_NORTH + WIDTH_SENSOR_TO_CENTER ,RIGHT);
                moveUntil(DISTANCE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                
            }
            
            else if (blockWestClearance && puckEastClearance){
                
                moveUntil(block_and_puck_edge_midpoint - DISTANCE_BACK_SENSOR_FROM_CENTER, BACKWARD, LESS_THAN, BACK, SPEED, TRUE);
                straightAdjust();
                changeOrientation(NORTH, SPEED);
                straightAdjust();
                moveUntil(CLEARANCE_RADIUS_CENTER_TO_FRONT, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                //displaceLeft(); Repeatedly call this if below function not implemented
                //displaceLeftUntil(CLEARANCE_RADIUS_CENTER_TO_BACK,RIGHT);
                changeOrientation(WEST, SPEED);
                //displaceLeft();
                //displaceLeftUntil(DISTANCE_PUCKS_FROM_NORTH + WIDTH_SENSOR_TO_CENTER ,RIGHT);
                moveUntil(DISTANCE_STOPPED_FROM_PUCK, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                
            }   
            
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
            
            
            if (heldColour == puckConstructionPlan[currentPuckStackSize] ) { // The currently held puck should go on the construction pile now
                moveUntil(HOME_MIDPOINT - DISTANCE_FRONT_SENSOR_FROM_CENTER, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);  
                changeOrientation(NORTH,SPEED);
                changeOrientation(WEST,SPEED);
                moveUntil(CONSTRUCTION_MIDPOINT - DISTANCE_FRONT_SENSOR_FROM_CENTER, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                straightAdjust();
                moveUntil(CONSTRUCTION_MIDPOINT - DISTANCE_FRONT_SENSOR_FROM_CENTER, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE); // Move to a point where we can face towards stack but able to get a distance check
                straightAdjust();
                /*
                changeOrientation(SOUTH, SPEED);
                straightAdjust();
                moveUntil(CONSTRUCTION_MIDPOINT - DISTANCE_FRONT_SENSOR_FROM_CENTER, FORWARD, LESS_THAN, FRONT_LEFT, SPEED);
                straightAdjust();
                */
                
            }
            else if ( heldColour == puckConstructionPlan[1] || heldColour == puckConstructionPlan[2]){ // Then put this puck in the home base for grabbing later
                moveUntil(ARENA_WIDTH - FIRST_TEMP_DROPOFF + DISTANCE_FRONT_SENSOR_FROM_CENTER, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE); // Check if already droppd off, go to second_temp_dropoff
                straightAdjust();
            }
            
            else {
                moveUntil(ARENA_WIDTH-FIRST_DISCARD_ZONE + DISTANCE_FRONT_SENSOR_FROM_CENTER, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE); // Check if already droppd off, go to second_temp_dropoff
            } // Discard the puck so drop it in rubbish pile

            changeOrientation(SOUTH,SPEED);    
            changeHeightToPuck(currentPuckStackSize + 1, CLOSE); // Lift claw above stack to avoid hitting the stack  
            
            if (currentPuckStackSize == 0){
               moveUntil(CONSTRUCTION_DISTANCE_FROM_WALL, FORWARD, LESS_THAN, FRONT_LEFT, SPEED,TRUE); // This function is being triggered by the stack. Only use this for non stacking part (ie. home base dropping) 
            }
            else {
                moveUntil(CONSTRUCTION_DISTANCE_FROM_WALL - 60, FORWARD, LESS_THAN, FRONT_RIGHT, SPEED, FALSE); // This function is being triggered by the stack. Only use this for non stacking part (ie. home base dropping)
            }
            
            //moveDynamic(CONSTRUCTION_DISTANCE_CLEAR_FROM_STACK - CONSTRUCTION_DISTANCE_FROM_WALL,SPEED);
            
            if (heldColour == puckConstructionPlan[currentPuckStackSize]){changeHeightToPuck(currentPuckStackSize, NEITHER);}
            else {changeHeightToPuck(0, NEITHER);}
            armOpen();
            changeHeightToPuck(currentPuckStackSize + 1, NEITHER); // Move higher than stack to avoid hitting it.
            

            if (current_stage >= 3){state = STATE_PARK_HOME;}        // Returns to home 
            else {current_stage++; currentPuckStackSize++;
            // Need to go back to the east wall facing east.
                //moveUntil(CONSTRUCTION_DISTANCE_CLEAR_FROM_STACK, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, FALSE);
                moveDynamic(-150,SPEED,FALSE);
                changeOrientation(WEST,SPEED);
                straightAdjust();
                moveUntil(HOME_MIDPOINT - DISTANCE_FRONT_SENSOR_FROM_CENTER, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);  
                straightAdjust();
                changeOrientation(SOUTH,SPEED);
                changeOrientation(EAST,SPEED);
                moveUntil(DISTANCE_FRONT_SENSOR_FROM_CENTER + SAFETY_MARGIN + 50, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                straightAdjust();
                
                
                //Usually we will check if the next colour we need is in home base already
                //
                // If in east edge of home base
                if ( puckConstructionPlan[currentPuckStackSize] == puckColoursTempPile[0]){
                    
                }
                //If in west edge of home base
                else if(puckConstructionPlan[currentPuckStackSize] == puckColoursTempPile[1]){
                         
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
            
            
            moveUntil(CONSTRUCTION_DISTANCE_CLEAR_FROM_STACK, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, FALSE);
            changeOrientation(WEST,SPEED);
            straightAdjust();
            moveUntil(HOME_MIDPOINT - DISTANCE_FRONT_SENSOR_FROM_CENTER - 30 , BACKWARD, GREATER_THAN, FRONT_RIGHT, SPEED, TRUE); // Absorb the 50 into some constant later
            moveSwivel(90, SPEED, TRUE);
            moveUntil(HOME_PARKING_DISTANCE , BACKWARD, LESS_THAN, BACK, SPEED, TRUE);

            // END Congratulations:
            changeHeightToPuck(GROUND, NEITHER);

            while(1) {} // Finish inside infinite loop
            
            // Run code to stop robot entirely.
        }
        
        
    }    
}