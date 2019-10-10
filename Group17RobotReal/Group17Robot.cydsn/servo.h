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
void armUp(void);
void armClose(void);
void armOpen(void);
void armDown(void);
void armMoveUpDown(int desiredPosition);
void gripperAngle(int amount);
void rackAngle(int amount);
void changeHeightToPuck(int puckHeightIndex); // 0 is ground, 2 will be maximum
void lowerAndOpen(int puck_stack_position);
void closeAndRaise(int puck_stack_position);

/* [] END OF FILE */
