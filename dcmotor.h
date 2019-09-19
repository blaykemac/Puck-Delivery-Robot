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

void motor_driver_compare_update(short m1_d1,short m1_d2,short m2_d1,short m2_d2);
void startMotion(short requiredCount);
void stopMotion(void);
void moveForward(short amount);
void moveBackward(short amount);
//void turnLeft(void);
//void turnRight(void);
void turnLeft(short amount);
void turnRight(short amount);
void updatePosition(short * currentPosition, short currentOrientation);
void moveClosedLoop(short desiredPosition[2]);
void moveForwardABit(void);
void moveBackwardABit(void);
void turnLeftABit(void);
void turnRightABit(void);
void displaceRight(int amount,int degrees);
void displaceLeft(int amount,int degrees);
void moveForwardIndefinitely(void);
void moveBackwardIndefinitely(void);
void correctAngleDrift();

extern int driveStraightEnable;
extern int state;



/* [] END OF FILE */
