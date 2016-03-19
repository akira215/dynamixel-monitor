#include "dxlax.h"

dxlAX::dxlAX(const int servoId):
    dxl(servoId)
{
   newProperty(0x1A, 1, "CW Compliance Margin",  0, true, false);
   newProperty(0x1B, 1, "CCW Compliance Margin", 0, true, false);
   newProperty(0x1C, 1, "CW Compliance Slope",   0, true, false);
   newProperty(0x1D, 1, "CCW Compliance Slope",  0, true, false);

}
