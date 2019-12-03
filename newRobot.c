#include "dynamixel.h"
#include "servo.c"
#include "point.c"
#include "constants.c"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

struct Pole {
	int pos;
	int index;
};

const int pos1 = 0x01aa;
const int pos2 = 0x01ff;
const int pos3 = 0x0254;

struct Pole pole1, pole2, pole3;

int connection;
struct Servo* servos[3];
struct Point *baseOfArm;
int* poleHeights;


void reset_arm(){
        unsigned char arr[] = { 0xff, 0xff, 0x00, 0x02, 0x06, 0xf7};

        int buff_len = 100;
        unsigned char buff[buff_len];

        int bytes_read = write_to_connection(connection,arr,6,buff,buff_len);
}

void reset(){
	float y = ARM_HEIGHT;
	int i;
	for(i = 0; i < 3; i++){
		servos[i]->a->x = 0;
		servos[i]->a->y = y;
		y += ARM_SERVO_LENGTH;
		servos[i]->b->x = 0;
		servos[i]->b->y = y;
	}
}

int robotArmServoValueToDegrees(float x){
	return (int)((x-511)*150/511);
}

int degreesToRobotArmServoValue(float x){
	return (int)(x*511/150 + 511);
}

float radsToDegrees(float x){
	return (x * 180 / PI);
}

float radsToRobotArmServoValue(float x){
	return degreesToRobotArmServoValue(radsToDegrees(x));
}

void move_to_location(unsigned char id, unsigned char loc_h, unsigned char loc_l){
	unsigned char cs = ~ ( id + 0x07 + 0x03 + 0x1e + loc_l + loc_h +
                                0x30 + 0x00);

        unsigned char arr[] = { 0xff, 0xff, id, 0x07, 0x03, 0x1e, loc_l,
                                       loc_h, 0x30, 0x00, cs };

        int buff_len = 100;
        unsigned char buff[buff_len];

        int bytes_read = write_to_connection(connection,arr,11,buff,buff_len);
}

void mtl(unsigned char id, unsigned int x){
	move_to_location(id, x >> 8, x);
}


void wait(float num){
	usleep(2000000 * num);
}

void grab(){
	mtl(5, 0x0144);
}

void drop(){
	mtl(5, 0x01ff);
}

void setPole(struct Pole thePole){
	mtl(1, thePole.pos);
}

void setServoAngle(unsigned char id, float rads){
	mtl(id, radsToRobotArmServoValue(rads));
}


void setServoHeightsArr(int blockNumber){
	int i;
	struct Point *target = malloc(sizeof(struct Point));
	target->x = ARM_BASE_TO_CUBES_DIST;
	target->y = BLOCK_HEIGHT * (blockNumber - 1) + BLOCK_Y_OFFSET;
	for(i = 2; i >= 0; i--){
		struct Servo *s = servos[i];
		newServoLocation(s, target);
		target->x = s->a->x;
		target->y = s->a->y;
	}
	free(target);
	struct Point *offset = subtractPoints(baseOfArm, servos[0]->a);
	for(i = 0; i < 3; i++){
		moveServoByPoint(servos[i], offset);
	}
	free(offset);
}
float abso(float f){
	if(f > 0){
		return f;
	}
	return -f;
}

float norm(float f){
	while(f >= 2 * PI){
		f -= 2 * PI;
	}
	return f;
}
void setVertical(int block){
	reset();
	printf("Setting vertical to: %d\n", block);
	int i;
	for(i = 0; i < 3; i++){ // Doing it 3 times ensures it's in the right place
		setServoHeightsArr(block);
	}
	int prevAngle = 0;
	for(i = 0; i < 3; i++){
		struct Point *diff = subtractPoints(servos[i]->b, servos[i]->a);
		//float angle = -PI / 6;
		float angle = -abso(getPointAngle(diff));
		// if(block == 3){
		// 	angle = getPointAngle(diff);
		// }
		printf("Angle(%d): %.6f\n", i + 2, radsToDegrees(getPointAngle(diff) - prevAngle));

		setServoAngle(6-(i + 2), angle - prevAngle);
		prevAngle = angle;

	}
}

void toSafeHeight(){
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

    if (n == 1)
    {
        moveBlock(from, to);
        return;
    }
    solveHanoi(n - 1, from, to, aux); // move n - 1 disks to the middle.
    moveBlock(from, to);              // move largest disk to last pole.
    solveHanoi(n - 1, aux, from, to); // move n - 1 disks to last pole.
}


void init(){
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
	for(i = 0; i < 3; i++){
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


	for(i = 0; i < 3; i++){
		printServo(servos[i]);
	}
	/*mtl(2, 255);
	mtl(3, 0x01ff);
	mtl(4, 0x0100);
	mtl(5, 0x00);
	setPole(pole1);
	wait(3);*/
}



int main(int argc, char* argv[]){
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
