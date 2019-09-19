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
#define TOTAL_SONIC_SENSORS 5

#define STATE_GO_WEST 0
#define STATE_GO_NORTH_CAREFUL 1
#define STATE_GO_NORTH_CARELESS 2
#define STATE_SWEEP_NW_PUCKS 3
#define STATE_SWEEP_N_PUCKS 4
#define STATE_RETURN_HOME_SIDE 5
#define STATE_LAND_ON_HOME_BASE 6
#define STATE_FORWARD_TO_PUCK 7
#define STATE_RETURN_TO_NW_FROM_PUCK 8
#define STATE_FINISH_LANDING 9

// States for final comp

#define STATE_SCAN_PLAN 10

// Constants / Hard-coded values for final comp
#define PUCK_RACK_0_WEST_DISTANCE 72
#define PUCK_RACK_1_WEST_DISTANCE 66
#define PUCK_RACK_2_WEST_DISTANCE 60
#define PUCK_RACK_3_WEST_DISTANCE 54
#define PUCK_RACK_4_WEST_DISTANCE 48

// Colour Sensing Constants
#define BLANK 0
#define RED 1
#define GREEN 2
#define BLUE 3

// * ARENA CONSTRAINTS IN CM !* //
#define MIN_OBJECT_SOUTH 50
#define MAX_OBJECT_NORTH 70

#define ARENA_WIDTH 120
#define ARENA_LENGTH 120

#define BLOCK_LENGTH 30
#define BLOCK_WIDTH 10

#define HOME_EDGE_LENGTH 25
#define CONSTRUCTION_EDGE_LENGTH 10

// Assume block is square for prelim
#define BLOCK_WEST_EDGE (ARENA_WIDTH - BLOCK_LENGTH) / 2
#define BLOCK_EAST_EDGE (BLOCK_WEST_EDGE + BLOCK_LENGTH)
#define BLOCK_NORTH_EDGE (BLOCK_SOUTH_EDGE + BLOCK_LENGTH)
#define BLOCK_SOUTH_EDGE (ARENA_LENGTH - BLOCK_LENGTH) / 2

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

// Miscellaneous Constants
#define SAFETY_MARGIN 10
#define WIDTH_SENSOR_TO_SENSOR 22
#define DISPLACE_DISTANCE_NW_PUCK_CHECK_CM 3

// 

#define NW_PUCK_POSITION_FROM_WEST 49
#define NW_PUCK_POSITION_FROM_NORTH 26

extern float ultrasonic_distances[TOTAL_SONIC_SENSORS];
int driveStraightEnable;
extern int westWall;
int currentPuckRackScanningIndex;
int puckRackColours[5];
int puckRackOffsetsFromWest[5];
