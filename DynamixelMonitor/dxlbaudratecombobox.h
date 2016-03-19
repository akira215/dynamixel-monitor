#ifndef DXLBAUDRATECOMBOBOX_H
#define DXLBAUDRATECOMBOBOX_H

#include <QComboBox>

class dxlBaudrateComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit dxlBaudrateComboBox(QWidget *parent = 0);
    virtual ~dxlBaudrateComboBox();
    int currentDxlBaudrate();

signals:
    void baudrateChanged(int dxlBaudrate);
private slots:
    void onBaudrateChange (int index);
private:
    void populateBaudRateList();
};

#endif // DXLBAUDRATECOMBOBOX_H
