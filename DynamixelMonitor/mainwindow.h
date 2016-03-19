#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "dxlcom.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onPortChange(QString portName);
    void onBaudRateChange(int baudNum);
    void onServoIdChange(QString id);
    void scanServosId();
    void cellChanged (int nRow, int nCol);

private:
    void fillTableWidget(const int id);
    void setupTableWidget(const int nRow);
    int addTableWidgetRow(int row, dxl::dxlProperty property);  // return the next row;

    Ui::MainWindow *ui;
    //dxlCom *mDxl;


};

#endif // MAINWINDOW_H
