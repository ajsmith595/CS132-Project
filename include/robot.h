#ifndef ROBOT_H
#define ROBOT_H

#include "dynamixel.h"
#include "servo.h"
#include "point.h"
#include "pole.h"
#include "constants.h"
#include "robotMath.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>


/**
 *
 */
void reset_arm();


void reset();


// Writes a particular value to a particular motor
void move_to_location(unsigned char id, unsigned char loc_h, unsigned char loc_l);


// Like move_to_location, but the high and low bits are automatically handled
void mtl(unsigned char id, unsigned int x);


// Waits some time
void wait(float num);


// Sets the gripping mechanism to a preset which grips a block securely
void grab();


// Sets the gripping mechanism to a preset where blocks can be freed from the grip
void drop();


// Sets the base motor to a particular pole location
void setPole(struct Pole thePole);


// Sets a particular motor's value such that it will be at a particular angle
void setServoAngle(unsigned char id, float rads);


// Given a target height, the motor's are adjusted using the IK algorithm
void setServoHeightsArr(int blockNumber);


void setVertical(int block);


void toSafeHeight();


void moveBlock(struct Pole from, struct Pole to);


void init();


void initRobot();


#endif