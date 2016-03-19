#ifndef DXLCOM_H
#define DXLCOM_H

#include "dxlcom_global.h"
#include "dxl.h"
#include <QSerialPort>
#include <QHash>

#define INST_START           (0xff)

#define INST_PING			(1)
#define INST_READ			(2)
#define INST_WRITE			(3)
#define INST_REG_WRITE		(4)
#define INST_ACTION			(5)
#define INST_RESET			(6)
#define INST_SYNC_WRITE		(131)

class DXLCOMSHARED_EXPORT dxlCom : public QSerialPort
{
     Q_OBJECT
public:
    enum dxlError
    {
        NoError = 0,
        NoHeaderError = 1,
        IdError = 2,
        LengthError = 3,
        ChecksumError = 4,
        DataSizeError = 5,
        HardwareError = 6,
        TxTimeoutError = 12,
        RxTimeoutError = 12,
    };

    static dxlCom* getInstance(QObject *parent = Q_NULLPTR);

    ~dxlCom();

    bool setBaudData(int baudData);
    bool ping(int id);
    int scanServos();

    QList<int> getServoIdList() const;
    bool idExist(int id) const;

    dxl* getServo(const int id);

    dxlError dxlerror() { return mDxlError; }
    void clearDxlError() { mDxlError = NoError; }

     int readDxlData(const int id, const int dxlAddress, const int nBytesToBeRead);
     dxlError writeDxlData(const int id, const int dxlAddress, const int value, const int nBytesToBeWritten);

private:
    dxlCom(QObject *parent = Q_NULLPTR);
    static dxlCom* mInstance;
    QHash<int, dxl*> mListServo;
    int mDelayForOneByte;
    dxlError mDxlError;

    dxlError writeDxl(const int id, const int dxlCommand, const QByteArray &parameters = QByteArray() );

};

#endif // DXLCOM_H
