#include "dxlbaudratecombobox.h"

dxlBaudrateComboBox::dxlBaudrateComboBox(QWidget *parent) : QComboBox(parent)
{
    connect(this, SIGNAL(activated(int)), this, SLOT(onBaudrateChange(int)));
    populateBaudRateList();
}


int dxlBaudrateComboBox::currentDxlBaudrate()
{
    return itemData(currentIndex()).toInt();
}


void dxlBaudrateComboBox::onBaudrateChange (int index)
{   emit baudrateChanged(itemData(index).toInt()); }


dxlBaudrateComboBox::~dxlBaudrateComboBox()
{
}

void dxlBaudrateComboBox::populateBaudRateList()
{
    clear();
    for(int i=1; i<255; i++)
    {
        QString text =  QString::number( 2000000/( i + 1 )) + QString(" (")+ QString::number(i)  + QString(")");
        addItem(text, i);
    }

}
