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


#include <project.h>
#include "dcmotor.h"
#include "main.h"
#include "ultrasonic.h"

// * C LIBRARIES * //
#include <stdio.h>
#include <math.h>


#define FALSE 0
#define TRUE 1
#define NORTH 0
#define WEST 1
#define SOUTH 2
#define EAST 3

#define FULL_SENSOR_DELAY_MS 200

typedef uint8 bool;

const short MOTOR_1_DUTY_CYCLE = 70; //Could split this up into left and right if we notice
const short MOTOR_2_DUTY_CYCLE = 70;
//one motor being permanently slower than another.
const short MOTOR_DRIVER_PERIOD = 300;

//Change between 0 and MOTOR_DRIVER_PERIOD/100 to see which one makes the robot drive straightest
const short ADDED_EXTRA_COMPARE_VALUE = 1; 

const int TIME = 70; //Delay time for the moveABit functions. This needs to be altered
//to ensure that we have 1 cm movement per call of moveForwardABit etc
const int TURNING_TIME_LEFT = 13; //This is for the turning functions, we wanna turn by a deg
const int TURNING_TIME_RIGHT = 12;

short currentPosition[2] = {0,0}; //Just defining the variable here, 
//we will need to initialise when power on
short currentOrientation = NORTH; //Just defining here again, the orientation 
//also needs to be initalised

bool forwardMoveEnable = FALSE;
bool backwardMoveEnable = FALSE;
bool leftTurnEnable = FALSE;
bool rightTurnEnable = FALSE;
bool motor1Enable = FALSE;
bool motor2Enable = FALSE;

//Feedforward control
    //Figure out how many counts equate to how much distance
    //In the function take a value indicating how much you want to move
    //Move exactly the right number of counts to get to the position you want

/*
//How to use ISR? 
//In this ISR, disable the relevant motor, stop the relevant counter and reset the relevant counter
CY_ISR(ISR_STOP_MOTOR_1){
    //The three commands here may make the ISRs a bit too long.
    Motor_1_Encoder_Counts_ReadStatusRegister();
    motor1Enable = FALSE;
}

CY_ISR(ISR_STOP_MOTOR_2){
    //The three commands here may make the ISRs a bit too long.
    Motor_2_Encoder_Counts_ReadStatusRegister();
    motor2Enable = FALSE;
}

*/

/*
//Takes four arguments (each a percentage duty cycle as a short) and writes them to 
//the compare values of the motors. Will be used inside the open-loop move and rotate 
//functions
void motor_driver_compare_update(short m1_d1,short m1_d2,short m2_d1,short m2_d2){
    Motor_1_driver_WriteCompare1(MOTOR_DRIVER_PERIOD*m1_d1/100); //Using 80% duty cycle for movement in general. 
    Motor_1_driver_WriteCompare2(MOTOR_DRIVER_PERIOD*m1_d2/100);
    Motor_2_driver_WriteCompare1(MOTOR_DRIVER_PERIOD*m2_d1/100); 
    Motor_2_driver_WriteCompare2(MOTOR_DRIVER_PERIOD*m2_d2/100);
}
*/

/*
void startMotion(short requiredCount){
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    Motor_1_Encoder_Counts_WritePeriod(requiredCount);
    Motor_2_Encoder_Counts_WritePeriod(requiredCount);
    Motor_1_Encoder_Counts_Start();
    Motor_2_Encoder_Counts_Start();
    Motor_1_driver_Start();
    Motor_2_driver_Start();
    
}

*/

/*
//stopMotion stops the motors (by disabling PWM), then stops the counters, then resets 
//the counters. This way, next time we need to use motion, we have to restart it all and go
void stopMotion(void){
    Motor_1_driver_Stop();
    Motor_2_driver_Stop();
    Motor_1_Encoder_Counts_Stop();
    Motor_2_Encoder_Counts_Stop();
}

*/

