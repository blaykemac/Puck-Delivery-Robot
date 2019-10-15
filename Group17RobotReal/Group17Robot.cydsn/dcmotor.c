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
#include "time.h"

//global variable currentPosition and currentOrientation (in main). Declare as externs in 
//dcmotor, I guess
extern float currentPosition[2];
extern float desiredPosition[2];
extern float currentOrientation; //in degrees (convert to radians when needed)
extern float desiredOrientation;
extern short int moveNow; //This is a flag that lets the main program tell the moving functions
//whether we want the robot to be moving or not. For example, when we need to operate servos
//the main program would set moveNow to FALSE.
extern float block_location[4]; //This will hold the co-ordinates for the obstacle block. 0,1 will be xmin
//and xmax, 2,3 will be ymin, ymax.
extern int state; //The state of the robot as defined in main

//The four values shown in the array below will help everyone keep track of 
//the duty cycles
int motorDutyCycles[4] = {0,0,0,0}; 
short int motor1Enable = 0; //These two will be on if the motors are on
short int motor2Enable = 0;
int motor1EncoderCounts; //These two variables will allow dcmotor functions to keep 
int motor2EncoderCounts; //track of how many turns the motors have spun  

//These flags will be used by the main program to allow the robot to move in certain directions
extern short int moveLeftAllowed;
extern short int moveRightAllowed;
extern short int moveForwardAllowed;
extern short int moveBackwardAllowed;

extern char output[32]; //for the UART

//This is the function that will be called to handle the interrupt when counter 1 triggers
//It needs to stop motor1 and set the motor1Enable flag to zero. It should be atomic
void encoderCounts1InterruptHandler(void){
    CyGlobalIntDisable;
    stopMotors(MOTOR_1);
    motor1Enable = FALSE;
    Motor_1_Encoder_Counts_ReadStatusRegister();
    CyGlobalIntEnable;
}

//This is the function that will be called to handle the interrupt when counter 1 triggers
//It needs to stop motor1 and set the motor1Enable flag to zero. It should be atomic
void encoderCounts2InterruptHandler(void){
    CyGlobalIntDisable;
    stopMotors(MOTOR_2);
    motor2Enable = FALSE;
    Motor_2_Encoder_Counts_ReadStatusRegister();
    CyGlobalIntEnable;
}

//Given the compare values of the two motors' forward directions, and knowing that both are on, 
//we can find out what the motion we are performing is. 
int decodeMotion(int M1_FD, int M2_FD){
    if ((M1_FD > 0) && (M2_FD > 0)){
        return MOVING_FORWARD;
    }else if ((M1_FD == 0) && (M2_FD > 0)){
        return TURNING_LEFT;
    }else if ((M1_FD > 0) && (M2_FD == 0)){
        return TURNING_RIGHT;
    }else if ((M1_FD == 0) && (M2_FD == 0)){
        return MOVING_BACKWARD;
    }else{
        return SOMETHING_WENT_WRONG;
    }
}


//This function takes a number of counts from the encoders and, assuming that the number of 
//counts has been moved in the currentOrientation direction, updates the current position. 
//Current position can only be updated from within this function. The positionUpdateTemp 
//variable is used to keep track of how many counts we have moved. 
void updatePosition(int encoder1Counts, int encoder2Counts, float * position, float orientation, int * positionUpdateTemp){
    float orientation_Radians = (M_PI*orientation/180);
    int encoderCounts = round((encoder1Counts + encoder2Counts)/2);
    position[0] += CM_PER_COUNT*(encoderCounts - *positionUpdateTemp)*cos(orientation_Radians); 
    position[1] += CM_PER_COUNT*(encoderCounts - *positionUpdateTemp)*sin(orientation_Radians);
    *positionUpdateTemp = encoderCounts;
    /*
    UART_1_PutString("X pos: ");
    sprintf(output,"%f",position[0]);
    UART_1_PutString(output);
    UART_1_PutString("\tY pos: ");
    sprintf(output,"%f",position[1]);
    UART_1_PutString(output);
    */
}

