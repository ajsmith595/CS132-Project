#include "solve.h"

struct Pole pole1, pole2, pole3;


void solveHanoi(int n, struct Pole from, struct Pole aux, struct Pole to)
{

    if (n > 0) 
    {
	    solveHanoi(n - 1, from, to, aux); // move n - 1 disks to the middle.
	    moveBlock(from, to);              // move largest disk to last pole.
	    solveHanoi(n - 1, aux, from, to); // move n - 1 disks to last pole.
	}
}

int main(int argc, char *argv[])
{

    initRobot();

    pole1 = (struct Pole){POLE_POS1, 0};
    pole2 = (struct Pole){POLE_POS2, 1};
    pole3 = (struct Pole){POLE_POS3, 2};

    solveHanoi(NUM_OF_CUBES, pole1, pole2, pole3);

    return 0;
}