/*
//We also need to figure out how we are going to store the location of the black block

//Note that we are taking a short as our input here. If the resolution is less than one, 
//we will need to change this data type, because it will now always call with zero
void moveForward(short amount){
    short requiredCount = amount/0.0175; //This assumes that amount is specified in cm
    //Also, the above variable is specified as a short int because the counter can only count up 
    //to 2^16 anyway (FF implementation). This means that we can only have 2^16*0.0175 cm 
    //available as our maximum (We need to do some arithmetic checking here - int/float = 
    //what?
    
    motor_driver_compare_update(MOTOR_DUTY_CYCLE,0,MOTOR_DUTY_CYCLE,0);
    startMotion(requiredCount);
    
    while (motor1Enable||motor2Enable){
        if (motor1Enable == FALSE){
            Motor_1_driver_Stop();
        }
        if (motor2Enable == FALSE){
            Motor_2_driver_Stop();
        }
    }
    stopMotion();
    //The below while statement is intended to keep us in the function until both 
    //motors are stopped by the ISRs. 
}

void moveBackward(short amount){
    short requiredCount = amount/0.0175; //This assumes that amount is specified in cm
    //Also, the above variable is specified as a short int because the counter can only count up 
    //to 2^16 anyway (FF implementation). This means that we can only have 2^16*0.0175 cm 
    //available as our maximum (We need to do some arithmetic checking here - int/float = 
    //what?
    
    motor_driver_compare_update(0,MOTOR_DUTY_CYCLE,0,MOTOR_DUTY_CYCLE);
    startMotion(requiredCount);
    //The below while statement is intended to keep us in the function until both 
    //motors are stopped by the ISRs. 
    while (motor1Enable||motor2Enable){
        if (motor1Enable == FALSE){
            Motor_1_driver_Stop();
        }
        if (motor2Enable == FALSE){
            Motor_2_driver_Stop();
        }
    }
    stopMotion();
    //The below while statement is inte
}

void turnLeft(void){
    short requiredCount = 449; //This comes from a calc involving dist from the centre
    
    motor_driver_compare_update(0,MOTOR_DUTY_CYCLE,MOTOR_DUTY_CYCLE,0);
    startMotion(requiredCount);
    
    while (motor1Enable||motor2Enable){
        if (motor1Enable == FALSE){
            Motor_1_driver_Stop();
        }
        if (motor2Enable == FALSE){
            Motor_2_driver_Stop();
        }
    }
    //Below stopMotion call ensures that BOTH drivers stop (not just one) and that 
    //we reset the encoders 
    stopMotion();
    currentOrientation = (currentOrientation+1)%4;
}

void turnRight(void){
    short requiredCount = 449; //This comes from a calc involving dist from the centre
    motor_driver_compare_update(MOTOR_DUTY_CYCLE,0,0,MOTOR_DUTY_CYCLE);
    startMotion(requiredCount);
    
    while (motor1Enable||motor2Enable){
        if (motor1Enable == FALSE){
            Motor_1_driver_Stop();
        }
        if (motor2Enable == FALSE){
            Motor_2_driver_Stop();
        }
    }
    stopMotion();
    currentOrientation = (currentOrientation-1)%4;
}

void updatePosition(short * currentPosition, short currentOrientation){
    float sensorList[5] = {0,0,0,0,0};
    //Poll the five sensors and store in sensorList
    if (currentOrientation == NORTH){
        currentPosition[2] = (sensorList[0]+sensorList[1])/2;
        currentPosition[1] = sensorList[2];
    }else if (currentOrientation == WEST){
        currentPosition[1] = (sensorList[0]+sensorList[1])/2;
        currentPosition[2] = sensorList[4];
    }else if (currentOrientation == EAST){
        currentPosition[1] = (sensorList[0]+sensorList[1])/2;
        currentPosition[2] = sensorList[2];
    }else{
    
    } 
}
*/

