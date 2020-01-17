#include "robotMath.h"


float abso(float f) // Returns the absolute value
{
    if (f > 0)
    {
        return f;
    }
    return -f;
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