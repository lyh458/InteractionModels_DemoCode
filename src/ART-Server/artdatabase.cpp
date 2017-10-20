#include "artdatabase.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtCore/QVariant>

#include "artringbuffer.h"

ARTDatabase::ARTDatabase(QObject* parent) :
  QObject(parent),
  databaseopen(false),
  buffer(new ARTRingBuffer())
{

}

void ARTDatabase::slotConnectDatabase(QString host, QString database, QString user, QString password)
{
  sqldatabase = QSqlDatabase::addDatabase("QMYSQL");
  if(!sqldatabase.isValid())
  {
    emit this->signalDisplayLogMessage("Error while loading database driver");
    return;
  }

  sqldatabase.setHostName(host);
  sqldatabase.setDatabaseName(database);
  sqldatabase.setUserName(user);
  sqldatabase.setPassword(password);
  databaseopen = sqldatabase.open();
  if(databaseopen)
    emit this->signalDisplayLogMessage("Database connected");
  else
    emit this->signalDisplayLogMessage("Cant connect database");
}

void ARTDatabase::slotGetAllTables()
{
  if(!databaseopen)
    emit this->signalDisplayLogMessage("Database not open: retry later");
  else
    emit this->signalSetTableList(sqldatabase.tables());
}

void ARTDatabase::slotFillBuffer(QStringList tables)
{
  bool error = false;
  buffer->clearBuffer();
  foreach(QString table, tables)
  {
    emit this->signalDisplayLogMessage("Parsing " + table);
    //Look for framenumbers
    QSqlQuery qry;
    qry.prepare( "SELECT DISTINCT framecounter FROM " + table );
    if( !qry.exec() )
    {
      emit this->signalDisplayLogMessage(qry.lastError().text());
      error = true;
    }
    else
    {
      QStringList framenumbers;
      while( qry.next() )
        framenumbers.append(qry.value("framecounter").toString());

      foreach(QString frame, framenumbers)
      {
        qry.prepare( "SELECT id, loc_1, loc_2, loc_3, rot_1, rot_2, rot_3, rot_4, rot_5, rot_6, rot_7, rot_8, rot_9 FROM " + table + " WHERE framecounter = " + frame );
        if( !qry.exec() )
        {
          emit this->signalDisplayLogMessage(qry.lastError().text());
          error = true;
        }
        else
        {
          QString bufferframe = "";
          int count = 0;
          while( qry.next() )
          {
            QSqlRecord rec = qry.record();
            count++;

            bufferframe += " [" + rec.value("id").toString() + " 1]";
            bufferframe += " [" + rec.value("loc_1").toString() + " " + rec.value("loc_2").toString() + " " + rec.value("loc_3").toString() + "]";
            bufferframe += " [" + rec.value("rot_1").toString() + " " + rec.value("rot_2").toString() + " " + rec.value("rot_3").toString() + " " + rec.value("rot_4").toString() +
                           " "  + rec.value("rot_5").toString() + " " + rec.value("rot_6").toString() + " " + rec.value("rot_7").toString() + " " + rec.value("rot_8").toString() +
                           " " + rec.value("rot_9").toString() + "]";
          }
          buffer->insert("6d " + QString::number(count) + bufferframe);
        }
      }
    }
  }
  emit this->signalBufferFinished(!error);
}

void ARTDatabase::slotClearBuffer()
{
  buffer->clearBuffer();
}

void ARTDatabase::slotResetBuffer()
{
  buffer->resetBuffer();
}

void ARTDatabase::slotSendFrame()
{
  emit this->signalSendFrame(buffer->getNextFrame());
}

