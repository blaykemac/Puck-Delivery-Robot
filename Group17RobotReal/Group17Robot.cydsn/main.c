
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



// * VARIABLES * //
int idac_value = 0;
int colour_flag = 1;                    // sets which photodiode to use 
extern float ultrasonic_distances[5];
int beginNavigation = 0;

int state = STATE_GO_WEST;

// Flag Initialisation


int findingPuck = 1;
int returningHome = 0;

int setup = 1;

int homeToNW = 1;
int NWToHome = 0;
int homeToNE = 0;
int NEToHome = 0;

int southWall = 1;
int westWall = 0;
int northWall = 1;
int eastWall = 0;

int running = 1;

// Driving Flags
int driveStraightEnable = 0; // Set to 1 as we can drive to begin with

int sweeping = 0;
float block_location[4] = {0,0,0,0}; // WEST, EAST, SOUTH, NORTH

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
    
    //Motor stuff initialisation		
    Motor_1_driver_Start();		
    Motor_2_driver_Start();		
    Motor_1_driver_Sleep();		
    Motor_2_driver_Sleep();		
    
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
            if (currentPuckRackScanningIndex == 0){moveBackwardIndefinitely();}
            else {moveForwardIndefinitely();}        
            
            ColourSensingViaHardware();
            // ColourOutput();
            // CyDelay(100);
            
            if (currentPuckRackScanningIndex == 4) {state = STATE_LOCATE_BLOCK;}
                     
        }

    	while (state == STATE_LOCATE_BLOCK){
            
            // Finding where the boundaries of the block are
    		// Sweep across WEST to EAST until discrepancy
            // But first sense the construction plan and then drive to wall and turn around to prepare for full width scan
            
            
    		moveForwardIndefinitely();
    		turnRight(180); // Now facing EAST wall
    		moveForwardIndefinitely(); // Do this until we get to EAST wall
            
            // Will now know the boundaries of the block.

	}
        
        // Now we need to locate the pucks
        
        state = STATE_FIND_PUCKS;
        
        
        while (state == STATE_FIND_PUCKS) {
            // We are at the EAST wall facing the EAST wall
            turnRight(180); // Now facing WEST at EAST wall
            moveForwardIndefinitely();
        }
        
        // * Navigation Loop * //
        
                
        while(running) {
            distanceCheck(); // This seems to be a quick fix to the first values recorded upon bootup being wacky.
            distanceCheck();
            
            
            state = STATE_GO_NORTH_CARELESS; 
            driveStraightEnable = 1;
            moveForwardIndefinitely();
            turnRight(94); // CHANGE TO 90 WHEN THE TURN FUNCTION IS ACCURATE. WE OVER COMPENSATE TEMPORARILY
            // Now facing east in the NW corner
            
            // state = STATE_SWEEP_N_PUCKS;
            
            driveStraightEnable = 1;
            state = STATE_FORWARD_TO_PUCK; 
            
            armDown();
            armOpen();
            
            moveForwardIndefinitely();

            moveForward(8);
            armClose();
            armUp();
                    
                    
            // Code for finding pucks along N wall and navigating towards the puck
            
            state = STATE_RETURN_TO_NW_FROM_PUCK;
            driveStraightEnable = 1;
            moveBackwardIndefinitely();
            turnLeft(87);
            
            state = STATE_RETURN_HOME_SIDE;
            driveStraightEnable = 1;
            moveBackwardIndefinitely();
            turnRight(94);
            
            state = STATE_LAND_ON_HOME_BASE;
            driveStraightEnable = 1;
            moveForwardIndefinitely();
            //displaceRight(ultrasonic_distances[RIGHT_SIDE] - 20,30);
            
            distanceCheck();
            
            while (ultrasonic_distances[RIGHT_SIDE] > 5){
                  
                displaceRight(3,10);
                turnRight(3);
                distanceCheck();
            
            }
            
            state = STATE_LAND_ON_HOME_BASE;
            driveStraightEnable = 1;
            moveForwardIndefinitely();
            
            state = STATE_FINISH_LANDING;
            driveStraightEnable = 1;
            moveBackwardIndefinitely();
            
           // state = STATE_LAND_ON_HOME_BASE;
           // driveStraightEnable = 1;
            //moveForwardIndefinitely();
            
            beginNavigation = 0;
            running = 0;
            
        }
    
    }
        
}
