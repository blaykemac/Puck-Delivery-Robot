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

/* [] END OF FILE */