//This function takes a number of counts from the encoder, and updates the currentOrientation. 
//The direction integer in the argument list below indicates if we are turning left or right. 
//0 = left, 1 = right.
void updateOrientation(int encoder1Counts, int encoder2Counts, int direction, float * orientation, int * orientationUpdateTemp){
    int encoderCounts = round((encoder1Counts + encoder2Counts)/2);
    if (direction == LEFT){ 
        *orientation += DEGREES_PER_COUNT*(encoderCounts - *orientationUpdateTemp);
        if (*orientation > 360){
            *orientation = *orientation - 360;
        }
    }else if (direction == RIGHT){ 
        *orientation -= DEGREES_PER_COUNT*(encoderCounts - *orientationUpdateTemp);
        //*orientation = *orientation%360;
        if (*orientation < 0){
            *orientation = 360 + *orientation;
        }
    }
    *orientationUpdateTemp = encoderCounts;
}

//Takes two integers count1 and count2, and sets the counters up so that they will start counting 
//at zero and interrupt when their counts equal the count values specified. Also initialise the 
//global variables that keep track of the number of counts
void initialiseCounters(int count1, int count2){
    CyGlobalIntDisable; //Whether we should use CyEnterCriticalSection is debatable - try and see
    motor1EncoderCounts = 0;
    motor2EncoderCounts = 0;
    Motor_1_Encoder_Counts_WritePeriod(count1);
    Motor_2_Encoder_Counts_WritePeriod(count2);
    Motor_1_Encoder_Counts_WriteCounter(0); //Start the counters from 0
    Motor_2_Encoder_Counts_WriteCounter(0); //Start the counters from 0
    CyGlobalIntEnable;
}

//Functions to start and stop the encoder counters
void startCounters(void){
    CyGlobalIntDisable;
    Motor_1_Encoder_Counts_Start();
    Motor_2_Encoder_Counts_Start();
    CyGlobalIntEnable;
}

void stopCounters(void){
    CyGlobalIntDisable;
    Motor_1_Encoder_Counts_Stop();
    Motor_2_Encoder_Counts_Stop();
    CyGlobalIntEnable;
}

void wipeCounters(void){
    CyGlobalIntDisable;
    motor1EncoderCounts = 0;
    motor2EncoderCounts = 0;
    Motor_1_Encoder_Counts_WriteCounter(0);
    Motor_2_Encoder_Counts_WriteCounter(0);
    CyGlobalIntEnable;
}

//Takes the duty cycles must be expressed as a percentage and updates the compare values of the
//PWM drivers (if the motor1Enable and motor2Enable are true). When the motors are running, 
//this will ensure a change if the motors are on. If not, it will initialise the duty cycles.
void updateMotorDutyCycles(float M1_fd, float M1_bd, float M2_fd, float M2_bd){
    if ((motor1Enable == TRUE) && (motor2Enable == TRUE)){
        CyGlobalIntDisable;
        Motor_1_driver_WriteCompare1((M1_fd/NUM_TO_PERCENT_CONVERTER)*DCMOTOR_PWM_PERIOD);
        Motor_2_driver_WriteCompare1((M2_fd/NUM_TO_PERCENT_CONVERTER)*DCMOTOR_PWM_PERIOD);
        Motor_1_driver_WriteCompare2((M1_bd/NUM_TO_PERCENT_CONVERTER)*DCMOTOR_PWM_PERIOD);
        Motor_2_driver_WriteCompare2((M2_bd/NUM_TO_PERCENT_CONVERTER)*DCMOTOR_PWM_PERIOD);
        CyGlobalIntEnable;
    }
}

//Functions to start and stop the motors
void startMotors(void){
    CyGlobalIntDisable;
    Motor_1_driver_Start();
    Motor_2_driver_Start();
    CyGlobalIntEnable;
}

void stopMotors(int number){
    CyGlobalIntDisable;
    if (number == MOTOR_1){
        Motor_1_driver_Stop();
    }else if (number == MOTOR_2){
        Motor_2_driver_Stop();
    }else if (number == BOTH_1_AND_2){
        Motor_1_driver_Stop();
        Motor_2_driver_Stop();
    }
    CyGlobalIntEnable;
}

