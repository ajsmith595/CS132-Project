#include "servo.h"


// Move the whole servo by a vector
void moveServoByPoint(struct Servo *servo, struct Point *p)
{
    // Get new positions, free the old positions from memory, then set the new ones.
    struct Point *newA = addPoints(servo->a, p);
    free(servo->a);
    servo->a = newA;
    struct Point *newB = addPoints(servo->b, p);
    free(servo->b);
    servo->b = newB;
}

// Calculate a new servo location given a target. Based on algorithm designed prior.
void newServoLocation(struct Servo *servo, struct Point *target)
{
    struct Point *aToTarget = subtractPoints(target, servo->a);
    makeUnitVector(aToTarget);
    multiplyPoint(aToTarget, ARM_SERVO_LENGTH);

    struct Point *newB = addPoints(servo->a, aToTarget); // Point ab in the direction of the target
    free(aToTarget);
    struct Point *newBToTarget = subtractPoints(target, newB); // Find how far b is from the target
    struct Point *newA = addPoints(servo->a, newBToTarget);    // Move a by how far b needs to go to get to the target i.e. move the servo to the target
    free(newBToTarget);
    newB = copyPoint(target); // Need a new point otherwise if target point supplied is changed, this will also change

    free(servo->a);
    free(servo->b);
    servo->a = newA; // Set the new values
    servo->b = newB;
}

// Output servo info to the terminal
void printServo(struct Servo *s)
{
    printf("---SERVO---\nServo[a]:\n\t");
    printPoint(s->a);
    printf("\nServo[b]:\n\t");
    printPoint(s->b);
    printf("\n-----------");
}