#ifndef CONSOLE_MODE
#ifndef ARTCONFIGURATION_H
#define ARTCONFIGURATION_H

#include <QtWidgets/QToolBox>
#include <QtCore/QMutex>

class QSettings;

class ARTConfiguration : public QToolBox
{
    Q_OBJECT
  public:
    explicit ARTConfiguration(QWidget *parent = 0);
    ~ARTConfiguration();

  public:
    QVariant readSetting(QString key);
    QStringList getGroupKeys(QString group);

  protected:
    virtual void paintEvent(QPaintEvent *event);
    //virtual QSize sizeHint() const {  }

  signals:
    void signalDisplayLogMessage(QString);

  public slots:
    void slotItemChanged(QString key, QString value);

  private:
    QSettings* settingsfile;
    QMutex settinglock;
};

#endif // ARTCONFIGURATION_H
#endif
