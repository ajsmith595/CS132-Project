
#ifndef SERVO_C
#define SERVO_C

#include "constants.c"
#include "point.c"
#include <stdio.h>
#include <math.h>


struct Servo {
        struct Point *a;
        struct Point *b;
};

void moveServoByPoint(struct Servo *servo, struct Point *p){
        struct Point *newA = addPoints(servo->a, p);
	free(servo->a);
	servo->a = newA;
	struct Point *newB = addPoints(servo->b, p);
	free(servo->b);
	servo->b = newB;
}

void newServoLocation(struct Servo *servo, struct Point *target){
	struct Point *aToTarget = subtractPoints(target, servo->a);
	makeUnitVector(aToTarget);
	multiplyPoint(aToTarget, ARM_SERVO_LENGTH);

        struct Point *newB = addPoints(servo->a, aToTarget);
	free(aToTarget);
	struct Point *newBToTarget = subtractPoints(target, newB);
        struct Point *newA = addPoints(servo->a, newBToTarget);
	free(newBToTarget);
	newB = copyPoint(target);

	free(servo->a);
	free(servo->b);
	servo->a = newA;
	servo->b = newB;       	
}
void printServo(struct Servo *s){
	printf("---SERVO---\nServo[a]:\n\t");
	printPoint(s->a);
	printf("\nServo[b]:\n\t");
	printPoint(s->b);
	printf("\n-----------");
}

#endif
