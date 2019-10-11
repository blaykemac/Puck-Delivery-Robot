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

//Standard C libraries
#include "project.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ultrasonic.h"
#include "main.h"
#include "dcmotor.h"

//global variable currentPosition and currentOrientation (in main). Declare as externs in 
//dcmotor, I guess
extern float currentPosition[2];
extern float desiredPosition[2];
extern int currentOrientation; //in degrees (convert to radians when needed)
extern int desiredOrientation;
extern short int motor1Enable; //These two will be on if the motors are on
extern short int motor2Enable;
extern int M1_FD; //The four values shown here will be the duty cycles of the motors
extern int M1_BD; //There are times when parts of the code (such as the driftCorrect function)
extern int M2_FD; //need to know about the duty cycles. So we make the duty cycles a globally 
extern int M2_BD; //known variable
extern int motor1EncoderCounts; //These two variables will allow the entire program to keep 
extern int motor2EncoderCounts; //track of how many turns the motors have spun 
extern short int moveNow; //This is a flag that lets the main program tell the moving functions
//whether we want the robot to be moving or not. For example, when we need to operate servos
//the main program would set moveNow to FALSE.
extern float block_location[4]; //This will hold the co-ordinates for the obstacle block. 0,1 will be xmin
//and xmax, 2,3 will be ymin, ymax.
extern int state; //The state of the robot as defined in main 

//The four flags below let the rest of the program know if the robot is trying to drive 
//forward or back, turn left or right. At the start of every motion, turn these flags on
//at the end turn them off.
extern int drivingForwardFlag;
extern int drivingBackwardFlag;
extern int turningLeftFlag;
extern int turningRightFlag;

//These flags will be used by the main program to allow the robot to move in certain directions
extern short int moveLeftAllowed;
extern short int moveRightAllowed;
extern short int moveForwardAllowed;
extern short int moveBackwardAllowed;

extern char output[32]; //for the UART

//This function takes a number of counts from the encoder and, assuming that the number of 
//counts has been moved in the currentOrientation direction, updates the current position. 
//Current position can only be updated from within this function
void positionUpdate(int counts, int drivingForwardFlag, int drivingBackwardFlag){
    float currentOrientation_Radians = (M_PI*currentOrientation/180);
    if (drivingForwardFlag == TRUE){
        //When we update position, be aware that we measure orientation as 0 degres facing
        //to the east of the arena and increasing counterclockwise
        currentPosition[0] += CM_PER_COUNT*counts*cos(currentOrientation_Radians);
        currentPosition[1] += CM_PER_COUNT*counts*sin(currentOrientation_Radians);
    }else if (drivingBackwardFlag == TRUE){
        currentPosition[0] += CM_PER_COUNT*counts*cos(currentOrientation_Radians + M_PI); 
        currentPosition[1] += CM_PER_COUNT*counts*sin(currentOrientation_Radians + M_PI);
    }
}

//This function takes a number of counts from the encoder, and based on whether we are 
//turning left or right, updates the current orientation. Current orientation can only
//be updated from within this function
void orientationUpdate(int counts,int turningLeftFlag, int turningRightFlag){
    int degreesMoved = counts*DEGREES_PER_COUNT;
    if (turningLeftFlag == TRUE){
        currentOrientation += degreesMoved;
        currentOrientation = currentOrientation%360;
    }else if (turningRightFlag == TRUE){
        currentOrientation -= degreesMoved;
        currentOrientation = currentOrientation%360;
        if (currentOrientation < 0){
            currentOrientation += 360;
        }
    }
}

//This function will be called once the ultrasonics or the encoders decide that it is time
//to stop moving. 
void stopMotion(void){
    if ((motor1Enable == FALSE) && (motor2Enable == FALSE)){
        //Update the driving permission flags
        //Read encoder counts and update from previous position
        int completedCounts;
        if ((drivingForwardFlag == TRUE)||(drivingBackwardFlag == TRUE)){
            //We assume that the two counter will have identical counts
            completedCounts = Motor_1_Encoder_Counts_ReadCounter();
            positionUpdate(completedCounts,drivingForwardFlag,drivingBackwardFlag);
        }else if (turningLeftFlag == TRUE){
            completedCounts = Motor_2_Encoder_Counts_ReadCounter();
            orientationUpdate(completedCounts,turningLeftFlag,turningRightFlag);
        }else if (turningRightFlag == TRUE){
            completedCounts = Motor_1_Encoder_Counts_ReadCounter();
            orientationUpdate(completedCounts,turningLeftFlag,turningRightFlag);
        }
                
        //Clear encoder counts
        Motor_1_Encoder_Counts_WriteCounter(0);
        Motor_2_Encoder_Counts_WriteCounter(0);
        
        motor1EncoderCounts = 0;
        motor2EncoderCounts = 0;
        
        //Just for now, we are going to update driving flags by turning all of them off
        drivingForwardFlag = FALSE;
        drivingBackwardFlag = FALSE;
        turningLeftFlag = FALSE;
        turningRightFlag = FALSE;
        
        //Have code here to plan the route which should also update the driving flags
        
    }
}

