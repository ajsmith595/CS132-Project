#include "dynamixel.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

const int pole1 = 0x01aa;
const int pole2 = 0x01ff;
const int pole3 = 0x0254;

int connection;

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

void verticalTo(float val){

}

void setPole(int pole){
	move_to_location(1,pole >> 8,pole);
}


void init(){
	move_to_location(5,0x01,0xff);
	setPole(pole1);
	move_to_location(2,0x00,0xbb);
	move_to_location(3,0x01,0xff);
	move_to_location(4,0x01,0xff);
	waitx(3);
}

int main(int argc, char* argv[]) {

	connection = open_connection("/dev/ttyUSB0",B1000000);

	init(connection);

	grab();
	waitx(2);
	drop();

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
