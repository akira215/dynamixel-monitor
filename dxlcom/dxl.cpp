#include "dxl.h"
#include "dxlax.h"
#include "dxlmx.h"
#include "dxlcom.h"

#include <QDebug>

dxl::dxl(const int servoId) :
    mServoId(servoId)
{
   newProperty(ADD_MODEL_NUM, 2, "Model Number",     0, false,   false);
   newProperty(0x02, 1, "Firmware",                  0, false,   false);
   newProperty(ADD_ID, 1, "ID",               mServoId, true,    true);
   newProperty(0x04, 1, "Baudrate",                  0, true,    false);
   newProperty(0x05, 1, "Return Delay Time",         0, true,    false);
   newProperty(0x06, 2, "CW Angle Limit",            0, true,    false);
   newProperty(0x08, 2, "CCW Angle Limit",           0, true,    false);
   newProperty(0x0B, 1, "Highest Limit Temperature", 0, true,    false);
   newProperty(0x0C, 1, "Lowest Limit Voltage",      0, true,    false);
   newProperty(0x0D, 1, "Highest Limit Voltage",     0, true,    false);
   newProperty(0x0E, 2, "Max Torque",                0, true,    false);
   newProperty(0x10, 1, "Status Return Level",       0, true,    false);
   newProperty(0x11, 1, "Alarm LED",                 0, true,    false);
   newProperty(0x12, 1, "Alarm Shutdown",            0, true,    false);

   newProperty(0x18, 1, "Torque Enable",             0, true,    false);
   newProperty(0x19, 1, "LED",                       0, true,    false);
   newProperty(0x1E, 2, "Goal Position",             0, true,    false);
   newProperty(0x20, 2, "Moving Speed",              0, true,    false);
   newProperty(0x22, 2, "Torque Limit",              0, true,    false);
   newProperty(0x24, 2, "Present Position",          0, false,   false);
   newProperty(0x26, 2, "Present Speed",             0, false,   false);
   newProperty(0x28, 2, "Present Load",              0, false,   false);
   newProperty(0x2A, 1, "Present Voltage",           0, false,   false);
   newProperty(0x2B, 1, "Present Temperature",       0, false,   false);
   newProperty(0x2C, 1, "Registered",                0, false,   false);
   newProperty(0x2E, 1, "Moving",                    0, false,   false);
   newProperty(0x2F, 1, "Lock",                      0, true,    false);
   newProperty(0x30, 2, "Punch",                     0, true,    false);
}


dxl::~dxl()
{
}

void dxl::newProperty(int address, int size, QString description, int value, bool writable, bool valid)
{
    dxlProperty property = {address, size, description, value, writable, valid};
    mPropertyList.insert(address, property);
}

dxl::dxlProperty dxl::getPropertyValue (const int address)
{
    dxlProperty property = {address, 0, QString("Error"), 0, false, false};

    if (!mPropertyList.contains(address))
        return property;

    property = mPropertyList.value(address);

    if (property.valid)
        return property;

    dxlCom* dxlHandle = dxlCom::getInstance();

    int result = dxlHandle->readDxlData( mServoId, property.address, property.size );

    if (result != -1)
    {
        property.valid = true;
        property.value = result;
    }
    else
    {
         qDebug()<<"Dxl error : "<<(int) dxlHandle->dxlerror();
         property.valid = false;
    }


    mPropertyList.insert(property.address, property);
    return property;
}

bool dxl::setPropertyValue (const int address, const int value)
{
    if (!mPropertyList.contains(address))
        return false;

    dxlProperty property = mPropertyList.value(address);

    qDebug()<<"Servo Id : "<<mServoId<<" Writing : "<<value<<" at address : "<<address;

    dxlCom* dxlHandle = dxlCom::getInstance();

    dxlCom::dxlError result = dxlHandle->writeDxlData(mServoId, property.address, value, property.size );
    if(  result == dxlCom::NoError )
    {
        property.valid = true;
        property.value = value;

        if(address == ADD_ID)
            mServoId = value;

    }
    else
    {
        qDebug()<<"Dxl error : "<<(int)result;
        property.valid = false;
    }
    mPropertyList.insert(property.address, property);

    return property.valid;

}

dxl* dxl::getInstance(const int servoId)
{
    dxlCom* dxlHandle = dxlCom::getInstance();

    int dxlModel  = dxlHandle->readDxlData( servoId, ADD_MODEL_NUM, 2 );

    if( dxlModel != -1 )
    {
        dxl* servo = 0;
        if ((dxlModel == 18)||(dxlModel == 12)||(dxlModel == 300))
            servo = new dxlAX(servoId);
        else if ((dxlModel == 310)||(dxlModel == 320))
            servo = new dxlMX(servoId);
        else
            servo = new dxl(servoId);
        return servo;
    }
    else
        return 0;

}

