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

// Functions: 
void mishaMoveDynamic(int distance, int speed);    // positive number to move forward by that distance, negative to move backwards
void translateMoveDynamic(int distance, int degree, int speed);    // positive number to move right by that distance, negative to move left
void mishaSwivel(int degrees, int speed);

// set speeds: 

#define SPEED 170                   // DO NOT PUT ABOVE 200
#define SPEED_MAX 200

#define DELAY 60
#define ENCODER_MULTIPLIER 5.29

/* [] END OF FILE */
