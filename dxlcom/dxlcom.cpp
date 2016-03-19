#include "dxlcom.h"

#include <QDebug>


dxlCom* dxlCom::mInstance = 0;

dxlCom* dxlCom::getInstance(QObject *parent)
{
    if (mInstance == 0)
        mInstance = new dxlCom(parent);

    return mInstance;
}

dxlCom::dxlCom(QObject *parent) :
    QSerialPort(parent), mDelayForOneByte(1), mDxlError(NoError)
{
    // dynamixel standard protocol (http://support.robotis.com/en/product/dynamixel/dxl_communication.htm)
    // it is default values for QSerialPort, we write it nevertheless

    setDataBits(QSerialPort::Data8);
    setStopBits(QSerialPort::OneStop);
    setParity(QSerialPort::NoParity);
    setFlowControl(QSerialPort::NoFlowControl);

    mListServo.clear();
}

dxlCom::~dxlCom()
{
    qDeleteAll(mListServo);
    mListServo.clear();

    close();

    mInstance = 0;
}

bool dxlCom::setBaudData(int baudData)
{
    quint32 baudrate = (quint32)(2000000.0f / (float)(baudData + 1));

    mDelayForOneByte = (int)(10000.0f / (float) baudrate) + 1; //  1 000 ms * 10 (8 bit + 1 start + 1 stop) rounded up
    return setBaudRate(baudrate);
}


bool dxlCom::ping(int id)
{
    mDxlError = writeDxl(id, INST_PING);

    if (mDxlError == NoError)
        return true;
    else
        return false;
}

int dxlCom::scanServos()
{
    if (!(isOpen()))
    {
        qDebug()<<"unable to scan, the port is not open.";
        return 0;
    }

    qDeleteAll(mListServo);
    mListServo.clear();

    qDebug()<<"Scanning on port : "<<portName()<<" - BaudData : "<<baudRate();

    for(int i=0;i<253;i++)  // Max ID is 252, 254 is used to broadcast
    {
        if(ping( i ))
            mListServo.insert(i, dxl::getInstance(i));

    }
    qDebug()<<mListServo.count()<<" servo(s) found";
    return mListServo.count();

}

QList<int> dxlCom::getServoIdList() const
{     return mListServo.keys(); }


bool dxlCom::idExist(int id) const
{    return mListServo.contains(id); }

dxl* dxlCom::getServo(const int id)
{   return mListServo.value(id);    }


dxlCom::dxlError dxlCom::writeDxlData(const int id, const int dxlAddress, const int value, const int nBytesToBeWritten)
{
    QByteArray sentenceToWrite;
    sentenceToWrite.append((char) dxlAddress);

    for(int i = 0; i != nBytesToBeWritten; ++i)
        sentenceToWrite.append((char)((value & (0xFF << (i*8))) >> (i*8)));

    return writeDxl(id, INST_WRITE, sentenceToWrite);
}

