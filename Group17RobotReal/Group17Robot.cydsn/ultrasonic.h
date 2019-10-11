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

/* Function headers */

#define OFFSET_COLOUR_SENSOR_FROM_FRONT 20 // cm
#define FINISH_CENTER_FROM_E 47
#define SENSOR_PULSE_DELAY_MS 50


void distanceCheck();  
void distanceSensor(int currentSensorIndex);
void printSensorToUART(int sensorNumber, float distanceMeasured);
float distanceFromCount(int timerCount);
void ultrasonicInterruptHandler(void);


// External variables (ie. globals)
extern int driveStraightEnable;
extern int state;
extern int currentPuckRackScanningIndex;
extern int puckRackColours[5];
extern int puckRackOffsetsFromWest[5];
extern int sweeping;
extern float block_edge_location[4];
extern int moving;

int initialisation;

