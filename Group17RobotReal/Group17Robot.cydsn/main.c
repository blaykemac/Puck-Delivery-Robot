
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


//Constants in main for dcmotor function
const short TRUE = 1;
const short FALSE = 0;
const int EAST_DIRECTION = 0; //This is just to remind us that we have our east as zero and
//angle increases counterclockwise (so toward the north of the arena will be 90)
const float CM_PER_COUNT = 0.0187; //0.0175*1.06849 cm per encoder count
const float DEGREES_PER_COUNT = 0.105; //11.465 cm per deg*0.0187 cm per count (equal to CM_PER_COUNT)
const short DRIFT_CORRECT_FREQ = 10;
const int DCMOTOR_PWM_PERIOD = 300;
const int DCMOTOR_IDEAL_DUTY_CYCLE = 80;
const int NUM_TO_PERCENT_CONVERTER = 100;
const short DRIFT_ADJUST_THRESHOLD = 5; //If the timer counts discrepancy between the timers is greater than this, we adjust
const int ENCODER_COUNTS_MAX = 65500; //The maximum number of counts the encoder can have
const int COLLISION_THRESHOLD = 5; //Distance in cm from any obstacle that we should stop

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
float block_edge_location[4] = {0,0,0,0}; // N E S W

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
            if (currentPuckRackScanningIndex == 0){moveBackwardIndefinitely();}
            else {moveForwardIndefinitely();}        
            
            ColourSensingViaHardware();
            // ColourOutput();
            // CyDelay(100);
            
            if (currentPuckRackScanningIndex == 4) {state = STATE_LOCATE_BLOCK;}
                     
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
                
            }
            
            

	}
        
        // Now we need to locate the pucks
 
        if (state == STATE_FIND_PUCKS) {
            // We are at the EAST wall facing the EAST wall
            turnRight(180); // Now facing WEST at EAST wall
            moveForwardIndefinitely();
        }

        if (state == STATE_GO_TO_PUCKS){
            
        }
        
        
        
            // Sequence of movements required for picking up a puck
            driveStraightEnable = 1;
            
        //state = STATE_FORWARD_TO_PUCK; 
            
        
        
            armDown();
            armOpen();
            
            moveForwardIndefinitely();

            moveForward(8);
            armClose();
            armUp();
    
    }          
}
