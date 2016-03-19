#ifndef PORTCOMBOBOX_H
#define PORTCOMBOBOX_H


#include <QComboBox>

class portComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit portComboBox(QWidget *parent = 0);
    virtual ~portComboBox();
    void showPopup();
    int currentPort();
signals:
    void portChanged(QString portName);
private slots:
    void onPortChange (int index);
private:
    void populatePortList();

};

#endif // PORTCOMBOBOX_H
