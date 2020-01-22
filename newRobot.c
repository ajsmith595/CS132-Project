#include "dynamixel.h"
#include "servo.c"
#include "point.c"
#include "constants.c"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

struct Pole
{
    int pos;
    int index;
};

// Set pole motor positions
const int pos1 = 0x01aa;
const int pos2 = 0x01ff;
const int pos3 = 0x0254;

struct Pole pole1, pole2, pole3;

int connection;
struct Servo *servos[3]; // There are 3 segments of the robot arm, so 3 servos
struct Point *baseOfArm; // The location of the base motor of the arm
int *poleHeights;        // An array of the number of cubes on each pole

void reset_arm() // Resets the motors
{
    unsigned char arr[] = {0xff, 0xff, 0x00, 0x02, 0x06, 0xf7};

    int buff_len = 100;
    unsigned char buff[buff_len];

    int bytes_read = write_to_connection(connection, arr, 6, buff, buff_len);
}

void reset() // Resets the virtualised version of the robot to be a vertical line of segments up
{
    float y = ARM_HEIGHT;
    int i;
    for (i = 0; i < 3; i++)
    {
        servos[i]->a->x = 0;
        servos[i]->a->y = y;
        y += ARM_SERVO_LENGTH;
        servos[i]->b->x = 0;
        servos[i]->b->y = y;
    }
}

// Converts a value used by motors to a degree value
int robotArmServoValueToDegrees(float x)
{
    return (int)((x - 511) * 150 / 511);
}
// Opposite of robotArmServoValueToDegrees
int degreesToRobotArmServoValue(float x)
{
    return (int)(x * 511 / 150 + 511);
}
// Converts radians to degrees
float radsToDegrees(float x)
{
    return (x * 180 / PI);
}
// Converts radians to the value used by motors
float radsToRobotArmServoValue(float x)
{
    return degreesToRobotArmServoValue(radsToDegrees(x));
}

// Writes a particular value to a particular motor
void move_to_location(unsigned char id, unsigned char loc_h, unsigned char loc_l)
{
    unsigned char cs = ~(id + 0x07 + 0x03 + 0x1e + loc_l + loc_h +
                         0x30 + 0x00);

    unsigned char arr[] = {0xff, 0xff, id, 0x07, 0x03, 0x1e, loc_l,
                           loc_h, 0x30, 0x00, cs};

    int buff_len = 100;
    unsigned char buff[buff_len];

    int bytes_read = write_to_connection(connection, arr, 11, buff, buff_len);
}

// Like move_to_location, but the high and low bits are automatically handled
void mtl(unsigned char id, unsigned int x)
{
    move_to_location(id, x >> 8, x);
}

// Waits some time
void wait(float num)
{
    usleep(2000000 * num);
}

// Sets the gripping mechanism to a preset which grips a block securely
void grab()
{
    mtl(5, 0x0144);
}

// Sets the gripping mechanism to a preset where blocks can be freed from the grip
void drop()
{
    mtl(5, 0x01ff);
}

// Sets the base motor to a particular pole location
void setPole(struct Pole thePole)
{
    mtl(1, thePole.pos);
}

// Sets a particular motor's value such that it will be at a particular angle
void setServoAngle(unsigned char id, float rads)
{
    mtl(id, radsToRobotArmServoValue(rads));
}

// Given a target height, the motor's are adjusted using the IK algorithm
void setServoHeightsArr(int blockNumber)
{
    int i;
    struct Point *target = malloc(sizeof(struct Point));           // For the first servo, the target is the actual target wanted
    target->x = ARM_BASE_TO_CUBES_DIST;                            // The x distance is constant: the three poles are circular around the robot
    target->y = BLOCK_HEIGHT * (blockNumber - 1) + BLOCK_Y_OFFSET; // Based on the constants set and height given. Block number 1 means the first block, not second like usual
    for (i = 2; i >= 0; i--)                                       // Go through each servo, starting at the servo most distant from the base motor.
    {
        struct Servo *s = servos[i];
        newServoLocation(s, target); // Get the new servo location, based on the target set
        target->x = s->a->x;
        target->y = s->a->y; // For all servos except the first, the target is the a point of the previous servo
    }
    free(target);                                                   // No longer needed
    struct Point *offset = subtractPoints(baseOfArm, servos[0]->a); // Get how far the base a point has moved from the origin point.
    for (i = 0; i < 3; i++)
    {
        moveServoByPoint(servos[i], offset); // Move all servos back by this amount
    }
    free(offset);
}
float abso(float f) // Returns the absolute value
{
    if (f > 0)
    {
        return f;
    }
    return -f;
}

