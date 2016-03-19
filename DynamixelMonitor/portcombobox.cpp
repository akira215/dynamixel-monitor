#include "portcombobox.h"


#include <QSerialPortInfo>
#include <QDebug>

portComboBox::portComboBox(QWidget *parent) : QComboBox(parent)
{

    connect(this, SIGNAL(activated(int)), this, SLOT(onPortChange(int)));
    populatePortList();
}

portComboBox::~portComboBox()
{
}


void portComboBox::showPopup()
{
   populatePortList();
   QComboBox::showPopup();
}

void portComboBox::populatePortList()
{
    clear();

    QList<QSerialPortInfo> listPorts = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo port, listPorts)
    {
        QString portDescription;

        if (!(port.description().isEmpty()))
                portDescription=" ("+port.description()+")";

        addItem(port.portName()+portDescription, port.portName());
    }
    setCurrentIndex(0);
    onPortChange(0);
}


void portComboBox::onPortChange (int index)
{    emit portChanged(itemData(index).toString()); }

int portComboBox::currentPort()
{    return itemData(currentIndex()).toInt(); }
