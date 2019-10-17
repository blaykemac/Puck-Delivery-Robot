
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


extern char output[32];   

// * STATE VARIABLES * //
int state = STATE_SCAN_PLAN;
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

float horizontalPuckGrid = 0;

// * NAVIGATION AND POSITION VARIABLES * //

int beginNavigation = 0; // Allow us to break out of the intial phase when powered up

int pathToPucks; // This will give us a corridor that we should initially take when trying to go to the pucks
int pathPastBlock;

float currentPosition[2] = {0,0};
float desiredPosition[2];
int currentOrientation = 90; //in degrees (convert to radians when needed)- 90 assuming we start facing north
int desiredOrientation;

short int moveNow = 1; //This is a flag that lets the main program tell the moving functions
//whethe we want the robot to be moving or not. For example, when we need to operate servos
//the main program would set moveNow to FALSE.

//The four flags below let the rest of the program know if the robot is trying to drive 
//forward or back, turn left or right. At the start of every motion, turn these flags on
//at the end turn them off.
short int drivingForwardFlag = 0;
short int drivingBackwardFlag = 0;
short int turningLeftFlag = 0;
short int turningRightFlag = 0;

//These flags will be used by the main program to allow the robot to move in certain directions
short int moveLeftAllowed;
short int moveRightAllowed;
short int moveForwardAllowed;
short int moveBackwardAllowed;


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
    
    beginNavigation = 1;
    colour_flag = 0; 
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
    int colour_calibration = FALSE;             // Do we want to calibrate the sensor? 
    int servo_testing = FALSE;                  // Do the servos need to be tested?      
    int motor_testing = FALSE;
    int ultrasonic_testing = TRUE;  
    
    
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
    
    // Misha Motors Initialisation:
    Motor_Left_Driver_Start();
    Motor_Left_Driver_Sleep();                  // Puts motor to sleep
    Motor_Right_Driver_Start();
    Motor_Right_Driver_Sleep();                 // Puts motor to sleep
    
    Motor_Left_Decoder_Start();
    Motor_Right_Decoder_Start();
    
    // Ultrasonic Initialisation: 
    
        
    // Timer and ISR instantiation
    Timer_1_Start();
    Timer_1_ReadStatusRegister();
    Sonic_StartEx(TIH);
    Start_StartEx(StartIH);
    beginNavigation = 0;
    
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

    // Main Loop for States
        
    for(;;)
    {   
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

        if (state == STATE_SCAN_PLAN) {              // colour sensing, while switch has not been pushed. change to if eventually
            
            // Colour Calibration: 
            if (colour_calibration)
            {
                int lock = FALSE;   
                do 
                    {   
                        if (lock == FALSE) 
                        {
                            
                            control_photodiode_Write(1);    // Claw
                            changeHeightToPuck(0);

                            UART_1_PutString("Count \t BLANK \t RED \t GREEN \t BLUE \t \n");
                            for (int i = 0; i < 50; i++)
                            {
                            sprintf(output, "%d \t", i);
                            UART_1_PutString(output);
                            colourSensingCalibration(); 
                            UART_1_PutString("\n");
                            }         
                            lock = TRUE;    
                        }
                } while (lock == TRUE);    
            }
            
            // Servo Testing:
            if (servo_testing) 
            {
                int lock = FALSE;
                do 
                    {   
                        if (lock == FALSE) 
                        {
                            UART_1_PutString("Servo Testing: ");
                            
                            armMoving();
                            
                            
                            for(int i = 0; i < 4; i++)
                            {
                                changeHeightToPuck(i);
                                CyDelay(1000);
                            }

                            lock = TRUE;    
                        }
                } while (lock == TRUE);    
            }    
                
            // Motor Testing: 
            if (motor_testing)
            {
                int lock = FALSE;
                do 
                    {   
                        if (lock == FALSE) 
                        {
                            UART_1_PutString("Motor Testing: \n");

                            //mishaMoveForward();
                            //mishaMoveBackward();
                            //mishaMoveDynamic(-300);
                            
                            mishaSwivel(-180);
                            
                            lock = TRUE;    
                        }
                } while (lock == TRUE);    
            }    
            
            // Ultrasonic Testing:
            
            if (ultrasonic_testing) 
            {
                int lock = FALSE;
                do 
                    {   
                        if (lock == FALSE) 
                        {
                            UART_1_PutString("Ultrasonic Testing: \n");
                            
                            //armMoving();
                            //straightAdjust();
                            
                            int dick = 2;
                            while(0) {
                                distanceSensor(dick);
                                CyDelay(500);
                                sprintf(output, "%d \t %d \t %d \t %d \t %d \n", ultrasonic_distances_mm[0], 
                                                         ultrasonic_distances_mm[1],
                                                         ultrasonic_distances_mm[2],
                                                            ultrasonic_distances_mm[3],
                                                            ultrasonic_distances_mm[4]);
                                UART_1_PutString(output);
                            }
                            // 0 gave front left    // front left
                            // 1 gave front right   // front right 
                            // 2 gave back sensor   // side left now??? 
                            // 3 gave side right    // side right 
                            // 4 gave back sensor   // back sensor
                            
                            while(0) {
                                for (int i = 0; i < 5; i++)
                                {
                                    distanceSensor(i);
                                    CyDelay(100);
                                    sprintf(output, "%d \t", ultrasonic_distances_mm[i]);
                                    UART_1_PutString(output);
                                }
                                UART_1_PutString("\n");
                            }
                            
                            
                            
                            
                            // TEST FIRING
                            for (int i = 0; i < 5; i++)
                                {
                                    distanceSensor(i);
                                    CyDelay(100);
                                    sprintf(output, "%d \t", ultrasonic_distances_mm[i]);
                                    UART_1_PutString(output);
                                }
                            UART_1_PutString("\n");
                            
                            
                            moveUntil(-150);              // should keep moving until we approach 50mm wall
                            straightAdjust();
                            
                            
                            /*
                            for (int i = 0; i < 5; i++)
                            {
                                sprintf(output, "%d: \t", i);       
                                UART_1_PutString(output);
                            }
                            UART_1_PutString("\n");   
                            
                            
                            for (int i = 0; i < 50; i++) {
                            distanceCheck();
                            CyDelay(200);
                            }
                            
                            */
                            
                            //straightAdjust();
                            
                            
                            lock = TRUE;    
                        }
                } while (lock == TRUE);    
            }    
            
                       
            // MOVEMENT TESTING:
            
            //mishaMoveDynamic(850);
            //mishaMoveDynamic(-850);
            
            mishaMoveDynamic(230);
            while(1) {};
            
            
            
            mishaMoveDynamic(-300);
            CyDelay(500);
            colourSensingInitialise();      
            CyDelay(500);
            mishaMoveDynamic(120);
            CyDelay(500);
            
            for (int i = 0; i < 5; i++) {
                puckRackColours[i] = colourSensingOutput();
                CyDelay(500);
                mishaMoveDynamic(61);
                CyDelay(500);
            }
                       
            UART_1_PutString("Found Colours: \n");
            for (int i = 0; i < 5; i++) 
            {
                sprintf(output, "%i \n", puckRackColours[i]);       
                UART_1_PutString(output);
            }
            
            
            control_photodiode_Write(1);    // changes to claw photodiode
            colour_sensing_algorithm = 1;   // changes it to claw algorithm
                       
            //while(1) {}
            
            // THEN WE FILTER THE NO PUCK VALUES:
            
            
            
            for (int i = 0; i < 5; i++){
                if (puckRackColours[i] != BLANK && i != 3){
                    puckConstructionPlan[i] = puckRackColours[i];
                    i++;
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

            
            
            int puckConstructionPlanIndex = 0; // Will be used to iterate through the 3 puck colours in construction plan
            
            // Iterate over all 5 rack slots and place the 3 colours into puckConstructionPlan.
            for (int puckRackIndex = 0; puckRackIndex < 5; puckRackIndex++){
                if (puckRackColours[puckRackIndex] != BLANK && puckConstructionPlanIndex != 3){
                    puckConstructionPlan[puckConstructionPlanIndex] = puckRackColours[puckRackIndex];
                    puckConstructionPlanIndex++;
                }
            }
            

            */
        }
        
        

    	if (state == STATE_LOCATE_BLOCK_AND_PUCKS){
            
            mishaSwivel(-45);  
            mishaMoveDynamic(-150);
            mishaSwivel(45);
            mishaMoveDynamic(200);
            mishaSwivel(90);
            mishaMoveDynamic(500);
            
            
            
            
            
            
            // Finding where the boundaries of the block are
    		// Sweep across WEST to EAST until discrepancy
            // But first sense the construction plan and then drive to wall and turn around to prepare for full width scan
            
            /*
    		moveForwardIndefinitely();
    		turnRight(180); // Now facing EAST wall
    		moveForwardIndefinitely(); // Do this until we get to EAST wall
            */
            
            //moveAndAngle(20,20,EAST_ANGLE); // Move to a position near construction base in an EAST orientation to be ready to scan for the pucks and block
            //moveForwardIndefinitely(); // Scan until we reach EAST wall.
            
            // Will now know the boundaries of the block.
            // Figure out direction we want to travel
            

            //pathPastBlock = (block_edge_location[WEST] >= WIDTH_SENSOR_TO_SENSOR + SAFETY_MARGIN ? WEST:EAST);
            //pathToPucks = (puckPileLocation < ARENA_WIDTH / 2 ? WEST:EAST);
            
            state = STATE_GO_TO_PUCKS;
	    }
        
        

        if (state == STATE_GO_TO_PUCKS){
            
            /*
            if (pathPastBlock == WEST){
                if (pathToPucks != WEST) {
                    moveAndAngle(SAFETY_MARGIN / 2 + WIDTH_SENSOR_TO_SENSOR / 2, ARENA_LENGTH - FRONT_CLAW_DISTANCE_FROM_CENTRE - SAFETY_MARGIN, EAST_ANGLE); // Take us to NW corner and then face EAST in preparation for finding the pucks
                }
                else {moveAndAngle(SAFETY_MARGIN / 2 + WIDTH_SENSOR_TO_SENSOR / 2, ARENA_LENGTH - FRONT_CLAW_DISTANCE_FROM_CENTRE - SAFETY_MARGIN - DISTANCE_PUCKS_FROM_NORTH, NORTH_ANGLE);} // Take us right up to the pucks in NW corner
            }
            else {
                if (pathToPucks != EAST) {
                    moveAndAngle(ARENA_WIDTH - SAFETY_MARGIN / 2 - WIDTH_SENSOR_TO_SENSOR / 2, ARENA_LENGTH - FRONT_CLAW_DISTANCE_FROM_CENTRE - SAFETY_MARGIN, WEST_ANGLE); // Take us to NE corner and then face WEST in preparation for finding the pucks
                }
                else {moveAndAngle(ARENA_WIDTH - SAFETY_MARGIN / 2 - WIDTH_SENSOR_TO_SENSOR / 2, ARENA_LENGTH - FRONT_CLAW_DISTANCE_FROM_CENTRE - SAFETY_MARGIN - DISTANCE_PUCKS_FROM_NORTH, NORTH_ANGLE);} // Take us right up to the pucks in NE corner
            }
            
            // Now depending on if the pucks are in the corner or not, 
            
            // We are now in front of the pucks
            */
            
            state = STATE_FIND_REQUIRED_PUCK;
        }
        
        
        
        if (state == STATE_FIND_REQUIRED_PUCK)
        {
            
            
        SPEED = 70;    
        int puck_correct = FALSE;   // A flag to determine if the correct puck has been picked up
        int puck_scan;
        
    
        puckConstructionPlan[1] = RED;
    
        // Scanning puck:

        while (puck_correct == FALSE) 
        {
            mishaMoveDynamic(50);  // robot moves forward towards puck
                                    // could replace this with the distance gathered from the ultrasonic 
            changeHeightToPuck(1);  // arm lowers onto robot
            puck_scan = colourSensingOutput();  // colour sensor takes a scan
            changeHeightToPuck(3);  // arm returns to high position
            mishaMoveDynamic(-50);   // robot moves backwards
            if (puck_scan == puckConstructionPlan[1]) {puck_correct = TRUE;}
                                    // if colour == true:
                
            else 
            {
                                    // if colour == false
                                        // robot translates to side to see next puck, and performs scanning puck again
            }
        }
                
        // Picking Up puck from pile:
        changeHeightToPuck(0);      // arm lowers to ground
        //changeHeightToPuck(1);
        moveUntilPuck();
        //mishaMoveDynamic(60);       // robot moves forward
        armClose();                 // claw closes on puck
        changeHeightToPuck(3);      // arm lifts up to highest position
        mishaMoveDynamic(-60);       // robot moves back away from puck area 
        
            
        while(1) {};           
            
            /*
            int requiredColour = puckConstructionPlan[currentPuckStackSize];
            int puckFound = 0;
            
            
            // Let's assume that we get to the east or west of the puck pile and we are as close to the SW/SE puck as possible.
            if (roundDirection() == NORTH){

            }
            else if (roundDirection() == EAST){
                // Poll ultrasonics, displaceRight until no longer see the pucks
                // Drive up to the middle puck
                // We know that we used moveAndAngle() to get to the exact vertical position of the south-most puck. Therefore we will drive forward until we are close to the puck
                moveForwardIndefinitely();
                
                // May need to displaceRight a bit to ensure that we are perfectly centered with the south-most puck
                // Now we need to inch forward with the claw lifted
                // Also store the horizontal location of the pucks based off our current position. This allows us to return for the other 2 pucks easily.
                
                horizontalPuckGrid = currentPosition[0]; // Take our horizontal position as a reference so we can use moveAndAngle(horizontalPuck,..,..) to return to the same column we took the first puck from.
                
                changeHeightToPuck(1); // Lift up enough so we dont hit the pucks when scanning the colour.
                moveForward(DISTANCE_STOPPED_FROM_PUCK); // Move forward until colour sensor above the unknown coloured puck in question.
               
                if (colourSensingOutput() == puckConstructionPlan[currentPuckStackSize]){ 
                    // This is the puck we need
                    moveBackward(DISTANCE_STOPPED_FROM_PUCK); // Reverse to make room for lowering the claw
                    lowerAndOpen(0); // We are grabbing the pucks from the ground.
                    moveForward(DISTANCE_STOPPED_FROM_PUCK);
                    closeAndRaise(1); // Lift above the height of the ground pucks

                }
                
                // If the colour isn't what we need, we could pick it up and drop it somewhere out of the way (for round 2) or else we just ignore it.
                else {
                    moveBackward(DISTANCE_STOPPED_FROM_PUCK); // Need to check the next puck
                    displaceLeft(5,22);
                    changeHeightToPuck(1); // Lift up enough so we dont hit the pucks when scanning the colour.
                    moveForward(DISTANCE_STOPPED_FROM_PUCK); // Move forward until colour sensor above the unknown coloured puck in question.
                    
                    // Scan the next puck
                    if (colourSensingOutput() == puckConstructionPlan[currentPuckStackSize]){ 
                        // This is the puck we need
                        moveBackward(DISTANCE_STOPPED_FROM_PUCK); // Reverse to make room for lowering the claw
                        lowerAndOpen(0); // We are grabbing the pucks from the ground.
                        moveForward(DISTANCE_STOPPED_FROM_PUCK);
                        closeAndRaise(1); // Lift above the height of the ground pucks

                    }
                    
                    else {
                        moveBackward(DISTANCE_STOPPED_FROM_PUCK); // Need to check the next puck
                        displaceLeft(5,22);
                        changeHeightToPuck(1); // Lift up enough so we dont hit the pucks when scanning the colour.
                        moveForward(DISTANCE_STOPPED_FROM_PUCK); // Move forward until colour sensor above the unknown coloured puck in question.
                        
                        if (colourSensingOutput() == puckConstructionPlan[currentPuckStackSize]){ 
                            // This is the puck we need
                            moveBackward(DISTANCE_STOPPED_FROM_PUCK); // Reverse to make room for lowering the claw
                            lowerAndOpen(0); // We are grabbing the pucks from the ground.
                            moveForward(DISTANCE_STOPPED_FROM_PUCK);
                            closeAndRaise(1); // Lift above the height of the ground pucks

                        }
                    
                    }
                }
                
                if (colourSensingOutput() == puckConstructionPlan[currentPuckStackSize]){ 
                    // This is the puck we need
                    moveBackward(DISTANCE_STOPPED_FROM_PUCK); // Reverse to make room for lowering the claw
                    lowerAndOpen(0); // We are grabbing the pucks from the ground.
                    moveForward(DISTANCE_STOPPED_FROM_PUCK);
                    closeAndRaise(1); // Lift above the height of the ground pucks

                }
                
            }
            
            else if (roundDirection() == WEST){
                // Poll ultrasonics, displaceRight until no longer see the pucks
                // Drive up to the middle puck
                // We know that we used moveAndAngle() to get to the exact vertical position of the south-most puck. Therefore we will drive forward until we are close to the puck
                moveForwardIndefinitely();
            }
            
            */
            
            /*
            while(!puckFound){
                  
            }
            */
            
            
        }
        
        if (state == STATE_DEPOSIT_PUCK){

            /*
            moveAndAngle(CONSTRUCTION_MIDPOINT,CONSTRUCTION_DISTANCE_FROM_WALL + 10, SOUTH_ANGLE); // Take us to the drop off point NEAR construction zone
            lowerAndOpen(currentPuckStackSize);
            moveAndAngle(CONSTRUCTION_MIDPOINT,CONSTRUCTION_DISTANCE_FROM_WALL, SOUTH_ANGLE); // Take us to the drop off point in construction zone
            changeHeightToPuck(currentPuckStackSize + 1); // Lift claw above stack to avoid hitting the stack
            moveAndAngle(CONSTRUCTION_MIDPOINT,CONSTRUCTION_DISTANCE_FROM_WALL, SOUTH_ANGLE); // Take us to the drop off point in construction zone

            
            lowerAndOpen(current_stage);
            changeHeightToPuck(current_stage + 1); // Lift claw above stack to avoid hitting the stack
            
            current_stage++;
            
            if (current_stage == 3){state = STATE_PARK_HOME;}        // Returns to home 
            */
            
        }
        
        if (state == STATE_PARK_HOME){
            
            //moveAndAngle(HOME_MIDPOINT, HOME_PARKING_DISTANCE, NORTH_ANGLE); // Should reverse into the spot to fit properly
            
            // Run code to stop robot entirely.
        }
        
        
    }    
}

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
    
    mishaMoveDynamic(30);
    
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
    
    
    int speed_left = 30;        // slow speed
    int speed_right = 30;
    
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


void moveUntil(int distance_set) {
    // distance_set is in millimeteres
    // will check the front two ultrasonics 
    // will move until distance specified in function 
    
    int count_left;                 // counts the encoder values
    int count_right;
    int speed_left = SPEED;         // the set speed of the motors
    int speed_right = SPEED;
    int compare;                    
    int ultrasonic_sensor;          // the ultrasonic sensor we will be using 
    int distance_sensor;            // the measured distance of the sensor
    
    
    
    
    // The distance is in millimetres 
    // Is the distance negative or positive? 
    if (distance_set > 0) { 
        Motor_Left_Control_Write(0); 
        Motor_Right_Control_Write(0); 
        ultrasonic_sensor = FRONT_LEFT;       // use the back ultrasonic to test our distance
                                        // THIS CHECKS THE FRONT LEFT SENSOR, for some reason?
        compare = 32000;                // max compare value
    }
    else {
        Motor_Left_Control_Write(1); 
        Motor_Right_Control_Write(1);
        ultrasonic_sensor = BACK; // We just use one ultrasonic front sensor
        compare = -32000;               // min compare value
    }
    
    Motor_Left_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(speed_left);
    Motor_Right_Driver_Wakeup();
    Motor_Right_Driver_WriteCompare(speed_right);
    
    count_left = Motor_Left_Decoder_GetCounter();       
    count_right = Motor_Right_Decoder_GetCounter();     
    //distanceCheckOne(ultrasonic_sensor);                            // checks the distance
    distanceSensor(ultrasonic_sensor);
    CyDelay(50);
    distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
    
    while (distance_sensor > abs(distance_set)) {                // This adjusts for drift 
        count_left = Motor_Left_Decoder_GetCounter();
        count_right = Motor_Right_Decoder_GetCounter();
        if (count_left > count_right) {
        speed_left -= 2;
        }
        if (count_right > count_left) {
        speed_right -=2; 
        }
        distanceSensor(ultrasonic_sensor);
        CyDelay(50);
        //distanceCheckOne(ultrasonic_sensor);                            // checks the distance
        distance_sensor = ultrasonic_distances_mm[ultrasonic_sensor];   // checks the distance measured by the ultrasonic
        sprintf(output, "%d \t", distance_sensor);       
        UART_1_PutString(output);
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



