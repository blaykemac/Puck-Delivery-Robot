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

// Variables:



void moveDynamic(int distance, int speed, int activate_safety){
    int count_left;
    int count_right;
    int count_difference;      //count_left - count_right
                                // if negative: count_right > count_left
                                // if positive count_left > count_right
    int compare;
    int direction;
    int old_count = -SAFETY_MARGIN*ENCODER_MULTIPLIER;   // used in the old failsafe to check how much distance has passed 
                                                        // will enter the failsafe straight away due to this
    int emergency_exit = FALSE;
    
    // FAILSAFE if you give a speed greater than it can handle 
    int speed_left = speed;
    int speed_right = speed;
    if (speed > SPEED_MAX) {
        speed_left = SPEED;         
        speed_right = SPEED;
    }
    
    // The distance is in millimetres 
    // Is the distance negative or positive? 
    if (distance > 0) { Motor_Left_Control_Write(0); Motor_Right_Control_Write(0); direction = FORWARD;}
    else {Motor_Left_Control_Write(1); Motor_Right_Control_Write(1); direction = BACKWARD;}
    
    Motor_Left_Driver_Wakeup();
    Motor_Right_Driver_Wakeup();
    
    compare = round(distance*5.29);
    count_left = Motor_Left_Decoder_GetCounter();
    count_right = Motor_Right_Decoder_GetCounter();
    
    //rampUpDown(speed, RAMP_UP);     // ramp the speed up smoothly
    
    while (abs(count_left) < abs(compare) && abs(count_right) < abs(compare) && emergency_exit == FALSE) {
        count_left = Motor_Left_Decoder_GetCounter();
        count_right = Motor_Right_Decoder_GetCounter();
        if (count_left > count_right) {
            if(speed_left == speed_right){
                speed_left -= ADJUST;
                speed_right += ADJUST;
            }
            else {
                //speed_left = speed;
                //speed_right = speed;
                int temp = speed_left;
                speed_left = speed_right;
                speed_right = temp;
            }        
        }
        if (count_right > count_left) {
            if(speed_left == speed_right) {
                speed_right -= ADJUST;              // If the speeds are equal, we decrememnt within the specific 
                                                    // adjust tolerance 
                speed_left += ADJUST;
            }
            else {
                //speed_left = speed;
                //speed_right = speed;
                int temp = speed_left;              // if they are not equal, we just swap em
                speed_left = speed_right;
                speed_right = temp;
            }
        }
        
        // Not sure if the following code made a difference: 
        
        /*
        count_difference = count_left - count_right;
        if (count_difference > 5) {
            count_left -= ADJUST;
            count_right += ADJUST;
        }
        if (count_difference < 5) {
            count_left += ADJUST;
            count_right -= ADJUST;
        }
        */
        
        Motor_Left_Driver_WriteCompare(speed_left);
        Motor_Right_Driver_WriteCompare(speed_right);
        
        // FAILSAFE:
        if (abs(count_left) > (old_count + SAFETY_MARGIN*ENCODER_MULTIPLIER - 100) && activate_safety == TRUE){
            emergency_exit = failsafe(direction);
            old_count = count_left; 
                
            //sprintf(output, "left motor: %d \n", count_left);       
            //UART_1_PutString(output);
            //sprintf(output, "right motor: %d \n", count_right);      
            //UART_1_PutString(output);
            
            
            
        }
        
    }
    
    sprintf(output, "left motor: %d \n", count_left);       
    UART_1_PutString(output);
    sprintf(output, "right motor: %d \n", count_right);      
    UART_1_PutString(output);
        
    if (emergency_exit == FALSE) {
        //rampUpDown(speed, RAMP_DOWN);     // ramp the speed down smoothly
    }
    
    Motor_Left_Driver_Sleep();
    Motor_Right_Driver_Sleep();    
    
    Motor_Left_Decoder_SetCounter(0);                       // RESET the decoders to 0
    Motor_Right_Decoder_SetCounter(0);   
}

