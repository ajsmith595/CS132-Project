#ifndef SERVO_H
#define SERVO_H

#include "constants.h"
#include "point.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/**
 *
 */
struct Servo
{
    struct Point *a;
    struct Point *b;
};

// Move the whole servo by a vector
/**
 *
 */
void moveServoByPoint(struct Servo *servo, struct Point *p);

// Calculate a new servo location given a target. Based on algorithm designed prior.
/**
 *
 */
void newServoLocation(struct Servo *servo, struct Point *target);

// Output servo info to the terminal
/**
 *
 */
void printServo(struct Servo *s);

#endif
