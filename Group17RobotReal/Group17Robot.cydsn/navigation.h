
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

void armMoving(void);
void moveUntilPuck(void);
void straightAdjust(void);
void moveUntil(int distance, int direction, int less_or_great, int ultrasonic_sensor, int speed);           // incorporates the ultrasonics
void locatePucks(void);
void changeOrientation(int orientation_change, int speed);

extern int puckConstructionPlan[3];
extern int current_stage;
extern int colour_sensing_algorithm;