void setVertical(int block)
{
    reset();
    printf("Setting vertical to: %d\n", block);
    int i;
    for (i = 0; i < 3; i++)
    { // Doing it 3 times ensures it's in the right place
        setServoHeightsArr(block);
    }
    int prevAngle = 0;
    for (i = 0; i < 3; i++)
    {
        struct Point *diff = subtractPoints(servos[i]->b, servos[i]->a);
        //float angle = -PI / 6;
        float angle = -abso(getPointAngle(diff));
        // if(block == 3){
        // 	angle = getPointAngle(diff);
        // }
        printf("Angle(%d): %.6f\n", i + 2, radsToDegrees(getPointAngle(diff) - prevAngle));

        setServoAngle(6 - (i + 2), angle - prevAngle);
        prevAngle = angle;
    }
}

void toSafeHeight()
{
    setVertical(NUM_OF_CUBES + 2);
}

int moveBlock(struct Pole from, struct Pole to)
{
    toSafeHeight();
    wait(1);
    setPole(from);
    wait(1);
    setVertical(poleHeights[from.index]--);
    wait(2);
    grab();
    wait(1);
    toSafeHeight();
    wait(1);
    setPole(to);
    setVertical(++poleHeights[to.index]);
    wait(2);
    drop();
    wait(1);
}

void solveHanoi(int n, struct Pole from, struct Pole aux, struct Pole to)
{

    if (n == 0)
    {
        moveBlock(from, to);
        return;
    }
    solveHanoi(n - 1, from, to, aux); // move n - 1 disks to the middle.
    moveBlock(from, to);              // move largest disk to last pole.
    solveHanoi(n - 1, aux, from, to); // move n - 1 disks to last pole.
}

void init()
{
    pole1 = (struct Pole){pos1, 0};
    pole2 = (struct Pole){pos2, 1};
    pole3 = (struct Pole){pos3, 2};

    baseOfArm = malloc(sizeof(struct Point));
    baseOfArm->x = 0;
    baseOfArm->y = ARM_HEIGHT;
    poleHeights = malloc(sizeof(int) * 3);
    poleHeights[0] = NUM_OF_CUBES;
    poleHeights[1] = 0;
    poleHeights[2] = 0;

    float y = ARM_HEIGHT;
    int i;
    for (i = 0; i < 3; i++)
    {
        struct Point *a = malloc(sizeof(struct Point));
        a->x = 0;
        a->y = y;
        y += ARM_SERVO_LENGTH;
        struct Point *b = malloc(sizeof(struct Point));
        b->x = 0;
        b->y = y;
        struct Servo *s = malloc(sizeof(struct Servo));
        s->a = a;
        s->b = b;
        servos[i] = s;
        printServo(servos[i]);
        printf("\n");
    }

    for (i = 0; i < 3; i++)
    {
        printServo(servos[i]);
    }
    /*mtl(2, 255);
	mtl(3, 0x01ff);
	mtl(4, 0x0100);
	mtl(5, 0x00);
	setPole(pole1);
	wait(3);*/
}

int main(int argc, char *argv[])
{
    connection = open_connection("/dev/ttyUSB0", B1000000);

    reset_arm();
    init();
    reset();
    drop();
    toSafeHeight();
    wait(2);
    solveHanoi(NUM_OF_CUBES, pole1, pole2, pole3);

    int cubeNum = 1;
    //	setVertical(cubeNum);

    return 0;
}
