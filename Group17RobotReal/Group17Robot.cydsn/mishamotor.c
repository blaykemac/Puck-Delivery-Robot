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
#include "mishamotor.h"
#include "customMath.h"
#include "navigation.h"

// Variables:



void mishaMoveDynamic(int distance, int speed){
    int count_left;
    int count_right;
    int compare;
    
    // FAILSAFE if you give a speed greater than it can handle 
    int speed_left = speed;
    int speed_right = speed;
    if (speed > SPEED_MAX) {
        speed_left = SPEED;         
        speed_right = SPEED;
    }
    
    // The distance is in millimetres 
    // Is the distance negative or positive? 
    if (distance > 0) { Motor_Left_Control_Write(0); Motor_Right_Control_Write(0); }
    else {Motor_Left_Control_Write(1); Motor_Right_Control_Write(1);}
    
    Motor_Left_Driver_Wakeup();
    Motor_Left_Driver_WriteCompare(speed_left);
    Motor_Right_Driver_Wakeup();
    Motor_Right_Driver_WriteCompare(speed_right);
    
    compare = round(distance*5.29);
    count_left = Motor_Left_Decoder_GetCounter();
    count_right = Motor_Right_Decoder_GetCounter();
    
    while (count_right != compare) {
        count_left = Motor_Left_Decoder_GetCounter();
        count_right = Motor_Right_Decoder_GetCounter();
        if (count_left > count_right) {
        speed_left -= ADJUST;
        }
        if (count_right > count_left) {
        speed_right -= ADJUST; 
        }
    }
    
    sprintf(output, "left motor: %d \n", count_left);       
    UART_1_PutString(output);
    sprintf(output, "right motor: %d \n", count_right);      
    UART_1_PutString(output);
        
    Motor_Left_Decoder_SetCounter(0);                       // RESET the decoders to 0
    Motor_Right_Decoder_SetCounter(0);   
    
    Motor_Left_Driver_Sleep();
    Motor_Right_Driver_Sleep();
   
    
}

void mishaSwivel(int degrees, int speed) {
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
    
    distance = 107*degrees;
    compare = round((distance*3.14*5.29)/180);
    count_left = Motor_Left_Decoder_GetCounter();
    count_right = Motor_Right_Decoder_GetCounter();
    
    while (count_right != -1*compare) {
        count_right = Motor_Right_Decoder_GetCounter();
        /*
        count_left = Motor_Left_Decoder_GetCounter();
        count_right = Motor_Right_Decoder_GetCounter();
        if (count_left > count_right) {
        speed_left--;
        }
        if (count_right > count_left) {
        speed_right--; 
        }
        */
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

void translateMoveDynamic(int distance) {
    distance = 0;
    
    
    
    
}





/* [] END OF FILE */
