#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include <QThread>

//#include "dxlcom.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusBar()->showMessage(tr("Initializing..."));

    connect(ui->cbPort, SIGNAL(portChanged(QString)), this, SLOT(onPortChange(QString)));
    connect(ui->cbBaudRate, SIGNAL(baudrateChanged(int)), this, SLOT(onBaudRateChange(int)));
    connect(ui->pbScan, SIGNAL(clicked()), this, SLOT(scanServosId()));
    connect(ui->cbIdList, SIGNAL(activated(QString)), this, SLOT(onServoIdChange(QString)));
    connect( ui->tableWidget, SIGNAL( cellChanged (int, int) ), this, SLOT( cellChanged( int, int ) ) );

}


MainWindow::~MainWindow()
{
    delete ui;
}

/*
void MainWindow::onPortChange(int portNum)
{

    DXLSERIALSHARED_EXPORT dxlSerial* dxl = dxlSerial::getInstance(this);



    if(mDxl->dxlComInitialize(portNum))
    {
        statusBar()->showMessage(tr("Succeed to open USB2Dynamixel."));
    }
    else
        statusBar()->showMessage(tr("Failed to open USB2Dynamixel."));
}

*/
void MainWindow::onPortChange(QString portName)
{

    if (portName.isEmpty())
        return;

    DXLCOMSHARED_EXPORT dxlCom* dxlHandle = dxlCom::getInstance(this);

    if (dxlHandle->isOpen())
        dxlHandle->close();

    dxlHandle->setPortName(portName);

    if(dxlHandle->open(QIODevice::ReadWrite))
    {
        statusBar()->showMessage(tr("Succeed to open %1.").arg(portName));
    }
    else
        statusBar()->showMessage(tr("Can't open %1, error code %2").arg(portName).arg(dxlHandle->error()));

}

void MainWindow::onBaudRateChange(int baudNum)
{


    DXLCOMSHARED_EXPORT dxlCom* dxlHandle = dxlCom::getInstance(this);

    if (dxlHandle->isOpen())
        dxlHandle->close();

    if(dxlHandle->portName().isNull())
    {
        dxlHandle->setPortName(ui->cbPort->itemData(ui->cbPort->currentIndex()).toString());
    }

    if(dxlHandle->setBaudData((quint32)baudNum))
    {
        if(dxlHandle->open(QIODevice::ReadWrite))
        {
            statusBar()->showMessage(tr("Succeed to setup baudrate to %1.").arg(baudNum));
        }
        else
            statusBar()->showMessage(tr("Error opening the port."));
    }
    else
        statusBar()->showMessage(tr("Failed to setup baudrate %1, error code %2").arg(baudNum).arg(dxlHandle->error()));

}

void MainWindow::onServoIdChange(QString id)
{
    DXLCOMSHARED_EXPORT dxlCom* dxlHandle = dxlCom::getInstance(this);

    if (dxlHandle->idExist(id.toInt()))
    {
        statusBar()->showMessage(tr("Collecting data for servo Id ") + id);
        fillTableWidget(id.toInt());
    }
    else
    {
        statusBar()->showMessage(tr("Servo Id ") + id + tr(" doesn't exist."));
        //ui->cbIdList->setCurrentText(QString::number(mDxl->getServoId()));
    }

}

void MainWindow::scanServosId()
{
    DXLCOMSHARED_EXPORT dxlCom* dxlHandle = dxlCom::getInstance(this);

    if (!(dxlHandle->isOpen()))
    {
        statusBar()->showMessage(tr("Cannot scan, device not initialized."));
        return;
    }
    statusBar()->showMessage(tr("Scanning..."));

    //dxlHandle->ping(1);


    int numServo = dxlHandle->scanServos();
    ui->cbIdList->clear();

    if (numServo == 0)
        statusBar()->showMessage(tr("No servo found. Check Port COM and baudrate an retry."));
    else
    {
        statusBar()->showMessage(QString::number(numServo) + tr(" servo(s) found"));

        foreach(int id, dxlHandle->getServoIdList() )
            ui->cbIdList->addItem(QString::number(id));

        // onServoIdChange(ui->cbIdList->currentText());
    }


}


void MainWindow::setupTableWidget(const int nRow)
{

    ui->tableWidget->clear();

    ui->tableWidget->setRowCount(nRow);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("#") << tr("Description") << tr("Value"));

    ui->tableWidget->setColumnWidth(0, 80);
    ui->tableWidget->setColumnWidth(1, 200);
    ui->tableWidget->setColumnWidth(2, 100);

    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(20);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch); // Only middle column is resizing

}

void MainWindow::fillTableWidget(const int id)
{
    int row=0;

    DXLCOMSHARED_EXPORT dxlCom* dxlHandle = dxlCom::getInstance(this);

    statusBar()->showMessage(tr("Receiving data..."));

    disconnect(ui->tableWidget, SIGNAL( cellChanged (int, int) ), 0,0); // disconnect to avoid useless write


    dxl* currentServo = dxlHandle->getServo(id);

    setupTableWidget(currentServo->getPropertyList().count());

    foreach (dxl::dxlProperty property, currentServo->getPropertyList())
    {
        property =  currentServo->getPropertyValue(property.address);
        row = addTableWidgetRow(row, property);
    }

     ui->tableWidget->sortItems(0);

    connect( ui->tableWidget, SIGNAL( cellChanged (int, int) ), this, SLOT( cellChanged( int, int ) ) );
    statusBar()->showMessage(tr("Data from servo received."));

}

int MainWindow::addTableWidgetRow(int row, dxl::dxlProperty property)
{

    QTableWidgetItem *item;

    if (property.valid)
    {
        item =  new QTableWidgetItem(QString("%1").arg(property.address, 2, 10, QChar('0')));

        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(row,0,item);

        item =  new QTableWidgetItem(property.description);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(row,1,item);

        item = new QTableWidgetItem(QString::number(property.value));
        if (property.writable)
        {
           item->setBackground(Qt::green);
           item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
        else
        {
           item->setBackground(Qt::red);
           item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        }
        ui->tableWidget->setItem(row,2,item);
        return ++row;
    }
    else
        return row;

}


void MainWindow::cellChanged(int nRow, int nCol)
{
    Q_UNUSED(nCol)

    statusBar()->showMessage(tr("Writing data to servo..."));

    DXLCOMSHARED_EXPORT dxlCom* dxlHandle = dxlCom::getInstance(this);

    //dxl* currentServo = mDxl->getServo(ui->cbIdList->currentText().toInt());
    dxl* currentServo = dxlHandle->getServo(ui->cbIdList->currentText().toInt());

    int address = ui->tableWidget->item(nRow, 0)->text().toInt();
    int value = ui->tableWidget->item(nRow, 2)->text().toInt();

    disconnect(ui->tableWidget, SIGNAL( cellChanged (int, int) ), 0,0); // disconnect to avoid useless write

    if(currentServo->setPropertyValue(address, value))
    {
        statusBar()->showMessage(tr("Data written in servo."));
        if(address == ADD_ID)
        {
            QThread::msleep(100);
            scanServosId(); // too fast !!!
            //ui->cbIdList->setItemText(ui->cbIdList->currentIndex(),QString::number(value));
        }
    }
    else
        statusBar()->showMessage(tr("An error occured during writting operation. Check the entered value and servo COM"));

    connect( ui->tableWidget, SIGNAL( cellChanged (int, int) ), this, SLOT( cellChanged( int, int ) ) );


}

