
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
void moveUntilPuck(int algorithm);
void straightAdjust(void);
void straightAdjustBack(void);          // adjust using the back sensor
void moveUntil(int distance, int direction, int less_or_great, int ultrasonic_sensor, int speed, int activate_safety);           // incorporates the ultrasonics
void translateUntil(int distance_set, int direction, int less_or_great, int ultrasonic_sensor, int speed);
void locatePucks(void);
void changeOrientation(int orientation_change, int speed);
int failsafe(int direction);
void toleranceCheck(void);
void blockAndPuckZoneFinding(void);
void straightAdjustSensor(int sensor);
void failsafeSideSensors(int side_sensing, int initial_value);

extern int puckConstructionPlan[3];
extern int current_stage;
extern int colour_sensing_algorithm;
extern int internal_orientation;
extern int safety_override;

