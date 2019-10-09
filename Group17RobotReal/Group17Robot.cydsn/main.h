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
// In case we change the number of ultrasonic sensors used
#define TOTAL_SONIC_SENSORS 5

// Main states for overall navigation and motor control
#define STATE_FIND_PUCKS 1
#define STATE_SCAN_PLAN 2
#define STATE_LOCATE_BLOCK 3
#define STATE_GO_TO_PUCKS 4

// Hard-coded distances from the west for where each puck may sit in the construction plan.
#define PUCK_RACK_0_WEST_DISTANCE 72
#define PUCK_RACK_1_WEST_DISTANCE 66
#define PUCK_RACK_2_WEST_DISTANCE 60
#define PUCK_RACK_3_WEST_DISTANCE 54
#define PUCK_RACK_4_WEST_DISTANCE 48


// Cardinal Directions. North faces the pucks from the home base
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3


// * ARENA CONSTRAINTS IN CM !* //
#define MIN_OBJECT_SOUTH 50
#define MAX_OBJECT_NORTH 70

#define ARENA_WIDTH 120
#define ARENA_LENGTH 120

#define BLOCK_LENGTH 30
#define BLOCK_WIDTH 10

#define HOME_EDGE_LENGTH 25
#define CONSTRUCTION_EDGE_LENGTH 10

#define HOME_BASE_NORTH_EDGE HOME_EDGE_LENGTH
#define HOME_BASE_SOUTH_EDGE 0
#define HOME_BASE_WEST_EDGE (ARENA_WIDTH - HOME_EDGE_LENGTH) / 2   // Measure arena to find this edge
#define HOME_BASE_EAST_EDGE (HOME_BASE_WEST_EDGE + HOME_EDGE_LENGTH)

#define CONSTRUCTION_BASE_NORTH_EDGE CONSTRUCTION_EDGE_LENGTH
#define CONSTRUCTION_BASE_SOUTH_EDGE 0
#define CONSTRUCTION_BASE_EAST_EDGE 20 // Measure arena to find this edge
#define CONSTRUCTION_BASE_WEST_EDGE (CONSTRUCTION_BASE_EAST_EDGE + CONSTRUCTION_EDGE_LENGTH)


// Robot Body Orientation Constants
#define LEFT_SIDE 0
#define LEFT_FRONT 1
#define RIGHT_FRONT 2
#define RIGHT_SIDE 3
#define BACK 4

#define SIDE_SENSORS_WIDTH 25

// Miscellaneous Constants
#define SAFETY_MARGIN 10 // The margin we will enforce be kept around us clear at all times.
#define WIDTH_SENSOR_TO_SENSOR 22 // The distance measured from the very outer edge of each side ultrasonic sensor.
#define DISPLACE_DISTANCE_NW_PUCK_CHECK_CM 3
#define BLOCK_TOLERANCE 10 // Used for sweeping block location. This affects sensitivity in detecting block.
#define SIDE_SENSOR_OFFSET_FROM_BACK 15 // Distance of the midpoint of the side ultrasonic sensors to the far rear sensor edge
#define PUCK_TOLERANCE 15 // Used for initially sweeping puck location. This affects sensitivity in detecting pucks.


// Variables to be shared / imported
extern float ultrasonic_distances[TOTAL_SONIC_SENSORS];
int driveStraightEnable;
int currentPuckRackScanningIndex;
int puckRackColours[5];
int puckRackOffsetsFromWest[5];
int sweeping;
float block_edge_location[4];
float puckPileLocation;