/*
short desiredPosition[2] = {1,2};

void moveClosedLoop(short desiredPosition[2]){
    short xToMove = desiredPosition[0]-currentPosition[0];
    short yToMove = desiredPosition[1]-currentPosition[1];
    const float resolution = 1; //We can move to 1 cm accuracy (can be adjusted 
    //depending on how good ultrasonics are and motor 
    while((yToMove > resolution/2) || (-resolution/2 < yToMove)){ //If within half of res, then good enough
        if ((yToMove > 0) && (forwardMoveEnable == TRUE)){
            moveForward(resolution);
        }else if ((yToMove < 0) && (backwardMoveEnable == TRUE)){
            moveBackward(resolution);
        }
        //Read ultrasonic sensors, update currentPosition,update xToMove and yToMove
        xToMove = desiredPosition[1]-currentPosition[0]; //Based on orientation, this 
        //should change
        yToMove = desiredPosition[2]-currentPosition[1];
    }
    
    short temp = 0;
    if (-resolution/2 < xToMove < resolution/2){
        return;
    }else if (xToMove < -resolution/2){
        turnLeft();
        temp = -1*xToMove;
        xToMove = yToMove;
        yToMove = temp;
    }else if (xToMove > resolution/2){
        turnRight();
        temp = xToMove;
        xToMove = yToMove;
        yToMove = temp;
    }
    
    while((yToMove > resolution/2) || (-resolution/2 < yToMove)){ //If within half of res, then good enough
        if ((yToMove > 0) && (forwardMoveEnable == TRUE)){
            moveForward(resolution); 
            
            //Read ultrasonic sensors, update currentPosition,update xToMove and yToMove
        }else if ((yToMove < 0) && (backwardMoveEnable == TRUE)){
            moveBackward(resolution);
            
            //Read ultrasonic sensors, update currentPosition,update xToMove and yToMove
        }
    }
    
    
}
 */

void moveForwardABit(){
    Motor_1_driver_Wakeup();
    Motor_2_driver_Wakeup();
    Motor_1_driver_WriteCompare1(MOTOR_1_DUTY_CYCLE*MOTOR_DRIVER_PERIOD/100 + ADDED_EXTRA_COMPARE_VALUE);
    Motor_1_driver_WriteCompare2(0);
    Motor_2_driver_WriteCompare1(MOTOR_2_DUTY_CYCLE*MOTOR_DRIVER_PERIOD/100);
    Motor_2_driver_WriteCompare2(0);
    CyDelay(TIME);
    Motor_1_driver_Sleep();
    Motor_2_driver_Sleep();
}

void moveBackwardABit(){
    Motor_1_driver_Wakeup();
    Motor_2_driver_Wakeup();
    Motor_1_driver_WriteCompare1(0);
    Motor_1_driver_WriteCompare2(MOTOR_1_DUTY_CYCLE*MOTOR_DRIVER_PERIOD/100);
    Motor_2_driver_WriteCompare1(0);
    Motor_2_driver_WriteCompare2(MOTOR_2_DUTY_CYCLE*MOTOR_DRIVER_PERIOD/100 + ADDED_EXTRA_COMPARE_VALUE);
    CyDelay(TIME);
    Motor_1_driver_Sleep();
    Motor_2_driver_Sleep();    
}

//This was originally turnRightABit, but had to be changed to turnLeftABit
void turnLeftABit(){
    Motor_1_driver_Wakeup();
    Motor_1_driver_WriteCompare1(0);
    Motor_1_driver_WriteCompare2(MOTOR_1_DUTY_CYCLE*MOTOR_DRIVER_PERIOD/100);
    Motor_2_driver_Wakeup();
    Motor_2_driver_WriteCompare1(MOTOR_2_DUTY_CYCLE*MOTOR_DRIVER_PERIOD/100);
    Motor_2_driver_WriteCompare2(0);
    CyDelay(TURNING_TIME_LEFT);
    Motor_1_driver_Sleep();
    Motor_2_driver_Sleep();    
}

//This was originally turnLeftABit, but had to be changed to turnRightABit
void turnRightABit(){
    Motor_1_driver_Wakeup();
    Motor_1_driver_WriteCompare1(MOTOR_1_DUTY_CYCLE*MOTOR_DRIVER_PERIOD/100);
    Motor_1_driver_WriteCompare2(0);
    Motor_2_driver_Wakeup();
    Motor_2_driver_WriteCompare1(0);
    Motor_2_driver_WriteCompare2(MOTOR_2_DUTY_CYCLE*MOTOR_DRIVER_PERIOD/100);
    CyDelay(TURNING_TIME_RIGHT);
    Motor_1_driver_Sleep();
    Motor_2_driver_Sleep();
}

