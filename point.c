
#ifndef POINT_C
#define POINT_C

#include "constants.c"
#include <stdio.h>
#include <math.h>

struct Point{
	float x;
	float y;
};


float getPointMagnitude(struct Point *a){
	return sqrt(a->x * a->x + a->y * a->y);
}

struct Point *addPoints(struct Point *a, struct Point *b){
	struct Point *result = malloc(sizeof(struct Point));
	result->x = a->x + b->x;
	result->y = a->y + b->y;
	return result;
}

struct Point *subtractPoints(struct Point *a, struct Point *b){
	struct Point *result = malloc(sizeof(struct Point));
	result->x = a->x - b->x;
	result->y = a->y - b->y;
	return result;
}

void makeUnitVector(struct Point *a){
	float magnitude = getPointMagnitude(a);
	a->x = a->x / magnitude;
	a->y = a->y / magnitude;
}
void multiplyPoint(struct Point *a, float k){
	a->x = a->x * k;
	a->y = a->y * k;
}

struct Point *copyPoint(struct Point *p){
	struct Point *newP = malloc(sizeof(struct Point));
	newP->x = p->x;
	newP->y = p->y;
	return newP;
}

float getPointAngle(struct Point *p){
	return atan2(p->y, p->x);
}

void printPoint(struct Point *p){
	printf("Vector: x=%.6f, y=%.6f;", p->x, p->y);
}


#endif
