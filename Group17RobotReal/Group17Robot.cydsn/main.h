
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

//Constants in main for dcmotor function
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

#define STATE_PRE_RUN 0
#define STATE_SCAN_PLAN 1
#define STATE_LOCATE_BLOCK_AND_PUCKS 2
#define STATE_GO_TO_PUCKS 3
#define STATE_FIND_REQUIRED_PUCK 4
#define STATE_RETURN_TO_SOUTH 5
#define STATE_DEPOSIT_PUCK 6
#define STATE_PARK_HOME 7


// Hard-coded distances from the west for where each puck may sit in the construction plan.
#define PUCK_RACK_0_WEST_DISTANCE 720
#define PUCK_RACK_1_WEST_DISTANCE 660
#define PUCK_RACK_2_WEST_DISTANCE 600
#define PUCK_RACK_3_WEST_DISTANCE 540
#define PUCK_RACK_4_WEST_DISTANCE 480

#define SCAN_INITIALISE_HORIZONTAL 85 // The distance from the west that we should stop the robot during the black wall colour initialisation


// Cardinal Directions. North faces the pucks from the home base
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

int internal_orientation;   // this is the robots internal orientation

// These are the numbers that represent the angle we are facing taken from the CCW vector pointing east
#define NORTH_ANGLE 90
#define EAST_ANGLE 0
#define SOUTH_ANGLE 270
#define WEST_ANGLE 180

int internal_orientation;

#define EAST_DIRECTION 0 //This is just to remind us that we have our east as zero and angle increases counterclockwise (so toward the north of the arena will be 90)

// * ARENA CONSTRAINTS IN MM !* //
#define BLOCK_ZONE_SOUTH 500
#define BLOCK_ZONE_NORTH 700

#define ARENA_WIDTH 1200
#define ARENA_LENGTH 1200

#define BLOCK_LENGTH 250
#define BLOCK_WIDTH 90

#define HOME_EDGE_LENGTH 250
#define CONSTRUCTION_EDGE_LENGTH 100

#define HOME_BASE_NORTH_EDGE HOME_EDGE_LENGTH
#define HOME_BASE_SOUTH_EDGE 0
#define HOME_BASE_WEST_EDGE (ARENA_WIDTH - HOME_EDGE_LENGTH) / 2   // Measure arena to find this edge
#define HOME_BASE_EAST_EDGE (HOME_BASE_WEST_EDGE + HOME_EDGE_LENGTH)
#define HOME_MIDPOINT (ARENA_WIDTH / 2)
#define HOME_PARKING_DISTANCE 40

#define CONSTRUCTION_BASE_NORTH_EDGE CONSTRUCTION_EDGE_LENGTH
#define CONSTRUCTION_BASE_SOUTH_EDGE 0
#define CONSTRUCTION_BASE_WEST_EDGE 200 // Measure arena to find this edge
#define CONSTRUCTION_BASE_EAST_EDGE 300
#define CONSTRUCTION_MIDPOINT 250
#define CONSTRUCTION_DISTANCE_FROM_WALL 150 // How far we should put the front of the robot to the wall when depositing hte puck
#define CONSTRUCTION_DISTANCE_CLEAR_FROM_STACK 210 // How far we should reverse from constructio stack before attempting to turn.

#define PUCK_GRID_FROM_NORTH 275 // Where the southmost part of the pucks can extend to from the north wall
#define PUCK_GRID_WIDTH 270 // Width from east to west of the 5x5 grid.
#define PUCK_RACK_0_WEST_DISTANCE 720
#define PUCK_RACK_1_WEST_DISTANCE 660
#define PUCK_RACK_2_WEST_DISTANCE 600
#define PUCK_RACK_3_WEST_DISTANCE 540
#define PUCK_RACK_4_WEST_DISTANCE 480
#define SCAN_INITIALISE_HORIZONTAL 85 // The distance from the west that we should stop the robot during the black wall colour initialisation
#define PUCK_GRID_DISTANCE_BETWEEN_PUCK_CENTERS 50

#define FIRST_DISCARD_ZONE 1000
#define SECOND_DISCARD_ZONE 900
#define FIRST_TEMP_DROPOFF HOME_BASE_EAST_EDGE
#define SECOND_TEMP_DROPOFF HOME_BASE_WEST_EDGE


