
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

// * OUR LIBRARIES * //
#include "main.h"
#include "colour.h"
#include "ultrasonic.h" 
#include "dcmotor.h"
#include "servo.h"
#include "customMath.h"


// * STATE VARIABLES * //
int state = STATE_SCAN_PLAN;
int running = 1;
int sweeping = 0;
float block_edge_location[4] = {0,0,0,0}; // N E S W edge positions respectively

// * PUCK CONSTRUCTION VARIABLES * //
int currentPuckStackSize = 0; // How many pucks are currently on the construction plate
int puckRackColours[5] = {0,0,0,0,0}; // 5 slots in puck rack.
int currentPuckRackScanningIndex = 0;
int puckRackOffsetsFromWest[5] = {PUCK_RACK_0_WEST_DISTANCE,
                                PUCK_RACK_1_WEST_DISTANCE,
                                PUCK_RACK_2_WEST_DISTANCE,
                                PUCK_RACK_3_WEST_DISTANCE,
                                PUCK_RACK_4_WEST_DISTANCE };

// * NAVIGATION AND POSITION VARIABLES * //

int beginNavigation = 0; // Allow us to break out of the intial phase when powered up

int pathToPucks; // This will give us a corridor that we should initially take when trying to go to the pucks
int pathPastBlock;

float currentPosition[2] = {0,0};
float desiredPosition[2];
int currentOrientation = 90; //in degrees (convert to radians when needed)- 90 assuming we start facing north
int desiredOrientation;

short int moveNow = 1; //This is a flag that lets the main program tell the moving functions
//whether we want the robot to be moving or not. For example, when we need to operate servos
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

int calibrate = FALSE;                    // Do we want to calibrate the sensor? 
int idac_value = 0;  
int colour_flag = 1;                    // sets which photodiode to use 
int ColourSensingAlgorithm = 0;      // Determines which colour sensing algorithm to use:
                                                // 0: Wall algorithm
                                            // 1: Claw algorithm
                                                // 2: old algorithm





int moving = 0; // Temp to stop while loop from repeated runs of scanning plan code

const int PLAN_SCAN_VERTICAL = 10; // Set to ultrasonic distance at the home base 


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


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    TIA_1_Start();
    UART_1_Start();
    UART_1_PutString("UART commencing... \n");
    ADC_Start();
    IDAC8_1_Start();
    
    // IDAC initialisation: 
    IDAC8_1_SetRange(IDAC8_1_RANGE_32uA);       // Sets the range between 0 and 32uA
    IDAC8_1_SetValue(idac_value);               // set a value between 0 and 255
    
     // Colour Sensing Initialisation & Debugging:
      
    
    // Ultrasonic Initialisation: 
    
        
    // Timer and ISR instantiation
    Timer_1_Start();
    Timer_1_ReadStatusRegister();
    Sonic_StartEx(TIH);
    Start_StartEx(StartIH);
    beginNavigation = 0;
    
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
    
    //Servos for the arm initialisation
    //Gripper_Servo_PWM_WriteCompare(1250); //This is the closed gripper pos
    //Rack_Servo_PWM_WriteCompare(1000); //This is the up arm pos
   
    Rack_Servo_PWM_Start();
    Gripper_Servo_PWM_Start();
    

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
        
        
        
            control_led_Write(1);   CyDelay(1000);  // Ensures all the RGBs are working
            control_led_Write(2);   CyDelay(1000);
            control_led_Write(3);   CyDelay(1000);
            control_led_Write(0);   CyDelay(500);
            ColourSensingInitialise();                  // Initialises the colour sensor
        

            
        if (state == STATE_SCAN_PLAN) {              // colour sensing, while switch has not been pushed. change to if eventually
            
            
            
            if (calibrate)
            {
                int lock = FALSE;
                do 
                    {   
                        if (lock == FALSE) 
                        {
                            UART_1_PutString("Count \t BLANK \t RED \t GREEN \t BLUE \t \n");
                            for (int i = 0; i < 50; i++)
                            {
                            sprintf(output, "%d \t", i);
                            UART_1_PutString(output);
                            ColourSensingCalibration(); 
                            UART_1_PutString("\n");
                            }         
                        lock = TRUE;    
                        }
                } while (lock == TRUE);    
            }
            
            //ColourSensingDebug();
            
            /*
            if (!moving){
                moving = 1;
            if (initialisation){moveBackwardIndefinitely(); initialisation = 0;}
            else {moveForwardIndefinitely();}        
            
            */
                     
            moveAndAngle(SCAN_INITIALISE_HORIZONTAL,PLAN_SCAN_VERTICAL,WEST_ANGLE); // Move beyond the puck rack to scan the black empty wall
            
            for (currentPuckRackScanningIndex = 0; currentPuckRackScanningIndex <= 4; currentPuckRackScanningIndex++){
                moveAndAngle(puckRackOffsetsFromWest[currentPuckRackScanningIndex],PLAN_SCAN_VERTICAL,WEST_ANGLE); // Choose the plan vertical to be whatever Y value we start at
                puckRackColours[currentPuckRackScanningIndex] = colourSensingOutput();
            }
        }

    	if (state == STATE_LOCATE_BLOCK_AND_PUCKS){
            
            // Finding where the boundaries of the block are
    		// Sweep across WEST to EAST until discrepancy
            // But first sense the construction plan and then drive to wall and turn around to prepare for full width scan
            
            /*
    		moveForwardIndefinitely();
    		turnRight(180); // Now facing EAST wall
    		moveForwardIndefinitely(); // Do this until we get to EAST wall
            */
            
            moveAndAngle(20,20,EAST_ANGLE); // Move to a position near construction base in an EAST orientation to be ready to scan for the pucks and block
            
            // Will now know the boundaries of the block.
            // Figure out direction we want to travel
            
            if (block_edge_location[WEST] >= WIDTH_SENSOR_TO_SENSOR + SAFETY_MARGIN){
                pathPastBlock = WEST;
            }
            else {pathPastBlock = EAST;}
            
            // Calculate puck position here too, then make a decision as to whether
            // pathToPucks = ... ;

            state = STATE_GO_TO_PUCKS;
	}
        
        

        if (state == STATE_GO_TO_PUCKS){
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
            
            // We are now in front of the pucks
            state = STATE_FIND_REQUIRED_PUCK;
        }
        
        
        
        if (state == STATE_FIND_REQUIRED_PUCK){
               
        }
        
        if (state == STATE_DEPOSIT_PUCK){
            moveAndAngle(CONSTRUCTION_MIDPOINT,CONSTRUCTION_DISTANCE_FROM_WALL, SOUTH_ANGLE); // Take us to the drop off point in construction zone
            lowerAndOpen(currentPuckStackSize);
            changeHeightToPuck(currentPuckStackSize + 1); // Lift claw above stack to avoid hitting the stack
            
            currentPuckStackSize++;
            
            if (currentPuckStackSize == 3){state = STATE_PARK_HOME;}
            
        }
        
        if (state == STATE_PARK_HOME){
            moveAndAngle(HOME_MIDPOINT, HOME_PARKING_DISTANCE, NORTH_ANGLE); // Should reverse into the spot to fit properly
            
            // Run code to stop robot entirely.
        }
        
        
    }
}