//This function should allow us to smoothly increase/decrease our speed when we move. 
//We take the total number of counts we want to move, as well as the number of counts that we have moved
//Taking the ratio of these, we can figure out what percentage of the relevant distance we have
//moved. Then we adjust speed accordingly. 
int decideMotorDutyCycles(float encoder1Counts, float encoder2Counts, float encoderCountsMax){
    //Only update speeds if both the motors are still moving should we update speed
    //This ensures that motors dont start up again when they stop
    float encoderCountsPercent = 100*((encoder1Counts + encoder2Counts)/2/encoderCountsMax);
    if ((motor1Enable == TRUE) && (motor2Enable == TRUE)){
        //Take the average of the two encoders' counts to get encoder counts
        //Take the ratio to find the percentage of the distance that we have moved
        return round((-SPEED_ADJUST_CONSTANT*encoderCountsPercent*(encoderCountsPercent - 100) + DCMOTOR_MIN_DUTY_CYCLE));
        //This gives us a parabolic speed characteristic: start slow, finish slow but be
        //quick in the middle.
    }else{
        return 0;
    }
}

//This function will be responsible for adjusting any drift. It will also update the speed
void driftCorrectAndSpeedUpdate(float encoder1Counts, float encoder2Counts, float encoderCountsMax){
    int baseline = decideMotorDutyCycles(encoder1Counts,encoder2Counts,encoderCountsMax);
        if ((motor1Enable == TRUE) && (motor2Enable == TRUE)){
        //We don't have to check that speedDecide doesn't give us zero because it can  only 
        //do that when one of the motorEnable flags is FALSE. Otherwise, if speedDecide was
        //giving us zero, we could write very bad duty cycle values from the below logic
        
        //From reading the forward duty cycles of the two motors, and knowing that both are on
        //we can decide what motion it is. 
        CyGlobalIntDisable;
        int M1_FD = Motor_1_driver_ReadCompare1();
        int M2_FD = Motor_2_driver_ReadCompare1();
        int motion = decodeMotion(M1_FD,M2_FD);            
        if (encoder1Counts > encoder2Counts){
            if (motion == MOVING_FORWARD){
                updateMotorDutyCycles(baseline - 1,0,baseline,0);       
            }else if (motion == MOVING_BACKWARD){
                updateMotorDutyCycles(0,baseline - 1,0,baseline);
            }else if (motion == TURNING_LEFT){
                updateMotorDutyCycles(0,baseline - 1,baseline,0);
            }else if (motion == TURNING_RIGHT){
                updateMotorDutyCycles(baseline - 1,0,0,baseline);                            
            }else{
                UART_1_PutString("DriftCorrect detects some strange motion");
            }
        }else if (motor2EncoderCounts > motor1EncoderCounts){
            if (motion == MOVING_FORWARD){
                updateMotorDutyCycles(baseline,0,baseline - 1,0);       
            }else if (motion == MOVING_BACKWARD){
                updateMotorDutyCycles(0,baseline,0,baseline - 1);
            }else if (motion == TURNING_LEFT){
                updateMotorDutyCycles(0,baseline,baseline - 1,0);
            }else if (motion == TURNING_RIGHT){
                updateMotorDutyCycles(baseline,0,0,baseline - 1);                            
            }else{
                UART_1_PutString("DriftCorrect detects some strange motion");
            }
        }else{
            if (motion == MOVING_FORWARD){
                updateMotorDutyCycles(baseline,0,baseline ,0);       
            }else if (motion == MOVING_BACKWARD){
                updateMotorDutyCycles(0,baseline,0,baseline);
            }else if (motion == TURNING_LEFT){
                updateMotorDutyCycles(0,baseline,baseline,0);
            }else if (motion == TURNING_RIGHT){
                updateMotorDutyCycles(baseline,0,0,baseline);                            
            }else{
                UART_1_PutString("DriftCorrect detects some strange motion");
            }
        }
        CyGlobalIntEnable;
    }
}

