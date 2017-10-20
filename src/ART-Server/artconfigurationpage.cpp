#ifndef CONSOLE_MODE
#include "artconfigurationpage.h"

#include <QtWidgets/QHeaderView>

ARTConfigurationPage::ARTConfigurationPage(QString groupname, QWidget *parent) : QTableWidget(parent)
{
  this->groupname = groupname;
  verticalHeader()->hide();
  horizontalHeader()->hide();
  horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  setColumnCount(2);

  connect(this, &QTableWidget::itemChanged,
          this, &ARTConfigurationPage::slotItemChanged);
}

ARTConfigurationPage::~ARTConfigurationPage()
{

}

void ARTConfigurationPage::setValues(QMap<QString, QString> values)
{
  setRowCount(values.count());
  setMinimumHeight(verticalHeader()->defaultSectionSize() * values.count() + 2);
  setParentMinimumHeight(verticalHeader()->defaultSectionSize() * values.count() + 2);

  int row = 0;
  foreach(QString s, values.keys())
  {
    QTableWidgetItem* item = new QTableWidgetItem(s);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);

    setItem(row, 0, item);
    setItem(row, 1, new QTableWidgetItem(values[s]));

    row ++;
  }
}

void ARTConfigurationPage::setParentMinimumHeight(int height)
{
  QWidget* parent = this->parentWidget();
  if (parent)
  {
    QWidget* gparent = parent->parentWidget();
    if (gparent)
    {
      gparent->setMinimumHeight(height);
    }
  }
}

void ARTConfigurationPage::slotItemChanged(QTableWidgetItem *item)
{
  QTableWidgetItem* keyitem = QTableWidget::item(item->row(), 0);
  emit this->signalItemChanged(groupname + "/" + keyitem->text(), item->text());
}
#endif