//Takes the duty cycles must be expressed as a percentage and runs the motors at those duty
//cycles
void motorDutyCycleUpdate(float M1_FD, float M1_BD, float M2_FD, float M2_BD){
    Motor_1_driver_WriteCompare1((M1_FD/NUM_TO_PERCENT_CONVERTER)*DCMOTOR_PWM_PERIOD);
    Motor_2_driver_WriteCompare1((M2_FD/NUM_TO_PERCENT_CONVERTER)*DCMOTOR_PWM_PERIOD);
    Motor_1_driver_WriteCompare2((M1_BD/NUM_TO_PERCENT_CONVERTER)*DCMOTOR_PWM_PERIOD);
    Motor_2_driver_WriteCompare2((M2_BD/NUM_TO_PERCENT_CONVERTER)*DCMOTOR_PWM_PERIOD);
}

//This function will be responsible for adjusting any drift. It will read and modify 
//global variables M1_FD,M1_BD,M2_FD and M2_BD to do so.
void driftCorrect(void){
    int temp; //allows us to swap motor encoders' speeds
    if (motor1EncoderCounts > motor2EncoderCounts){
        if (drivingForwardFlag == TRUE){
            if (M1_FD == M2_FD){
                M1_FD = M2_FD - 1;
            }else{
                temp = M1_FD;
                M1_FD = M2_FD;
                M2_FD = temp;
            }
        }
        else if (drivingBackwardFlag == TRUE){
            if (M1_BD == M2_BD){
                M1_BD = M2_BD - 1;
            }else{
                temp = M1_BD;
                M1_BD = M2_BD;
                M2_BD = temp;
            }
        }else if (turningLeftFlag == TRUE){
            if (M1_BD == M2_FD){
                M1_BD = M2_FD - 1;
            }else{
                temp = M1_BD;
                M1_BD = M2_FD;
                M2_FD = temp;
            }
        }else if (turningRightFlag == TRUE){
            if (M1_FD == M2_BD){
                M1_FD = M2_BD - 1;
            }else{
                temp = M1_FD;
                M1_FD = M2_BD;
                M2_BD = temp;
            }
        }
    }else if (motor2EncoderCounts > motor1EncoderCounts){
        if (drivingForwardFlag == TRUE){
            if (M1_FD == M2_FD){
                M2_FD = M1_FD - 1 ;
            }else{
                temp = M1_FD;
                M1_FD = M2_FD;
                M2_FD = temp;
            }
        }
        else if (drivingBackwardFlag == TRUE){
            if (M1_BD == M2_BD){
                M2_BD = M1_BD - 1;
            }else{
                temp = M1_BD;
                M1_BD = M2_BD;
                M2_BD = temp;
            }
        }else if (turningLeftFlag == TRUE){
            if (M1_BD == M2_FD){
                M1_BD = M2_FD - 1;
            }else{
                temp = M1_BD;
                M1_BD = M2_FD;
                M2_FD = temp;
            }
        }else if (turningRightFlag == TRUE){
            if (M1_FD == M2_BD){
                M1_FD = M2_BD - 1;
            }else{
                temp = M1_FD;
                M1_FD = M2_BD;
                M2_BD = temp;
            }
        }
    }
    motorDutyCycleUpdate(M1_FD,M1_BD,M2_FD,M2_BD);
    Drift_Check_Timer_Start(); 
}

//Takes two integers count1 and count2, and sets the counters up so that they will start counting 
//at zero and interrupt when their counts equal the count values specified
void counterInitialise(int count1, int count2){
    Motor_1_Encoder_Counts_WriteCounter(0); //Start the counters from 0
    Motor_2_Encoder_Counts_WriteCounter(0); //Start the counters from 0
    Motor_1_Encoder_Counts_WritePeriod(count1);
    Motor_2_Encoder_Counts_WritePeriod(count2);
}