//Takes a distance to move in cm and moves. Blocking.
void moveForward(float amount){
    int counts = round(amount/CM_PER_COUNT);
    int positionUpdateTemp = 0; //This variable is set to zero when we start, and changed
    //to reflect how many counts the robot has gone up inside the loop.
    int speedUpdateTemp;
    float ultrasonicTemp;
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    //Can only initialise the counters after starting the counters
    startCounters(); //Start counters
    initialiseCounters(counts,counts); //Sets up counters to interrupt at right time
    //We can only initialise the duty cycles after starting the motor
    startMotors(); //Start motors
    updateMotorDutyCycles(DCMOTOR_MIN_DUTY_CYCLE,0,DCMOTOR_MIN_DUTY_CYCLE,0); 
    while ((motor1Enable == TRUE)||(motor2Enable == TRUE)){
        UART_1_PutString("RUNS");
        //clock_t begin = clock();
        motor1EncoderCounts = Motor_1_Encoder_Counts_ReadCounter();
        motor2EncoderCounts = Motor_2_Encoder_Counts_ReadCounter();
        //Correct drift
        driftCorrectAndSpeedUpdate(motor1EncoderCounts, motor2EncoderCounts, Motor_1_Encoder_Counts_ReadPeriod());
        //Update Position (not orientation since that doesn't change)
        updatePosition(motor1EncoderCounts, motor2EncoderCounts, currentPosition, currentOrientation, &positionUpdateTemp);
    }
    //Both the motors are assumed to be stopped by this point. So we don't stop motors
    //Only one of the ultrasonic interrupts or the encoder interrupts must fire. 
    //If both fire, we could potentially have issues with the PWM hardware. We have tried 
    //to address this issue by having the interrupt handlers do something only if the 
    //motorEnable flags are true but there is a very low possibility that things could hit
    //the fan.
    
    //We do, however, need to stop the counters (but not set them to zero as that is done
    //at the start of the next motion.
    updatePosition(motor1EncoderCounts, motor2EncoderCounts, currentPosition, currentOrientation, &positionUpdateTemp);
    wipeCounters();
    stopCounters();
}

void moveBackwardIndefinitely(void){
    int counts = 65500; //Maximum counts
    int positionUpdateTemp = 0; 
    int speedUpdateTemp;
    float ultrasonicTemp;
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    startCounters(); 
    initialiseCounters(counts,counts); 
    startMotors(); 
    updateMotorDutyCycles(0,DCMOTOR_MIN_DUTY_CYCLE,0,DCMOTOR_MIN_DUTY_CYCLE);
    sensorToCheck = 4;
    while ((motor1Enable == TRUE)||(motor2Enable == TRUE)){
        UART_1_PutString("RUNS");
        //clock_t begin = clock();
        motor1EncoderCounts = Motor_1_Encoder_Counts_ReadCounter();
        motor2EncoderCounts = Motor_2_Encoder_Counts_ReadCounter();
        //Correct drift
        driftCorrectAndSpeedUpdate(motor1EncoderCounts, motor2EncoderCounts, Motor_1_Encoder_Counts_ReadPeriod());
        //Update Position (not orientation since that doesn't change)
        updatePosition(motor1EncoderCounts, motor2EncoderCounts, currentPosition, currentOrientation, &positionUpdateTemp);
        distanceCheck(&sensorToCheck);
        if (sensorDistance < COLLISION_THRESHOLD + 2 ){
            stopMotors(BOTH_1_AND_2);
            motor1Enable = FALSE;
            motor2Enable = FALSE;
        }
    }
    updatePosition(motor1EncoderCounts, motor2EncoderCounts, currentPosition, currentOrientation, &positionUpdateTemp);
    wipeCounters();
    stopCounters();
}

void moveForwardIndefinitely(void){
    int counts = 65500; //Maximum counts
    int positionUpdateTemp = 0; 
    int speedUpdateTemp;
    float ultrasonicTemp;
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    startCounters(); 
    initialiseCounters(counts,counts); 
    startMotors(); 
    updateMotorDutyCycles(DCMOTOR_MIN_DUTY_CYCLE,0,DCMOTOR_MIN_DUTY_CYCLE,0);
    sensorToCheck = 1;
    while ((motor1Enable == TRUE)||(motor2Enable == TRUE)){
        UART_1_PutString("RUNS");
        //clock_t begin = clock();
        motor1EncoderCounts = Motor_1_Encoder_Counts_ReadCounter();
        motor2EncoderCounts = Motor_2_Encoder_Counts_ReadCounter();
        //Correct drift
        driftCorrectAndSpeedUpdate(motor1EncoderCounts, motor2EncoderCounts, Motor_1_Encoder_Counts_ReadPeriod());
        //Update Position (not orientation since that doesn't change)
        updatePosition(motor1EncoderCounts, motor2EncoderCounts, currentPosition, currentOrientation, &positionUpdateTemp);
        distanceCheck(&sensorToCheck);
        if (sensorDistance < COLLISION_THRESHOLD + 2 ){
            stopMotors(BOTH_1_AND_2);
            motor1Enable = FALSE;
            motor2Enable = FALSE;
        }
        if (sensorToCheck == 1){
            sensorToCheck = 2;
        }else {
            sensorToCheck = 1;
        }
    }
    updatePosition(motor1EncoderCounts, motor2EncoderCounts, currentPosition, currentOrientation, &positionUpdateTemp);
    wipeCounters();
    stopCounters();
}

