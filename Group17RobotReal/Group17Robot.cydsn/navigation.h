
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
void checkHoldingPuck(void);
void moveUntilPuck(int algorithm);

void moveUntil(int distance, int direction, int less_or_great, int ultrasonic_sensor, int speed, int activate_safety);           // incorporates the ultrasonics
void straightAdjust(int front_back);    // takes paramaters FRONT_SENSORS
void straightAdjustBack(void);          // adjust using the back sensor
void moveForwardThenBackward(int distance, int less_or_great,int ultrasonic_sensor, int speed, int activate_safety);

void translateUntil(int distance, float max_back, int left_or_right, int less_or_great);
void locatePucks(void);
void changeOrientation(int orientation_change, int speed);
int failsafe(int direction);
void toleranceCheck(void);
//void blockAndPuckZoneFinding(void);
void straightAdjustSensor(int sensor);
void failsafeSideSensors(int side_sensing, int initial_value);

extern int puck_construction_plan[3];
extern int current_stage;
extern int colour_sensing_algorithm;
extern int internal_orientation;
extern int safety_override;

extern int block_location[4];
extern int puck_location[4];

void straightAdjustBlayke(int front_back);
void moveForwardThenBackward(int distance, int less_great, int sensor, int speed, int activate_safety);

extern int global_encoder; 
extern int global_distance;