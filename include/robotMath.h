#ifndef ROBOT_MATH_H
#define ROBOT_MATH_H

#include "constants.h"

float abso(float f); // Returns the absolute value


// Converts a value used by motors to a degree value
int robotArmServoValueToDegrees(float x);

// Opposite of robotArmServoValueToDegrees
int degreesToRobotArmServoValue(float x);

// Converts radians to degrees
float radsToDegrees(float x);

// Converts radians to the value used by motors
float radsToRobotArmServoValue(float x);

#endif