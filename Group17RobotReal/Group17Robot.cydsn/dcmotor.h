
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

#define ANGLE_TOLERANCE 0.01

void positionUpdate(int counts, int drivingForwardFlag, int drivingBackwardFlag);
void orientationUpdate(int counts,int turningLeftFlag, int turningRightFlag);
void stopMotion(void);
void motorDutyCycleUpdate(float M1_FD, float M1_BD, float M2_FD, float M2_BD);
void driftCorrect(void);
void counterInitialise(int count1, int count2);
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
void moveAndAngle(int horizontal, int vertical, int angle);
void correctAngle();


//global variable currentPosition and currentOrientation (in main). Declare as externs in 
//dcmotor, I guess
extern float currentPosition[2];
extern float desiredPosition[2];
extern int currentOrientation; //in degrees (convert to radians when needed)
extern int desiredOrientation;
short int motor1Enable; //These two will be on if the motors are on
short int motor2Enable;
int M1_FD; //The four values shown here will be the duty cycles of the motors
int M1_BD; //There are times when parts of the code (such as the driftCorrect function)
int M2_FD; //need to know about the duty cycles. So we make the duty cycles a globally 
int M2_BD; //known variable
int motor1EncoderCounts; //These two variables will allow the entire program to keep 
int motor2EncoderCounts; //track of how many turns the motors have spun 
extern short int moveNow; //This is a flag that lets the main program tell the moving functions
//whether we want the robot to be moving or not. For example, when we need to operate servos
//the main program would set moveNow to FALSE.
extern float block_location[4]; //This will hold the co-ordinates for the obstacle block. 0,1 will be xmin
//and xmax, 2,3 will be ymin, ymax.
extern int state; //The state of the robot as defined in main 

//The four flags below let the rest of the program know if the robot is trying to drive 
//forward or back, turn left or right. At the start of every motion, turn these flags on
//at the end turn them off.
extern short int drivingForwardFlag;
extern short int drivingBackwardFlag;
extern short int turningLeftFlag;
extern short int turningRightFlag;

//These flags will be used by the main program to allow the robot to move in certain directions
extern short int moveLeftAllowed;
extern short int moveRightAllowed;
extern short int moveForwardAllowed;
extern short int moveBackwardAllowed;

extern char output[32]; //for the UART

/* [] END OF FILE */
