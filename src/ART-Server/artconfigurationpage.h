#ifndef CONSOLE_MODE
#ifndef ARTCONFIGURATIONPAGE_H
#define ARTCONFIGURATIONPAGE_H

#include <QtCore/QMap>
#include <QtWidgets/QTableWidget>

class ARTConfigurationPage : public QTableWidget
{
    Q_OBJECT
  public:
    explicit ARTConfigurationPage(QString groupname, QWidget *parent = 0);
    ~ARTConfigurationPage();

    void setValues(QMap<QString, QString> values);

  signals:
    void signalItemChanged(QString, QString);

  public slots:
    void slotItemChanged(QTableWidgetItem * item);

  private:
    void setParentMinimumHeight(int height);

    QString groupname;
};

#endif // ARTCONFIGURATIONPAGE_H
#endif
