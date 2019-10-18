
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


//These are the values that correspond to open, closed, up and down positions of the 
//arm. These were found by experiment, so if need to recalibrate, you know where to go\


// Different positions of the arm:
    // Ground
    // 1 puck above
    // 2 pucks above
    // 3 pucks above
    // 4 pucks above
//  When the puck is dropped off at the construction zone, the claw will lift up to the
//      next puck level before it moves away 

// Notes:
    // values written to the rack are always divisible by 10, in order to comply with the code written,
    // E.g.: 100,1000,1500,2300, etc.

#define LOW_POS_ARM_CMP 2400 
#define HIGH_POS_ARM_CMP 1000

# define ARM_POS_GROUND 2400    // Arm positioned at the ground
# define ARM_POS_1 1700              // Arm positioned 1 pucks length above the ground (plus some tolerance)
# define ARM_POS_2 1250           // Arm positioned 2...
# define ARM_POS_3

#define GRIPPER_OPEN 1120 //1170
#define GRIPPER_CLOSED 1250


void armOpen(){
    Gripper_Servo_PWM_Wakeup();                    // Brings the servo back from sleep
    Gripper_Servo_PWM_WriteCompare(GRIPPER_OPEN); //1170 turns out to be vibrationless open val
    CyDelay(500);
    Gripper_Servo_PWM_Sleep();                    // Puts Servo back to sleep
}
void armClose(){
    Gripper_Servo_PWM_Wakeup();                    // Brings the servo back from sleep
    Gripper_Servo_PWM_WriteCompare(GRIPPER_CLOSED); //1250 turns out to be a decent closed val
    CyDelay(500);
    Gripper_Servo_PWM_Sleep();                    // Puts Servo back to sleep
}
void armTranslate(int new_position){
    
    if (new_position > 2400 | new_position < 1000) {return;}    // if a value is inputed that is bad for the servo
                                                                // function won't run
    
    Rack_Servo_PWM_Wakeup();                    // Brings the servo back from sleep
    
    int current_position = Rack_Servo_PWM_ReadCompare();       // Reads the current value that the servo is at 
                                                // Higher servo value means ground (~2400)
                                                // lower servo value means higher (max ~1400)
    
    int difference = (new_position - current_position)/10;
                                            //Step in 1/10th the amount
            // if arm_position > current_position then a negative value will be given to current_position
            // if arm_position < current_position then a positive value will be given to current_position
    
    // The below code ensures that the servo moves smoothly, so as not to vibrate and unintentially
    // drop the puck from the arm 
    while (current_position != new_position){
        current_position += difference;         
        Rack_Servo_PWM_WriteCompare(current_position);
        CyDelay(10);
    }
    
    Rack_Servo_PWM_WriteCompare(new_position); // Reaches value set
    
    CyDelay(500);                               // Holds for half a second? 
    
    Rack_Servo_PWM_Sleep();                    // Puts the servo back to sleep
    
}
void changeHeightToPuck(int puckHeightIndex){
    
    // open gripper
    // down
    // move forward
    // close gripper 
   
    // Different positions of the arm:
    // Ground                               
    // 1 puck above
    // 2 pucks above
    // 3 pucks above                        
//  When the puck is dropped off at the construction zone, the claw will lift up to the
//      next puck level before it moves away 
    
    switch(puckHeightIndex){
        case 0:                     // Ground
        armTranslate(2400);    break;
        case 1:                     // 1 puck above the ground
        armTranslate(1700);    break;
        case 2:                     // 2 pucks above the ground
        armTranslate(1250);    break;
        case 3:                     // 3 pucks above the ground
        armTranslate(1000);    break;
                                    // also max height
                                    // also the position it will be in when driving (so that ultrasonics will work
        default:                    // will be set to max height as default
        armTranslate(2400);    break;
        break;
    }
}


// NEW CODE WRITTEN BY MISHA: 

// POSITIONS needed:
    // Driving position - gripper is above the ultrasonic
    // picking up position - down, and then up
            // different switch cases - stage 1 height, stage 2 height, stage 3 height 



// OLD CODE:

void closeAndRaise(int nothing) {nothing++;}
void lowerAndOpen(int nothing) {nothing++;}


//This takes input in cm
void armMoveUpDown(int desiredPosition){
    //40 teeth on the gear
    //as the wheel rotates a certain angle, the arm moves up by r*theta. 
    //At angle = 0, we have arm at lowest position
    //At angle = 180, we have it at the highest
    //So theta= desiredheight/r . r = 20 mm approx. We'll work on it
    const int GEAR_RADIUS = 20;
    int theta = (desiredPosition*10)/GEAR_RADIUS;
    int compare_val = round(theta*5.5555);
    Rack_Servo_PWM_WriteCompare(1000+compare_val); //This needs to be modified a lot
    CyDelay(500);
}


/* [] END OF FILE */