void moveForward(short amount){
    //AMOUNT_TO_LOOPS_CONVERTER depends on the TIME constant in dcmotor.c file: each 
    //moveForwardABit() function call will move by an integer number of cm 
    //(lets try and make this accurate) then AMOUNT_TO_LOOPS_CONVERTER = amount of cm moved
    //under one call of moveForward
    
    const int AMOUNT_TO_LOOPS_CONVERTER = 1; 
    short requiredLoops = amount/AMOUNT_TO_LOOPS_CONVERTER;
    int i = 0;
    while (i < (requiredLoops)){
        moveForwardABit();
        i++;
    }
}

void moveBackward(short amount){
    const int AMOUNT_TO_LOOPS_CONVERTER = 1;
    short requiredLoops = amount/AMOUNT_TO_LOOPS_CONVERTER;
    int i = 0;
    while (i < (requiredLoops)){
        moveBackwardABit();
        i++;
    }
}

//This was originally turnRight, but now has to be changed to turnLeft
void turnRight(short amount){
    //Again will depend on TIME. We need to write so that each turnLeftABit goes up 
    //by like 5 deg or something. Then requiredLoops = amount/5 (so AMOUNT_TO_LOOPS_CONVERTER
    // = degrees turned by one turnRightABit call
    const int AMOUNT_TO_LOOPS_CONVERTER = 1;
    short requiredLoops = amount/AMOUNT_TO_LOOPS_CONVERTER;
    int i = 0;
    while (i < (requiredLoops)){
        turnRightABit();
        i++;
    }
}

//This was originally turnRight, but now has to be changed to turnLeft (turnLeft 
//can only accept multiples of 5 deg as arguments. So telling it to turn left by 12 degrees
//will end up only turning 10.
void turnLeft(short amount){
      
    const int AMOUNT_TO_LOOPS_CONVERTER = 1;
    short requiredLoops = amount/AMOUNT_TO_LOOPS_CONVERTER;
    int i = 0;
    while (i < (requiredLoops)){
        turnLeftABit();
        i++;
    }

}

//Make sure degrees is less than 90. Something like 20 would be ideal
void displaceLeft(int amount,int degrees){
    turnRight(degrees);
    moveBackward(amount/sin((M_PI/180)*degrees));
    turnLeft(degrees);
    moveForward(amount/tan((M_PI/180)*degrees));
}

void displaceRight(int amount,int degrees){
    turnLeft(degrees);
    float temp = amount/sin((M_PI/180)*degrees);
    moveBackward(amount/sin((M_PI/180)*degrees));
    turnRight(degrees);
    temp = amount/tan((M_PI/180)*degrees);
    moveForward(amount/tan((M_PI/180)*degrees));
}

void moveForwardIndefinitely(){
     
    int i = 0;
    const int SENSE_TO_DRIVE_RATIO = 1;
    
  
    Motor_1_driver_Wakeup();
    Motor_2_driver_Wakeup();
    Motor_1_driver_WriteCompare1(MOTOR_1_DUTY_CYCLE*MOTOR_DRIVER_PERIOD/100 + ADDED_EXTRA_COMPARE_VALUE);
    Motor_1_driver_WriteCompare2(0);
    Motor_2_driver_WriteCompare1(MOTOR_2_DUTY_CYCLE*MOTOR_DRIVER_PERIOD/100);
    Motor_2_driver_WriteCompare2(0);
        
    while (driveStraightEnable){
        
        distanceCheck();
        CyDelay(FULL_SENSOR_DELAY_MS);
    }

}

void moveBackwardIndefinitely(){
     
    int i = 0;
    const int SENSE_TO_DRIVE_RATIO = 1;
    
  
    Motor_1_driver_Wakeup();
    Motor_2_driver_Wakeup();
    Motor_1_driver_WriteCompare1(0);
    Motor_1_driver_WriteCompare2(MOTOR_1_DUTY_CYCLE*MOTOR_DRIVER_PERIOD/100);
    Motor_2_driver_WriteCompare1(0);
    Motor_2_driver_WriteCompare2(MOTOR_2_DUTY_CYCLE*MOTOR_DRIVER_PERIOD/100 + ADDED_EXTRA_COMPARE_VALUE);
        
    while (driveStraightEnable){
        
        distanceCheck();
        CyDelay(FULL_SENSOR_DELAY_MS);
    }

}
/* [] END OF FILE */
