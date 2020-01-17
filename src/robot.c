#include "robot.h"

int connection;
struct Servo *servos[3]; // There are 3 segments of the robot arm, so 3 servos
struct Point *baseOfArm; // The location of the base motor of the arm
int *poleHeights;        // An array of the number of cubes on each pole


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



// Writes a particular value to a particular motor
void move_to_location(unsigned char id, unsigned char loc_h, unsigned char loc_l)
{
    unsigned char cs = ~(id + 0x07 + 0x03 + 0x1e + loc_l + loc_h +
                         0x30 + 0x00);

    unsigned char arr[] = {0xff, 0xff, id, 0x07, 0x03, 0x1e, loc_l,
                           loc_h, 0x30, 0x00, cs};

    int buff_len = 100;
    unsigned char buff[buff_len];

    write_to_connection(connection, arr, 11, buff, buff_len);
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

void moveBlock(struct Pole from, struct Pole to)
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
    wait(1);
    setVertical(++poleHeights[to.index]);
    wait(2);
    drop();
    wait(1);
}


void init()
{

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
}

void initRobot()
{
    connection = open_connection("/dev/ttyUSB0", B1000000);

    init();
    reset();
    drop();
    toSafeHeight();
    wait(2);
}
