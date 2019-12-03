#include "dynamixel.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

const int pole1 = 0x01aa;
const int pole2 = 0x01ff;
const int pole3 = 0x0254;

const int numCubes = 3;
const float lengthToPile = 8;
const float lengthBetweenServos = 4.5;
const float lengthOfCube = 1;
const float k1 = lengthToPile / lengthBetweenServos;

int connection;

struct angles {
	float t1;
	float t2;
}

/*
 Helper functions
*/

int numToDegrees(float x){
	return (int)((x-511)*150/511);
}
int degreesToNum(float x){
	return (int)(x*511/150+511);
}

float radsToDegrees(float x){
	return (x * 180 / PI);
}

float radsToNum(float x){
	return degreesToNum(radsToDegrees(x));
}




struct angles findTheta(int cubeNum) {
	const float k2 = lengthOfCube * cubeNum / lengthBetweenServos;
	float t1 = (k2+sqrt(8-k2*k2-4*k1));
	float t2 = k2 - 2 * t1;
	return struct angles {t1, t2};

}

/*
 Actuator functions
*/

void move_to_location(unsigned char id,
			unsigned char loc_h, unsigned char loc_l) {

	unsigned char cs = ~ ( id + 0x07 + 0x03 + 0x1e + loc_l + loc_h +
				0x30 + 0x00);

	unsigned char arr[] = { 0xff, 0xff, id, 0x07, 0x03, 0x1e, loc_l,
                                       loc_h, 0x30, 0x00, cs };

	int buff_len = 100;
	unsigned char buff[buff_len];

	int bytes_read = write_to_connection(connection,arr,11,buff,buff_len);

}

void wait() {
	usleep(2000000);
}
void waitx(int x) {
	usleep(2000000 * x);
}

void grab(){
	move_to_location(5, 0x01, 0x44);
}
void drop(){
	move_to_location(5, 0x01, 0xff);
}

void move_to_location_(int servo, int num){
	move_to_location(servo, num >> 8, num);
}

void verticalTo(float val){

}

void setPole(int pole){
	move_to_location(1,pole >> 8,pole);
}

/*
 Initalization
*/

void init(){
	move_to_location_(2, 255);
	move_to_location_(3, 0x01ff);
	move_to_location_(4, 0x0100);
	move_to_location_(5, 0x00);
	setPole(pole1);
	waitx(3);
}


int main(int argc, char* argv[]) {

	connection = open_connection("/dev/ttyUSB0",B1000000);

	init();

	int cubeNum = 1;
	struct angles x = findTheta(cubeNum);
	move_to_location(2, radsToNum(x.t1));
	move_to_location(3, radsToNum(x.t2));

	// wait_until_done(connection,4);
	// wait_until_done(connection,4);
	// wait_until_done(connection,4);
	//
	// move_to_location(connection,2,0x01,0x00);
	// move_to_location(connection,3,0x01,0xaa);
	// move_to_location(connection,4,0x01,0x33);
	// wait_until_done(connection, 4);
	//
	// move_to_location(connection, 5, 0x01, 0x44);
	// wait_until_done(connection, 5);
	//
	// move_to_location(connection, 2, 0x01, 0xaa);
	// move_to_location(connection, 2, 0x01, 128);
	return 0;

}
