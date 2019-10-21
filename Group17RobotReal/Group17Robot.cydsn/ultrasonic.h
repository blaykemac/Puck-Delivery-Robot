
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

/* Function headers */

#define OFFSET_COLOUR_SENSOR_FROM_FRONT 20 // cm
#define FINISH_CENTER_FROM_E 47
#define SENSOR_PULSE_DELAY_MS 50


void distanceCheck();  
void distanceSensor(int current_sensor_index);
void ultrasonicInterruptHandler(void);
void safetyDistanceCheck();

// External variables (ie. globals)
extern int state;
extern int puck_rack_scans[5];
extern int puck_rack_west_offsets[5];
extern int sweeping;
extern float block_edge_location[4];
extern int moving;
extern char output[32];

int initialisation;

int ultrasonic_flag;

