#include "imodutile.h"

#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtCore/QDate>
#include <QtCore/QDebug>
#include <QtCore/QTimer>

iModUtile::iModUtile(QObject *parent) : QObject(parent)
{
  if(getenv("IMOD_ROOT"))
  {
    settingsfile = new QSettings( QString::fromStdString(getenv("IMOD_ROOT")) + "CPP/src/configuration.ini", QSettings::IniFormat, 0);
    logfile = new QFile(settingsfile->value("iModMiddleware/LoggerFilepath").toString());

    updatetimer = new QTimer(this);
    connect(updatetimer, &QTimer::timeout, this, &iModUtile::slotUpdateSettingsFile);
    updatetimer->setInterval(1000);
  }
  else
    qDebug() << "Cant find enviroment variable IMOD_ROOT";
}

iModUtile::~iModUtile()
{
  delete settingsfile;
  delete logfile;
}

void iModUtile::writeLog(QString message)
{
  logfile->open(QIODevice::Append);
  if(logfile->isOpen())
  {
    QTextStream out(logfile);
    message = "[" + QDate::currentDate().toString("dd.MM.yyyy") + " " + QTime::currentTime().toString("HH:mm:ss")  + "] " + message + "\n";
    out << message;
    logfile->close();
  }
  else
  {
    qDebug() << "Logfile not open";
  }
}

QVariant iModUtile::readSetting(QString path)
{
  if(!updatetimer->isActive())
    updatetimer->start();
  return settingsfile->value(path, QVariant::Invalid);
}

QStringList iModUtile::getAllKeys(QString category)
{
  QStringList keys = settingsfile->allKeys();
  if(category == "")
    return keys;

  QStringList filterkeys;
  foreach(QString s, keys)
  {
    if(s.startsWith(category + "/"))
      filterkeys.append(s.split("/")[1]);
  }
  return filterkeys;
}

void iModUtile::slotUpdateSettingsFile()
{
  // Imports changes made by other processes (in addition to writing the changes from this QSettings)
  settingsfile->sync();
}