dxlCom::dxlError dxlCom::writeDxl(const int id, const int dxlCommand, const QByteArray &parameters )
{

    int txDataLength =  parameters.size() + 2 ;    // length is "number of parameters N +2"
    QByteArray sentence(2, (char) INST_START);
    char Checksum = 0;

    foreach(char byte, parameters)
        Checksum += byte;

    Checksum = (~(id + txDataLength  + dxlCommand + Checksum )) & 0xFF;

    sentence.append((char) id);
    sentence.append((char) txDataLength);
    sentence.append((char) dxlCommand);
    sentence.append(parameters);
    sentence.append(Checksum);

    clear(QSerialPort::AllDirections);  // clear input & ouput buffer

    write(sentence);


    if(!waitForBytesWritten( mDelayForOneByte * sentence.size() ))
    {
        qDebug()<<"tx timeout";
        return TxTimeoutError;
    }

    if (!waitForReadyRead(mDelayForOneByte * 7)) //adding 1 wait for status return
    {
        qDebug()<<"rx timeout";
        return RxTimeoutError;
    }

    QByteArray responseData = readAll();

    while (waitForReadyRead(mDelayForOneByte))
        responseData += readAll();

    if(responseData.size()!=6)
    {
        qDebug()<<QString("status pack length %1 is incorrect").arg(responseData.size());
        return DataSizeError;
    }

    if (!( ( responseData[0] == (char) INST_START) & ( responseData[1]  == (char) INST_START) ))
    {
        qDebug()<<"No dxl header found";
        return  NoHeaderError;
    }

    if (responseData[2] != (char)id)
    {
        qDebug()<<"The response comes from another Servo !";
        return  IdError;
    }

    if (responseData[3] != (char)2)
    {
        qDebug()<<QString("unexpected length of status packet %1").arg(responseData[3]);
        return  LengthError;
    }


    if (responseData[4] != (char)0)
    {
        qDebug()<<QString("servo send error code %1").arg(responseData[4]);
        return HardwareError;
    }

    if ((char)responseData[5] != (char)((~(responseData[2] + responseData[3]  + responseData[4] )) & 0xff ))
    {
        qDebug()<<"rx checksum error";
        return ChecksumError;
    }

    return NoError;

}

int dxlCom::readDxlData(const int id, const int dxlAddress, const int nBytesToBeRead)
{

    QByteArray sentence(2, (char) INST_START);
    char Checksum = (~(id + 4 + INST_READ + dxlAddress + nBytesToBeRead )) & 0xFF;

    sentence.append((char) id);
    sentence.append((char) 4);
    sentence.append((char) INST_READ);
    sentence.append((char) dxlAddress);
    sentence.append((char) nBytesToBeRead);
    sentence.append(Checksum);

    clear(QSerialPort::AllDirections);  // clear input & ouput buffer

    write(sentence);


    if(!waitForBytesWritten( mDelayForOneByte * sentence.size() ))
    {
        qDebug()<<"tx timeout";
        mDxlError = TxTimeoutError;
        return -1;
    }

    if (!waitForReadyRead(mDelayForOneByte * ( 6 + nBytesToBeRead ) )) //adding 1 wait for status return
    {
        qDebug()<<"rx timeout";
        mDxlError = RxTimeoutError;
        return -1;
    }

    QByteArray responseData = readAll();

    while (waitForReadyRead(mDelayForOneByte))
        responseData += readAll();

    if(responseData.size() != ( 6 + nBytesToBeRead ))
    {
        qDebug()<<QString("status pack length %1 is incorrect").arg(responseData.size());
        mDxlError = DataSizeError;
        return -1;
    }

    if (!( ( responseData[0] == (char) INST_START) & ( responseData[1]  == (char) INST_START) ))
    {
        qDebug()<<"No dxl header found";
        mDxlError = NoHeaderError;
        return  -1;
    }

    if (responseData[2] != (char) id )
    {
        qDebug()<<"The response comes from another Servo !";
        mDxlError = IdError;
        return  -1;
    }

    if (responseData[3] != (char) (2 + nBytesToBeRead) )
    {
        qDebug()<<QString("unexpected length of status packet %1").arg(responseData[3]);
        mDxlError = LengthError;
        return  -1;
    }


    if (responseData[4] != (char) 0 )
    {
        qDebug()<<QString("servo send error code %1").arg(responseData[4]);
        mDxlError = HardwareError;
        return -1;
    }

    QByteArray data = responseData.mid( 5, nBytesToBeRead);

    Checksum = 0;

    foreach(char byte, data)
        Checksum += byte;

    Checksum = (~(responseData[2] + responseData[3]  + responseData[4] + Checksum)) & 0xff ;


    if ((char)responseData.at(responseData.size()-1) != Checksum)
    {
        qDebug()<<"rx checksum error";
        mDxlError = ChecksumError;
        return -1;
    }

    int result = 0;
    for (int i = 0; i < data.size(); ++i)
        result |= ((int)data[i] & 0x000000ff) << (i * 8);

    return result;

}
