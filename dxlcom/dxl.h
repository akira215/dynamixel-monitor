#ifndef DXL_H
#define DXL_H

#include <QString>
#include <QHash>

#include <dynamixel.h>
#include "dxlConfig.h"

#define ADD_MODEL_NUM   0 // address of model number in servo register
#define ADD_ID          3 // address of ID in servo register

class dxl
{


public:
    static dxl* getInstance(const int servoId);

    struct dxlProperty {
        int address;        // address in Dunamixel register
        int size;           // size in byte of the property
        QString description;// human readable description
        int value;
        bool writable;
        bool valid;
    };

    dxl(const int servoId);
    ~dxl();

    int getId() const { return mServoId; }

    QList<dxlProperty> getPropertyList() const { return mPropertyList.values(); }

    dxlProperty getPropertyValue (const int address);

    bool setPropertyValue (const int address, const int value);

private:
    int mServoId;

protected:

   void newProperty(int address, int size, QString description, int value, bool writable, bool valid);
    QHash<int, dxlProperty>  mPropertyList;

};

#endif // DXL_H