void moveBackward(float amount){
    int counts = round(amount/CM_PER_COUNT);
    int positionUpdateTemp = 0; 
    int speedUpdateTemp;
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    startCounters(); 
    initialiseCounters(counts,counts); 
    startMotors(); 
    updateMotorDutyCycles(0,DCMOTOR_MIN_DUTY_CYCLE,0,DCMOTOR_MIN_DUTY_CYCLE); 
    while ((motor1Enable == TRUE)||(motor2Enable == TRUE)){
        motor1EncoderCounts = Motor_1_Encoder_Counts_ReadCounter();
        motor2EncoderCounts = Motor_2_Encoder_Counts_ReadCounter();
        driftCorrectAndSpeedUpdate(motor1EncoderCounts, motor2EncoderCounts, Motor_1_Encoder_Counts_ReadPeriod());
        updatePosition(motor1EncoderCounts, motor2EncoderCounts, currentPosition, currentOrientation, &positionUpdateTemp);
        //Poll ultrasonics
    }
    updatePosition(motor1EncoderCounts, motor2EncoderCounts, currentPosition, currentOrientation, &positionUpdateTemp);
    wipeCounters();
    stopCounters();
}

//Takes an angle to turn in degrees and turns left that much
void turnLeft(float amount){
    int counts = round(amount/DEGREES_PER_COUNT);
    int orientationUpdateTemp = 0; 
    int speedUpdateTemp;
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    startCounters(); 
    initialiseCounters(counts,counts); 
    startMotors(); 
    updateMotorDutyCycles(0,DCMOTOR_MIN_DUTY_CYCLE,DCMOTOR_MIN_DUTY_CYCLE,0);
    while ((motor1Enable == TRUE)||(motor2Enable == TRUE)){
        motor1EncoderCounts = Motor_1_Encoder_Counts_ReadCounter();
        motor2EncoderCounts = Motor_2_Encoder_Counts_ReadCounter();
        driftCorrectAndSpeedUpdate(motor1EncoderCounts, motor2EncoderCounts, Motor_1_Encoder_Counts_ReadPeriod());
        updateOrientation(motor1EncoderCounts, motor2EncoderCounts, LEFT, &currentOrientation, &orientationUpdateTemp);
        //Poll ultrasonics
    }
    updateOrientation(motor1EncoderCounts, motor2EncoderCounts, LEFT, &currentOrientation, &orientationUpdateTemp);
    currentOrientation = round(currentOrientation);
    wipeCounters();
    stopCounters();
}

void turnRight(float amount){
    int counts = round(amount/DEGREES_PER_COUNT);
    int orientationUpdateTemp = 0; 
    int speedUpdateTemp;
    motor1Enable = TRUE;
    motor2Enable = TRUE;
    startCounters();
    initialiseCounters(counts,counts);
    startMotors();
    updateMotorDutyCycles(DCMOTOR_MIN_DUTY_CYCLE,0,0,DCMOTOR_MIN_DUTY_CYCLE); 
    while ((motor1Enable == TRUE)||(motor2Enable == TRUE)){
        motor1EncoderCounts = Motor_1_Encoder_Counts_ReadCounter();
        motor2EncoderCounts = Motor_2_Encoder_Counts_ReadCounter();
        driftCorrectAndSpeedUpdate(motor1EncoderCounts, motor2EncoderCounts, Motor_1_Encoder_Counts_ReadPeriod());
        updateOrientation(motor1EncoderCounts, motor2EncoderCounts, RIGHT, &currentOrientation, &orientationUpdateTemp);
        //Poll ultrasonics
    }
    updateOrientation(motor1EncoderCounts, motor2EncoderCounts, RIGHT, &currentOrientation, &orientationUpdateTemp);
    currentOrientation = round(currentOrientation);
    wipeCounters();
    stopCounters();
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

/* [] END OF FILE */
