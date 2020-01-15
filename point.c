
// Prevent file from being included twice
#ifndef POINT_C
#define POINT_C

#include "constants.c"
#include <stdio.h>
#include <math.h>

struct Point
{
    float x;
    float y;
};

// Gets the magnitude of the vector of a given point
float getPointMagnitude(struct Point *a)
{
    return sqrt(a->x * a->x + a->y * a->y);
}

// Adds two point vectors together and returns the result
struct Point *addPoints(struct Point *a, struct Point *b)
{
    struct Point *result = malloc(sizeof(struct Point));
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    return result;
}

// Same as addPoints, but subtracts one from the other
struct Point *subtractPoints(struct Point *a, struct Point *b)
{
    struct Point *result = malloc(sizeof(struct Point));
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    return result;
}

// Makes the given point vector into a vector with a magnitude of 1
void makeUnitVector(struct Point *a)
{
    float magnitude = getPointMagnitude(a);
    a->x = a->x / magnitude;
    a->y = a->y / magnitude;
}
// Multiplies both components by a constant factor
void multiplyPoint(struct Point *a, float k)
{
    a->x = a->x * k;
    a->y = a->y * k;
}

// Clones a point to a new memory location
struct Point *copyPoint(struct Point *p)
{
    struct Point *newP = malloc(sizeof(struct Point));
    newP->x = p->x;
    newP->y = p->y;
    return newP;
}

// Gets the angle a point vector makes
float getPointAngle(struct Point *p)
{
    return atan2(p->y, p->x);
}

// Outputs a point to the terminal
void printPoint(struct Point *p)
{
    printf("Vector: x=%.6f, y=%.6f;", p->x, p->y);
}

#endif