//* Robot Body Constants *//

// Indices of ultrasonic sensors

#define SIDE_LEFT 2          //0     
#define FRONT_LEFT 0         //1
#define FRONT_RIGHT 1        //2
#define SIDE_RIGHT 3         //3
#define BACK 4               //4
#define FRONT_MIDDLE 5       //5
#define FRONT_SENSORS 7      

#define TOTAL_SONIC_SENSORS 6

// Distance offsets on bot body (eg. side sensor to center)

#define SIDE_SENSORS_WIDTH 250 // Delete this when block & puck locating code implemented
#define WIDTH_SENSOR_TO_SENSOR 220 // The distance measured from the very outer edge of each side ultrasonic sensor.
#define WIDTH_SENSOR_TO_CENTER 110 // WIDTH_SENSOR_TO_SENSOR / 2
#define CLEARANCE_RADIUS_CENTER_TO_BACK 240 // Smallest circle centered about turning point enclosing the back half of robot. Make larger for larger tolerance
#define CLEARANCE_RADIUS_CENTER_TO_FRONT 130 // Smallest circle centered about turning point enclosing the front half of robot
#define SIDE_SENSOR_OFFSET_FROM_BACK 150 // Distance of the midpoint of the side ultrasonic sensors to the far rear sensor edge
#define FRONT_CLAW_DISTANCE_FROM_CENTER 110 // Distance from the absolute front extending part of robot to center turning point.
#define DISTANCE_FRONT_SENSOR_FROM_CENTER 50 // Distance from front ultrasonic sensor to the turning center
#define DISTANCE_BACK_SENSOR_FROM_CENTER 230 // Distance from back ultrasonic sensor to the turning center
#define DISTANCE_FRONT_SENSOR_TO_SIDE_SENSOR 150
#define DISTANCE_BACK_SENSOR_TO_SIDE_SENSOR 100 

#define WIDTH_WHEEL_TO_WHEEL 214


// Tolerances and thresholds
#define SAFETY_MARGIN 85 // The margin we will enforce be kept around us clear at all times.
#define BLOCK_TOLERANCE 50 // Used for sweeping block location. This affects sensitivity in detecting block.
#define PUCK_TOLERANCE 150 // Used for initially sweeping puck location. This affects sensitivity in detecting pucks.
#define DISTANCE_STOPPED_FROM_PUCK 50 // When we drive up to the puck using moveForwardIndefinitely(), we use this value to moveForward even further until puck is scooped up.
#define SENSOR_DELAY_MIN 60
#define WALL_CLEARANCE_FRONT 175 // Distance to stop from east wall 

// Miscellaneous:
#define ADJUST 4        // 4 was test to be best

#define BACKWARD 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3
#define LESS_THAN 4
#define GREATER_THAN 5
#define EQUAL_TO 6

#define TRUE 1
#define FALSE 0

// Colour Sensing:
#define CLAW_SENSING 1
#define WALL_SENSING 0

#define WALL_ALGORITHM 0
#define CLAW_GROUND_ALGORITHM 5
#define CLAW_1_ALGORITHM 1
#define CLAW_BLACK_PUCK_ALGORITHM 4

// Servo:
#define GROUND 0
#define ABOVE_1_PUCK 1
#define ABOVE_2_PUCK 2
#define ABOVE_3_PUCK 3

/* Variables to be shared / imported */

extern float ultrasonic_distances[TOTAL_SONIC_SENSORS];
int driveStraightEnable;
int currentPuckRackScanningIndex;
int puckRackColours[5];
int puckRackOffsetsFromWest[5];
int sweeping;
float block_edge_location[4];
float puckPileLocation;

short int drivingForwardFlag;
short int drivingBackwardFlag;
short int turningLeftFlag;
short int turningRightFlag;

extern int initialisation;

extern void lowerAndOpen(int puck_stack_position);
extern void closeAndRaise(int puck_stack_position);

char output[32];

extern float ultrasonic_distances[TOTAL_SONIC_SENSORS];
extern char output[32];

extern int ultrasonic_distances_mm[TOTAL_SONIC_SENSORS];