void moveSwivel(int degrees, int speed, int activate_safety) {
    int count_left;
    int count_right;
    int compare;
    int distance;
    int speed_left = speed;
    int speed_right = speed;
    
    // FAILSAFE if you give a speed greater than it can handle:
    if (speed > SPEED_MAX) {
        speed_left = SPEED;         
        speed_right = SPEED;
    }
    
    // Is the degrees negative or positive? 
        // positive = right hand turn: left wheel goes forward, right wheel goes backward 
        // negative = left hand turn: right wheel goes forward, left wheel goes backward 
    if (degrees > 0) { Motor_Left_Control_Write(0); Motor_Right_Control_Write(1); }
    else {Motor_Left_Control_Write(1); Motor_Right_Control_Write(0);}
    
    Motor_Left_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(speed_left);
    Motor_Right_Driver_Wakeup();
    Motor_Right_Driver_WriteCompare(speed_right);
    
    distance = (WIDTH_WHEEL_TO_WHEEL/2)*degrees;            // based on the arclength the wheels need to travel
    compare = round((distance*3.14*5.29)/180);
    count_left = Motor_Left_Decoder_GetCounter();
    count_right = Motor_Right_Decoder_GetCounter();
    
    // NOT SURE IF THE DRIFT ADJUSTING FOR THIS FUNCTION WORKS
    
    
    while (count_right != -1*compare) {
        count_right = Motor_Right_Decoder_GetCounter();        
        count_left = Motor_Left_Decoder_GetCounter();
         if (count_left > -1*count_right) {
            if(speed_left == speed_right){
                //speed_left -= ADJUST;
                //speed_right += ADJUST;
            }
            else {
                int temp = speed_left;
                speed_left = speed_right;
                speed_right = temp;
            }        
        }
        if (count_right > -1*count_left) {
            if(speed_left == speed_right) {
                //speed_right -= ADJUST;              // If the speeds are equal, we decrememnt within the specific 
                //speed_left += ADJUST;
            }
            else {
                int temp = speed_left;              // if they are not equal, we just swap em
                speed_left = speed_right;
                speed_right = temp;
            }
        } 
        Motor_Left_Driver_WriteCompare(speed_left);
        Motor_Right_Driver_WriteCompare(speed_right);
    }
    
    /*
    sprintf(output, "left motor: %d \n", count_left);       
    UART_1_PutString(output);
    sprintf(output, "right motor: %d \n", count_right);      
    UART_1_PutString(output);
    */
    
    Motor_Left_Decoder_SetCounter(0);                       // RESET the decoders to 0
    Motor_Right_Decoder_SetCounter(0);   
    
    Motor_Left_Driver_Sleep();
    Motor_Right_Driver_Sleep();
    
    
}

void translateMoveDynamic(int distance, int degree, int speed, int activate_safety) {
    // The distance selectected can be positive or negative
    // The degree selected can be positive or negative 
    int new_dist = distance/sin((M_PI/180)*degree);
    
    moveSwivel(degree, speed, activate_safety);
    moveDynamic(new_dist, speed, activate_safety);
    moveSwivel(-degree, speed, activate_safety);
    moveDynamic(-new_dist, speed, activate_safety);
}

void rampUpDown(int speed, int ramp_up_down) {
    speed = abs(speed);     // fixes bug with negative 
    int step = (speed/10);
    int current_speed = 0; 
    int old_speed;
    
    if (ramp_up_down == RAMP_UP) {
        while (current_speed < (speed - 1.5*step)){
            current_speed += step;
            Motor_Left_Driver_WriteCompare(current_speed);
            Motor_Right_Driver_WriteCompare(current_speed);
            CyDelay(50);
        }
        Motor_Left_Driver_WriteCompare(speed);
        Motor_Right_Driver_WriteCompare(speed);
    }
    
    if (ramp_up_down == RAMP_DOWN) {
        current_speed = speed;
        while (current_speed > 1.5*step){
            current_speed -= step;
            Motor_Left_Driver_WriteCompare(current_speed);
            Motor_Right_Driver_WriteCompare(current_speed);
            CyDelay(50);
        }
        Motor_Left_Driver_WriteCompare(0);
        Motor_Right_Driver_WriteCompare(0);
    }

}



/* [] END OF FILE */
