
/* ========================================
 *
 * Copyright Group 17, 2019
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETAR INFORMATION
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

// Set which debugging you want to do:
int colour_calibration = FALSE;             // Do we want to calibrate the sensor? 
int servo_testing = FALSE;                  // Do the servos need to be tested?      
int motor_testing = FALSE;
int ultrasonic_testing = FALSE;  


void ultimateDebugging(void){
    // Colour Calibration: 
        if (colour_calibration)
        {
            int lock = FALSE;   
            do 
                {   
                    if (lock == FALSE) 
                    {
                        
                        control_photodiode_Write(CLAW_SENSING); 
                        changeHeightToPuck(0, NEITHER);
                        
                        for (int i = 0; i < 4; i++) 
                        {
                            control_led_Write(i);
                            CyDelay(500);
                        }

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
                        
                        //armMoving();
                        
                        /*
                        changeHeightToPuck(GROUND, NEITHER);
                        CyDelay(1000);
                        changeHeightToPuck(ABOVE_1_PUCK, NEITHER);
                        CyDelay(1000);
                        changeHeightToPuck(ABOVE_2_PUCK, NEITHER);
                        CyDelay(1000);
                        changeHeightToPuck(ABOVE_3_PUCK, NEITHER);
                        CyDelay(1000);
                        */
                        //for(int i = 0; i < 4; i++)
                        //{
                        //    changeHeightToPuck(i, NEITHER);
                        //    CyDelay(1000);
                        //}
                        
                  
                        //armClose();
                        //CyDelay(2000);
                        //armOpen();
                        //CyDelay(2000);
                        
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
                        moveForwardThenBackward(200, LESS_THAN, FRONT_MIDDLE, SPEED_LOW, TRUE);
                        //translateUntil(150, LEFT, GREATER_THAN, SIDE_RIGHT, SPEED);
                        
                       //straightAdjustSensor(FRONT_MIDDLE);
                        //straightAdjustBack();
                        
                        //straightAdjust(BACK_SENSORS);
                        //straightAdjust(FRONT_SENSORS);
                        
                        while(0) {
                            straightAdjust(FRONT_SENSORS);
                            blinkLED(GREEN, 1000);
                        }
                        
                        
                        
                        while(0){
                        moveUntil(120, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);
                        CyDelay(2000);
                        }
                        
                        while(0) {
                        straightAdjust(BACK_SENSORS);
                        blinkLED(GREEN, 1000);
                        }
                        
                        int timer;
                        
                        Timer_straight_adjust_Start();
                        
                        while(0) {
                        timer = Timer_straight_adjust_ReadCounter();
                        sprintf(output, "%d \t", timer);      
                        UART_1_PutString(output); 
                        CyDelay(100);
        
                        }
                        
                        //moveDynamic(-300);
                        
                        //moveSwivel(360, SPEED, TRUE);
                        
                        
                        
                        //moveUntil(200, BACKWARD, GREATER_THAN, FRONT_LEFT, SPEED, TRUE);
                        
                        /*
                        changeOrientation(WEST, SPEED);
                        CyDelay(1000);
                        changeOrientation(SOUTH, SPEED);
                        CyDelay(1000);
                        changeOrientation(WEST, SPEED);
                        CyDelay(1000);
                        changeOrientation(NORTH, SPEED);
                        CyDelay(1000);
                        
                        
                        
                        
                        moveUntil(150,FORWARD,LESS_THAN,FRONT_LEFT,SPEED,TRUE);
                        straightAdjust();
                        
                        
                        
                        
                        
                        int move = 400;
                        moveDynamic(move,SPEED,TRUE);
                        moveDynamic(-move,SPEED,TRUE);
                        distanceSensor(BACK);
                        CyDelay(50);
                        
                        sprintf(output, "measure: %d\n", sensor_distances[BACK]);      
                            UART_1_PutString(output); 
        

                        */
                        
  
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
                            distanceSensor(BACK_LEFT);
                            CyDelay(500);
                            sprintf(output, "%d \t %d \t %d \t %d \t %d \n", sensor_distances[0], 
                                                     sensor_distances[1],
                                                     sensor_distances[2],
                                                        sensor_distances[3],
                                                        sensor_distances[4]);
                            UART_1_PutString(output);
                        }
                        // 0 gave front left    // front left
                        // 1 gave front right   // front right 
                        // 2 gave back sensor   // side left now??? 
                        // 3 gave side right    // side right 
                        // 4 gave back sensor   // back sensor
                        
                        while(0) {
                            for (int i = 0; i < 7; i++)
                            {
                                distanceSensor(i);
                                CyDelay(100);
                                sprintf(output, "%d \t", sensor_distances[i]);
                                UART_1_PutString(output);
                            }
                            UART_1_PutString("\n");
                        }
                        
                        // FRONT_LEFT   0
                        // FRONT_ RIGHT 1
                        // SIDE_LEFT 2
                        // SIDE_RIGHT 3
                        // BACK_LEFT 4
                        // BACK_RIGHT 6
                        
                        while(1) {
                            distanceSensor(BACK_LEFT);
                            CyDelay(50);
                            distanceSensor(BACK_RIGHT);
                            CyDelay(50);
                            sprintf(output, "%d , %d \n", sensor_distances[BACK_LEFT], sensor_distances[BACK_RIGHT]);
                            UART_1_PutString(output);
                            CyDelay(100);
                        }
                        
                        
                        // TEST FIRING
                        for (int i = 0; i < 5; i++)
                            {
                                distanceSensor(i);
                                CyDelay(100);
                                sprintf(output, "%d \t", sensor_distances[i]);
                                UART_1_PutString(output);
                            }
                        UART_1_PutString("\n");
                        
                        
                        moveUntil(150, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);              // should keep moving until we approach 50mm wall
                        straightAdjust(FRONT_SENSORS);
                        
                        
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

}

/* [] END OF FILE */