//Takes a distance to move in cm and moves. Blocking (sadly)
void moveForward(float amount){
    int counts = amount/CM_PER_COUNT;
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    drivingForwardFlag = TRUE;
    motor1EncoderCounts = 0;
    motor2EncoderCounts = 0;
    counterInitialise(counts,counts); //Sets up counters to interrupt at right time
    M1_FD = DCMOTOR_IDEAL_DUTY_CYCLE;
    M1_BD = 0;
    M2_FD =  DCMOTOR_IDEAL_DUTY_CYCLE;
    M2_BD = 0;
    Motor_1_Encoder_Counts_Start();
    Motor_2_Encoder_Counts_Start();
    Drift_Check_Timer_Start();
    motorDutyCycleUpdate(M1_FD,M1_BD,M2_FD,M2_BD);//This gets the motors going
    //The below line just traps execution here until the motion is completed
    while ((motor1Enable == TRUE)||(motor2Enable == TRUE)){
    
    }
}

//Non-blocking move function. It will be called and will cause the robot to move forward until 
//interrupted out by ultrasonics
void moveForwardIndefinitely(void){
    int counts = ENCODER_COUNTS_MAX;
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    drivingForwardFlag = TRUE;
    motor1EncoderCounts = 0;
    motor2EncoderCounts = 0;
    counterInitialise(counts,counts); //Sets up counters to interrupt at right time
    M1_FD = DCMOTOR_IDEAL_DUTY_CYCLE;
    M1_BD = 0;
    M2_FD =  DCMOTOR_IDEAL_DUTY_CYCLE;
    M2_BD = 0;
    Motor_1_Encoder_Counts_Start();
    Motor_2_Encoder_Counts_Start();
    Drift_Check_Timer_Start();
    motorDutyCycleUpdate(M1_FD,M1_BD,M2_FD,M2_BD);//This gets the motors going
    //The below line just traps execution here until the motion is completed
}
//Takes a distance to move in cm and moves. Blocking (sadly)
void moveBackward(float amount){
    int counts = amount/CM_PER_COUNT;
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    drivingBackwardFlag = TRUE;
    motor1EncoderCounts = 0;
    motor2EncoderCounts = 0;
    counterInitialise(counts,counts); //Sets up counters to interrupt at right time
    M1_FD = 0;
    M1_BD = DCMOTOR_IDEAL_DUTY_CYCLE;
    M2_FD =  0;
    M2_BD = DCMOTOR_IDEAL_DUTY_CYCLE;
    Motor_1_Encoder_Counts_Start();
    Motor_2_Encoder_Counts_Start();
    Drift_Check_Timer_Start();
    motorDutyCycleUpdate(M1_FD,M1_BD,M2_FD,M2_BD);//This gets the motors going
    //The below line just traps execution here until the motion is completed
    while ((motor1Enable == TRUE)||(motor2Enable == TRUE)){
    
    }
}

//Allows the robot to move backward indefinitely until interrupted out by ultrasonics
void moveBackwardIndefinitely(void){
    int counts = ENCODER_COUNTS_MAX;
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    drivingBackwardFlag = TRUE;
    motor1EncoderCounts = 0;
    motor2EncoderCounts = 0;
    counterInitialise(counts,counts); //Sets up counters to interrupt at right time
    M1_FD = 0;
    M1_BD = DCMOTOR_IDEAL_DUTY_CYCLE;
    M2_FD =  0;
    M2_BD = DCMOTOR_IDEAL_DUTY_CYCLE;
    Motor_1_Encoder_Counts_Start();
    Motor_2_Encoder_Counts_Start();
    Drift_Check_Timer_Start();
    motorDutyCycleUpdate(M1_FD,M1_BD,M2_FD,M2_BD);//This gets the motors going
}

//Takes an angle to turn in degrees and turns left that much
void turnLeft(float amount){
    //We need to make sure that the timers are disabled when we are doing turning motions
    //Otherwise they will try and correct the turning
    int counts = round(amount/DEGREES_PER_COUNT);
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    turningLeftFlag = TRUE;
    motor1EncoderCounts = 0;
    motor2EncoderCounts = 0;
    counterInitialise(counts,counts); //Sets up counters to interrupt at right time
    M1_FD = 0;
    M1_BD = DCMOTOR_IDEAL_DUTY_CYCLE;
    M2_FD = DCMOTOR_IDEAL_DUTY_CYCLE;
    M2_BD = 0;
    Motor_1_Encoder_Counts_Start();
    Motor_2_Encoder_Counts_Start();
    Drift_Check_Timer_Start();
    motorDutyCycleUpdate(M1_FD,M1_BD,M2_FD,M2_BD);//This gets the motors going
    //The below line just traps execution here until the motion is completed
    while ((motor1Enable == TRUE)||(motor2Enable == TRUE)){
    
    }
}

