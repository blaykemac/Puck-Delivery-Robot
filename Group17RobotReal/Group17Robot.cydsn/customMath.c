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
// * PSOC LIBRARY * //
#include "project.h"

// * C LIBRARIES * // 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// * OUR LIBRARIES * //
#include "main.h"
#include "colour.h"
#include "ultrasonic.h" 
#include "servo.h"
#include "mishamotor.h"
#include "customMath.h"
#include "navigation.h"


float averageSensor(float num1, float num2){
    return (num1 + num2) / 2;
}
// Round the current direction into cardinal directions
int roundDirection(){
    if (currentOrientation < 45 && currentOrientation > 315){
    return NORTH;}
    else if (currentOrientation >= 45 && currentOrientation < 135){
    return EAST;}
    else if (currentOrientation >= 135 && currentOrientation < 225){
    return SOUTH;}
    else {return WEST;}
}







