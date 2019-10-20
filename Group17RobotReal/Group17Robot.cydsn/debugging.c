
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
                        
                        armMoving();
                        
                        
                        for(int i = 0; i < 4; i++)
                        {
                            changeHeightToPuck(i, NEITHER);
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
                        
                        sprintf(output, "measure: %d\n", ultrasonic_distances_mm[BACK]);      
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
                        
                        while(1) {
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
                        
                        
                        moveUntil(150, FORWARD, LESS_THAN, FRONT_LEFT, SPEED, TRUE);              // should keep moving until we approach 50mm wall
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

}

/* [] END OF FILE */
