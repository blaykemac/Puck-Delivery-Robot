
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



//global variable currentPosition and currentOrientation (in main). Declare as externs in 
//dcmotor, I guess
float currentPosition[2] = {0,0};
float desiredPosition[2];
int currentOrientation = 90; //in degrees (convert to radians when needed)- 90 assuming we start facing north
int desiredOrientation;
int M1_FD; //The four values shown here will be the duty cycles of the motors
int M1_BD; //There are times when parts of the code (such as the driftCorrect function)
int M2_FD; //need to know about the duty cycles. So we make the duty cycles a globally 
int M2_BD; //known variable
short int motor1Enable = 0; //These two will be on if the motors are on
short int motor2Enable = 0;
int M1_FD; //The four values shown here will be the duty cycles of the motors
int M1_BD; //There are times when parts of the code (such as the driftCorrect function)
int M2_FD; //need to know about the duty cycles. So we make the duty cycles a globally 
int M2_BD; //known variable
int motor1EncoderCounts; //These two variables will allow the entire program to keep 
int motor2EncoderCounts; //track of how many turns the motors have spun 
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

// * VARIABLES * //
int idac_value = 0;
int colour_flag = 1;                    // sets which photodiode to use 
extern float ultrasonic_distances[5];
int beginNavigation = 0;

int state = STATE_SCAN_PLAN;

int running = 1;

int sweeping = 0;

int pathToPucks;
int pathPastBlock;

float block_edge_location[4] = {0,0,0,0}; // N E S W

int currentPuckStackSize = 0;

// Puck Construction Scanning
int puckRackColours[5] = {0,0,0,0,0}; // 5 slots in puck rack.
int currentPuckRackScanningIndex = 0;
int puckRackOffsetsFromWest[5] = {PUCK_RACK_0_WEST_DISTANCE,
                                PUCK_RACK_1_WEST_DISTANCE,
                                PUCK_RACK_2_WEST_DISTANCE,
                                PUCK_RACK_3_WEST_DISTANCE,
                                PUCK_RACK_4_WEST_DISTANCE };


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
    
    // Colour Sensing Initialisation:
    PWM_colour_Start();                         // Starts the PWM
    PWM_colour_Sleep();                         // Puts it to sleep until we need it
    //RGB1Initialise();                         // Initialise 2nd RGB
    //RGB2Initialise();                         // Initialise 2nd RGB
    ColourIntialiseViaHardware();               // Initialises via hardware 
    
    // Ultrasonic Initialisation: 
    
        
    // Timer and ISR instantiation
    Timer_1_Start();
    Timer_1_ReadStatusRegister();
    Sonic_StartEx(TIH);
    Start_StartEx(StartIH);
    beginNavigation = 0;
    int tmp = 0;
    
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
        

        while (state == STATE_SCAN_PLAN) {              // colour sensing, while switch has not been pushed 
            
            
            ColourIntialiseViaHardware();
            
            
            
            /*
            if (currentPuckRackScanningIndex == 0){moveBackwardIndefinitely();}
            else {moveForwardIndefinitely();}        
            
            puckRackColours[currentPuckRackScanningIndex] = ColourSensingOutput();

            
            if (currentPuckRackScanningIndex == 4) {state = STATE_LOCATE_BLOCK;}
            */       
        }

    	if (state == STATE_LOCATE_BLOCK){
            
            // Finding where the boundaries of the block are
    		// Sweep across WEST to EAST until discrepancy
            // But first sense the construction plan and then drive to wall and turn around to prepare for full width scan
            
            
    		moveForwardIndefinitely();
    		turnRight(180); // Now facing EAST wall
    		moveForwardIndefinitely(); // Do this until we get to EAST wall
            
            
            // Will now know the boundaries of the block.
            // Figure out direction we want to travel
            
            if (block_edge_location[WEST] >= WIDTH_SENSOR_TO_SENSOR + SAFETY_MARGIN){
                pathPastBlock = WEST;
            }
            else {pathPastBlock = EAST;}
            
            // Calculate puck position here too, then make a decision as to whether
            // pathToPucks = ... ;

	}
        
        // Now we need to locate the pucks
 
        if (state == STATE_FIND_PUCKS) {
            // We are at the EAST wall facing the EAST wall
            turnRight(180); // Now facing WEST at EAST wall
            moveForwardIndefinitely();
        }
        
        state = STATE_GO_TO_PUCKS;

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
        
        }
        
        state = STATE_FIND_REQUIRED_PUCK;
        
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


