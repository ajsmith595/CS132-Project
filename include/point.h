#ifndef POINT_H
#define POINT_H

#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 *
 */
struct Point
{
    float x;
    float y;
};


/**
 *
 */
float getPointMagnitude(struct Point *a);


/**
 *
 */
struct Point *addPoints(struct Point *a, struct Point *b);


/**
 *
 */
struct Point *subtractPoints(struct Point *a, struct Point *b);


/**
 *
 */
void makeUnitVector(struct Point *a);


/**
 *
 */
void multiplyPoint(struct Point *a, float k);


/**
 *
 */
struct Point *copyPoint(struct Point *p);


/**
 *
 */
float getPointAngle(struct Point *p);


/**
 *
 */
void printPoint(struct Point *p);

#endif