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

#define OFFSET_COLOUR_SENSOR_FROM_FRONT 8 // cm
#define FINISH_CENTER_FROM_E 47
#define SENSOR_PULSE_DELAY_MS 50
#define BLOCK_WEST_EDGE 0
#define BLOCK_EAST_EDGE 1

void distanceCheck();  
void distanceSensor(int currentSensorIndex);
void printSensorToUART(int sensorNumber, float distanceMeasured);
float distanceFromCount(int timerCount);
void ultrasonicInterruptHandler(void);
int takeColourMeasurement(void);


// External variables (ie. globals)
extern int driveStraightEnable;
extern int northWall;
extern int state;
extern int currentPuckRackScanningIndex;
extern int puckRackColours[5];
extern int puckRackOffsetsFromWest[5];

