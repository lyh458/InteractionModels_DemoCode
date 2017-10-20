#ifndef CONSOLE_MODE
#include "artconfiguration.h"

#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtGui/QPainter>
#include <QtWidgets/QToolBox>

#include "artconfigurationpage.h"

ARTConfiguration::ARTConfiguration(QWidget *parent) : QToolBox(parent),
  settingsfile(NULL)
{
  readSetting("ART/ServerAdress");
  if(settingsfile == NULL)
  {
    emit this->signalDisplayLogMessage("Error: Cant read from settings file!");
    return;
  }

  //resize(size);
  move(0,0);

  // Create Tabs
  QStringList keys = settingsfile->allKeys();
  QList<QString> groups;
  foreach(QString key, keys)
  {
    QString name = key.split("/")[0];
    if(!groups.contains(name))
      groups.append(name);
  }
  qSort(groups);

  foreach(QString group, groups)
  {
    ARTConfigurationPage* newpage = new ARTConfigurationPage(group);
    addItem(newpage, group);

    QMap<QString, QString> values;
    foreach(QString key, getGroupKeys(group))
      values.insert(key, settingsfile->value(group + "/" + key).toString());

    newpage->setValues(values);

    connect(newpage, &ARTConfigurationPage::signalItemChanged,
            this,    &ARTConfiguration::slotItemChanged);
  }

  show();
}

ARTConfiguration::~ARTConfiguration()
{

}

QVariant ARTConfiguration::readSetting(QString key)
{
  QMutexLocker locker(&settinglock);

  if(settingsfile == NULL)
  {
    if(getenv("IMOD_ROOT") == NULL)
    {
      emit this->signalDisplayLogMessage("IMOD_ROOT not set. Aborting");
      return "";
    }
    QDir configPath = QDir((QString::fromStdString(getenv("IMOD_ROOT")) + "/CPP/src/"));
    if(!QFileInfo(configPath, "configuration.ini").exists())
    {
      emit this->signalDisplayLogMessage("Can't load configuration.ini from " + configPath.path() + ". Aborting.");
      return "";
    }
    settingsfile = new QSettings(configPath.path() + "/configuration.ini", QSettings::IniFormat, 0);
  }
  return settingsfile->value(key, "");
}

QStringList ARTConfiguration::getGroupKeys(QString group)
{
  settingsfile->beginGroup(group);
  QStringList artkeys = settingsfile->childKeys();
  settingsfile->endGroup();

  return artkeys;
}

void ARTConfiguration::paintEvent(QPaintEvent *event)
{
  QToolBox::paintEvent(event);
}

void ARTConfiguration::slotItemChanged(QString key, QString value)
{
  settingsfile->setValue(key, value);
}
#endif
