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

#include "servo.h"
#include "project.h"

// * C LIBRARIES * // 
#include <stdio.h>
#include <math.h>


//These are the values that correspond to open, closed, up and down positions of the 
//arm. These were found by experiment, so if need to recalibrate, you know where to go
#define LOW_POS_ARM_CMP 2400 
#define HIGH_POS_ARM_CMP 1000
#define OPEN_POS_GRIPPER_CMP 1120 //1170
#define CLOSED_POS_GRIPPER_CMP 1250

void armOpen(){
    Gripper_Servo_Disable_Write(0);
    Gripper_Servo_PWM_WriteCompare(OPEN_POS_GRIPPER_CMP); //1170 turns out to be vibrationless open val
    CyDelay(500);
    Gripper_Servo_Disable_Write(1);
}

void armClose(){
    Gripper_Servo_Disable_Write(0);
    Gripper_Servo_PWM_WriteCompare(CLOSED_POS_GRIPPER_CMP); //1250 turns out to be a decent closed val
    CyDelay(500);
    Gripper_Servo_Disable_Write(1);
}

//This is just for the prelim comp - final comp will have a more comprehensive function 
//that opens to 3 or 4 discrete heights for each of the pucks
void armUp(){
    Gripper_Servo_Disable_Write(0);
    int i = Rack_Servo_PWM_ReadCompare();
    while (i > HIGH_POS_ARM_CMP){
        Rack_Servo_PWM_WriteCompare(i);
        i -= (LOW_POS_ARM_CMP-HIGH_POS_ARM_CMP)/10; //Step in 1/10th of the amount 
        CyDelay(10);
    }
    Rack_Servo_PWM_WriteCompare(HIGH_POS_ARM_CMP); //1000 was a nice max height value
    CyDelay(500);
    Gripper_Servo_Disable_Write(1);
}

void armDown(){
    Gripper_Servo_Disable_Write(0);
    int i = Rack_Servo_PWM_ReadCompare();
    while (i < LOW_POS_ARM_CMP){
        Rack_Servo_PWM_WriteCompare(i);
        i += (LOW_POS_ARM_CMP-HIGH_POS_ARM_CMP)/10; //Step in 1/10th the amount
        CyDelay(10);
    }
    Rack_Servo_PWM_WriteCompare(LOW_POS_ARM_CMP); //2300 was a nice low height value
    CyDelay(500);
    Gripper_Servo_Disable_Write(1);
}

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

void gripperAngle(int amount){
    if (0 < amount && amount < 180)
    {
        Gripper_Servo_PWM_WriteCompare(1000+amount*5.5);
    }
    Gripper_Servo_PWM_Start();
    CyDelay(500);
}
        
void rackAngle(int amount)
{
    Rack_Servo_PWM_WriteCompare(1000+amount*5.5);
    Rack_Servo_PWM_Start();
    CyDelay(500);
}

void changeHeightToPuck(int puckHeightIndex){
    // TO COMPLETE NIDHIN   
}

void lowerAndOpen(int puck_stack_position){
    changeHeightToPuck(puck_stack_position);
    armOpen();
}

void closeAndRaise(int puck_stack_position){
    armClose();
    changeHeightToPuck(puck_stack_position);
}

/* [] END OF FILE */
