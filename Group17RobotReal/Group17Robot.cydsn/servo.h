
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

void armTranslate(int new_position);
void changeHeightToPuck(int puckHeightIndex); // 0 is ground, 2 will be maximum
void armOpen(void);
void armClose(void);

void armMoveUpDown(int desiredPosition);

//These are the values that correspond to open, closed, up and down positions of the 
//arm. These were found by experiment, so if need to recalibrate, you know where to go
#define LOW_POS_ARM_CMP 2400 
#define HIGH_POS_ARM_CMP 1000
#define OPEN_POS_GRIPPER_CMP 1120 //1170
#define CLOSED_POS_GRIPPER_CMP 1250

