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
void moveDynamic(int distance, int speed, int activate_safety);    // positive number to move forward by that distance, negative to move backwards
void translateMoveDynamic(int distance, int degree, int speed, int activate_safety);    // positive number to move right by that distance, negative to move left
void moveSwivel(int degrees, int speed, int activate_safety);
void rampUpDown(int speed, int ramp_up_down);

// set speeds: 

#define SPEED_LOW 70                
#define SPEED 200                   // DO NOT PUT ABOVE 200
#define SPEED_MAX 225

#define DELAY 60
#define ENCODER_MULTIPLIER 5.29

#define RAMP_UP 0
#define RAMP_DOWN 1


/* [] END OF FILE */
