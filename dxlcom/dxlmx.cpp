#include "dxlmx.h"

dxlMX::dxlMX(const int servoId):
    dxl(servoId)
{
    newProperty(0X14, 2, "Multi Turn Offset",            0, true, false);
    newProperty(0X16, 1, "Resolution Divider",           0, true, false);

    newProperty(0X1A, 1, "Derivative Gain",              0, true, false);
    newProperty(0X1B, 1, "Integral Gain",                0, true, false);
    newProperty(0X1C, 1, "Proportional Gain",            0, true, false);

    newProperty(0X44, 2, "Current",                      0, true, false);
    newProperty(0X46, 1, "Torque Control Mode Enable",   0, true, false);
    newProperty(0X47, 2, "Goal Torque",                  0, true, false);
    newProperty(0X49, 1, "Goal Acceleration",            0, true, false);

}