void turnRight(float amount){
    int counts = round(amount/DEGREES_PER_COUNT);
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    turningRightFlag = TRUE;
    motor1EncoderCounts = 0;
    motor2EncoderCounts = 0;
    counterInitialise(counts,counts); //Sets up counters to interrupt at right time
    M1_FD = DCMOTOR_IDEAL_DUTY_CYCLE;
    M1_BD = 0;
    M2_FD = 0;
    M2_BD = DCMOTOR_IDEAL_DUTY_CYCLE;
    Motor_1_Encoder_Counts_Start();
    Motor_2_Encoder_Counts_Start();
    Drift_Check_Timer_Start();
    motorDutyCycleUpdate(M1_FD,M1_BD,M2_FD,M2_BD);//This gets the motors going
    //The below line just traps execution here until the motion is completed
    while ((motor1Enable == TRUE)||(motor2Enable == TRUE)){
    
    }
}

//The displaceLeft and displaceRight functions should let us slide left and right. 
//We should ensure that these are only called when we are not close enough to the walls
//to create issues otherwise we could get stuck in these functions permanently
void displaceLeft(int amount,int degrees){
    turnRight(degrees);
    float temp = amount/sin((M_PI/180)*degrees);
    moveBackward(amount/sin((M_PI/180)*degrees));
    turnLeft(degrees);
    temp = amount/tan((M_PI/180)*degrees);
    moveForward(amount/tan((M_PI/180)*degrees));
}

//The displaceLeft and displaceRight functions should let us slide left and right. 
//We should ensure that these are only called when we are not close enough to the walls
//to create issues otherwise we could get stuck in these functions permanently
void displaceRight(int amount,int degrees){
    turnLeft(degrees);
    float temp = amount/sin((M_PI/180)*degrees);
    moveBackward(amount/sin((M_PI/180)*degrees));
    turnRight(degrees);
    temp = amount/tan((M_PI/180)*degrees);
    moveForward(amount/tan((M_PI/180)*degrees));
}

//Looks at the current position, desired position and the location of the block, 
//and plans a route, executing the next motion in that route if the main program 
//permits (via having the moveNow flag set to TRUE)
void nextMotion(int * desiredPosition){
    //Check that we have already acquired the block's location (by checking we aren't in 
    //the scanning for block state)
    //Check what motions are available to us from the flags
    //Check what motions are available to us from our location compared to obstacle
    if (block_location[0] < currentPosition[0] && currentPosition[0] < block_location[1]){
        float xToMove = desiredPosition[0] - currentPosition[0];
        if ((xToMove <0) && (moveLeftAllowed == TRUE)){
            if (currentOrientation != 180){
                turnLeft(180 - currentOrientation);
            }
        }
    }
}

void stopMotor1AndUpdate(void){
    Motor_1_driver_WriteCompare1(0);
    Motor_1_driver_WriteCompare2(0);
    Motor_1_Encoder_Counts_Stop();
    M1_FD = 0;
    M1_BD = 0;
    UART_1_PutString("Counter 1 interrupt \n");
    motor1Enable = FALSE;
    stopMotion();
    Motor_1_Encoder_Counts_ReadStatusRegister(); //Clears the interrupt
}

void stopMotor2AndUpdate(void){
    Motor_2_driver_WriteCompare1(0);
    Motor_2_driver_WriteCompare2(0);
    Motor_2_Encoder_Counts_Stop();
    M2_FD = 0;
    M2_BD = 0;
    UART_1_PutString("Counter 2 interrupt \n");
    motor2Enable = FALSE;   
    stopMotion();
    Motor_2_Encoder_Counts_ReadStatusRegister(); //Clears the interrupt
}

void moveAndAngle(int horizontal, int vertical, int angle){
    // TO COMPLETE NIDHIN
    UART_1_PutString("Move and Angle called");
    return;    
}

void correctAngle(){
    moveAndAngle(HOME_MIDPOINT, 20, WEST_ANGLE); // Choose west angle because nothing will be blocking the wall
    distanceCheck();
    // rotate by micro steps until front two sensors are equal within tolerance
    
    int error = abs(ultrasonic_distances[LEFT_FRONT] - ultrasonic_distances[RIGHT_FRONT]);
    while (error > ANGLE_TOLERANCE){
        // microTurn();
        error = abs(ultrasonic_distances[LEFT_FRONT] - ultrasonic_distances[RIGHT_FRONT]);
    }
    
}

/* [] END OF FILE */
