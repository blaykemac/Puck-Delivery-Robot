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
void encoderCounts1InterruptHandler(void);
void encoderCounts2InterruptHandler(void);
int decodeMotion(int M1_FD, int M2_FD);
void updatePosition(int encoder1Counts, int encoder2Counts, float * position, float orientation, int * positionUpdateTemp);
void updateOrientation(int encoder1Counts, int encoder2Counts, int direction, float * orientation, int * orientationUpdateTemp);
void stopMotion(void);
void initialiseCounters(int count1, int count2);
void startCounters(void);
void stopCounters(void);
void wipeCounter(void);
void updateMotorDutyCycles(float M1_fd, float M1_bd, float M2_fd, float M2_bd);
void startMotors(void);
void stopMotors(int number);
int decideMotorDutyCycles(float encoder1Counts, float encoder2Counts, float encoderCountsMax);
void driftCorrectAndSpeedUpdate(float encoder1Counts, float encoder2Counts, float encoderCountsMax);
void moveForward(float amount);
void moveForwardIndefinitely(void);
void moveBackward(float amount);
void moveBackwardIndefinitely(void);
void turnLeft(float amount);
void turnRight(float amount);
void displaceLeft(int amount,int degrees);
void displaceRight(int amount,int degrees);
void nextMotion(int * desiredPosition);
void stopMotor1AndUpdate(void);
void stopMotor2AndUpdate(void);
void moveAndAngle(float desiredX, float desiredY, float desiredOrient);
void correctAngle();

extern int sensorToCheck;
extern float sensorDistance;

#define ANGLE_TOLERANCE 0.01
#define SPEED_ADJUST_CONSTANT 0.024
#define DCMOTOR_MIN_DUTY_CYCLE 20
#define DCMOTOR_MAX_DUTY_CYCLE 80

//The below constants will be the arguments to stopMotors, allowing it to disable one or both
#define MOTOR_1 1
#define MOTOR_2 2
#define BOTH_1_AND_2 3

//The below constants will be used by the updateOrientation function
#define LEFT 0
#define RIGHT 1

//The below constants will be used by the motionDecode function to tell relevant functions
//what motion is being performed
#define MOVING_FORWARD 0
#define MOVING_BACKWARD 1
#define TURNING_LEFT 2
#define TURNING_RIGHT 3
#define SOMETHING_WENT_WRONG 4

//These are the buffer distances that we want to keep from obstacles
#define ROBOT_FRONT_BUFFER 11 //The furthest point from the base of the front ultras to the tip of the front colorSensor
#define ROBOT_BACK_BUFFER 3 //Buffer amount from the base of the back ultrasonic
#define ROBOT_LEFT_BUFFER 4 //15 +1
#define ROBOT_RIGHT_BUFFER 3 //13 + 1

/* [] END OF FILE */
