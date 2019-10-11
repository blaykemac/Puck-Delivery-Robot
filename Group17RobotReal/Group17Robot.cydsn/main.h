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

//Constants in main for dcmotor function
#define TRUE 1
#define FALSE 0
#define EAST_DIRECTION 0 //This is just to remind us that we have our east as zero and
//angle increases counterclockwise (so toward the north of the arena will be 90)
#define CM_PER_COUNT 0.0187 //0.0175*1.06849 cm per encoder count
#define DEGREES_PER_COUNT 0.105 //11.465 cm per deg*0.0187 cm per count (equal to CM_PER_COUNT)
#define DRIFT_CORRECT_FREQ 10
#define DCMOTOR_PWM_PERIOD 300
#define DCMOTOR_IDEAL_DUTY_CYCLE 80
#define NUM_TO_PERCENT_CONVERTER 100
#define DRIFT_ADJUST_THRESHOLD 5 //If the timer counts discrepancy between the timers is greater than this, we adjust
#define ENCODER_COUNTS_MAX 65500 //The maximum number of counts the encoder can have
#define COLLISION_THRESHOLD 5 //Distance in cm from any obstacle that we should stop

// Main states for overall navigation and motor control

#define STATE_SCAN_PLAN 1
#define STATE_LOCATE_BLOCK_AND_PUCKS 2
#define STATE_GO_TO_PUCKS 3
#define STATE_FIND_REQUIRED_PUCK 4
#define STATE_DEPOSIT_PUCK 5
#define STATE_PARK_HOME 6

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

// These are the numbers that represent the angle we are facing taken from the CCW vector pointing east
#define NORTH_ANGLE 90
#define EAST_ANGLE 0
#define SOUTH_ANGLE 270
#define WEST_ANGLE 180


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
#define HOME_MIDPOINT (ARENA_WIDTH / 2)
#define HOME_PARKING_DISTANCE 3

#define CONSTRUCTION_BASE_NORTH_EDGE CONSTRUCTION_EDGE_LENGTH
#define CONSTRUCTION_BASE_SOUTH_EDGE 0
#define CONSTRUCTION_BASE_WEST_EDGE 20 // Measure arena to find this edge
#define CONSTRUCTION_BASE_EAST_EDGE (CONSTRUCTION_BASE_WEST_EDGE + CONSTRUCTION_EDGE_LENGTH)
#define CONSTRUCTION_MIDPOINT ((CONSTRUCTION_BASE_WEST_EDGE + CONSTRUCTION_BASE_EAST_EDGE + CONSTRUCTION_EDGE_LENGTH) / 2)
#define CONSTRUCTION_DISTANCE_FROM_WALL 5 // How far we should put the front of the robot to the wall when depositing hte puck

#define DISTANCE_PUCKS_FROM_NORTH 28 // Where the southmost part of the pucks can extend to from the north wall


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
#define FRONT_CLAW_DISTANCE_FROM_CENTRE
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
extern int initialisation;

extern void lowerAndOpen(int puck_stack_position);
extern void closeAndRaise(int puck_stack_position);

char output[32];



