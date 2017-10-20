#include "imoddatabase.h"

#include <QtCore/QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtCore/QFile>
#include <QtCore/QDataStream>

#include "imodutile.h"

iModDatabase::iModDatabase(iModUtile* _utile, QObject *parent) :
  QObject(parent),
  utile(_utile),
  currentframe(-1),
  databasebussy(false)
{
  database = QSqlDatabase::addDatabase("QMYSQL");
  database.setHostName(utile->readSetting("iModMiddleware/MySqlAdress").toString());
  database.setDatabaseName(utile->readSetting("iModMiddleware/MySqlDatabase").toString());
  database.setUserName(utile->readSetting("iModMiddleware/MySqlUser").toString());
  database.setPassword(utile->readSetting("iModMiddleware/MySqlPassword").toString());

  if(database.open())
    qDebug() << "Database open" << utile->readSetting("iModMiddleware/MySqlAdress").toString() << utile->readSetting("iModMiddleware/MySqlDatabase").toString();
  else
    qDebug() << "Can't open Database" << database.lastError().text();
}

iModDatabase::~iModDatabase()
{
  if(database.isOpen())
    database.close();
}

void iModDatabase::updateCategorys()
{
  categorylist.clear();
  QSqlQuery query;
  bool success = query.exec("SELECT distinct(Category) FROM iMod_Root");
  if(!success) return;
  int fieldNo = query.record().indexOf("Category");
  while (query.next())
  {
    QString name = query.value(fieldNo).toString();
    categorylist.append(name);
  }
}

QString iModDatabase::readDouble(char* bytes, int* offset)
{
  bytes += *offset;

  double d;
  memcpy(&d, bytes, sizeof(double));

  *offset += sizeof(double);
  return QString::number(d);
}

QStringList iModDatabase::getTablesFromCategory(QString category)
{
  QStringList tables;
  QSqlQuery query;
  bool success = query.exec("SELECT Name FROM iMod_Root WHERE Category = '" + category + "'");
  if(!success) return tables;
  int fieldNo = query.record().indexOf("Name");
  while (query.next())
  {
    QString name = query.value(fieldNo).toString();
    tables.append(name);
  }
  return tables;
}

void iModDatabase::loadTable(QString category, QString name)
{
  currentcategory = category;
  currenttable = name;
  databasebussy = true;
  frames.clear();
  // Code nimmt an, dass 6d daten da sind... überarbeiten -> mit JOIN's prüfen! -> Weil wegen UR5 später
  qDebug() << "Load Table" << name << "from" << category;
  QSqlQuery query;
  bool success = query.exec("SELECT * FROM ART_Tracking WHERE Tracking_id = (SELECT id FROM iMod_Root WHERE Category = '" + category + "' AND Name = '" + name + "')");
  int dbid_id = query.record().indexOf("id");
  if(success)
  {
    while(query.next())
    {
      QSqlQuery queryframe;
      success = queryframe.exec("SELECT * FROM ART_6d WHERE ART_Tracking_id = " + query.value(dbid_id).toString());
      //qDebug() << query.value(dbid_id).toString();
      int targetcount = 0;
      QString framelist = "";
      while(queryframe.next())
      {
        targetcount++;
        framelist += "[" + queryframe.value("target").toString() + " 1.000]";
        framelist += "[" + queryframe.value("px").toString() + " " + queryframe.value("py").toString() + " " + queryframe.value("pz").toString() +  " 0.000 0.000 0.000]";
        framelist += "[" + QString::number(queryframe.value("m00").toDouble(), 'f', 4) + " " + QString::number(queryframe.value("m01").toDouble(), 'f', 4) + " " + QString::number(queryframe.value("m02").toDouble(), 'f', 4) + " "
                     + QString::number(queryframe.value("m10").toDouble(), 'f', 4) + " " + QString::number(queryframe.value("m11").toDouble(), 'f', 4) + " " + QString::number(queryframe.value("m12").toDouble(), 'f', 4) + " "
                     + QString::number(queryframe.value("m20").toDouble(), 'f', 4) + " " + QString::number(queryframe.value("m21").toDouble(), 'f', 4) + " " + QString::number(queryframe.value("m22").toDouble(), 'f', 4) + "] ";
        // Build Frame
      }
      framelist = framelist.left(framelist.size() - 1);
      framelist = "fr %1\r\n6d " + QString::number(targetcount) + " " + framelist + "\r\n";
      frames.append(framelist);
    }
    currentframe = 0;
  }
  qDebug() << "Finished loading";
  databasebussy = false;
}

QString iModDatabase::getNextFrame()
{
  if(!frames.isEmpty() && currentframe >= 0)
  {
    QString frame = frames[currentframe];
    currentframe++;
    if(currentframe >= frames.count() )
      currentframe = 0;
    return frame;
  }
  return "";
}

void iModDatabase::writeRecording(QString category, QString name, QStringList data, bool art)
{
  if(data.count() == 0)
    return;

  databasebussy = true;
  //Insert into iMod_Root
  qDebug() << "Writing back recording with" << data.count() << "frames. ART:" << art;
  QSqlQuery query;
  bool success;

  success = query.exec("START TRANSACTION");
  qDebug() << "Start Transaction:" << success;

  int trackingid = -1;
  success = query.prepare("SELECT id FROM iMod_Root WHERE Name='"+name+"' AND Category='"+category+"'");
  success = query.exec();
  if(success)
    if(query.next())
      trackingid = query.value(0).toInt();

  if(trackingid == -1)
  {
    success = query.prepare("INSERT INTO iMod_Root (Name, Category) VALUES ('"+name+"', '"+category+"')");
    success = query.exec();
    if(success)
    {
      trackingid = query.lastInsertId().toInt();
      qDebug() << "Insert new Table:" << success << "ID:" << trackingid;
    }
    else
    {
      qDebug() << "Insert new Table:" << success;
      qDebug() << query.lastError().text();
    }
  }

  QString trackingtable = art?"ART_Tracking":"UR5_Vec6";
  success = query.prepare("SELECT MAX(id) FROM " + trackingtable);
  success = query.exec();
  int frameid;
  if(success)
  {
    query.next();
    frameid = query.value(0).toInt();
    qDebug() << "Next Frame id:" << frameid;
  }
  else
  {
    qDebug() << "kind of an error...";
    return;
  }

  QStringList queryTrackingList;
  QString queryTracking;
  if(art)
    queryTracking = "INSERT INTO ART_Tracking (Tracking_id, fr, ts) VALUES ";
  else
    queryTracking = "INSERT INTO UR5_Tracking_Simple (Tracking_id, time, robotstate, handstate, blenderjoints) VALUES ";
  QStringList query6dList;
  QString query6d;
  if(art)
    query6d = "INSERT INTO ART_6d (ART_Tracking_id, target, q, px, py, pz, px2, py2, pz2, m00, m01, m02, m10, m11, m12, m20, m21, m22) VALUES ";
  else
    query6d = "INSERT INTO UR5_Vec6 (a1, a2, a3, a4, a5, a6) VALUES ";
  QString lockstring = "LOCK TABLES " + trackingtable + " WRITE";
  int internalframes = 1;

  if(art)
  {
    foreach(QString frame, data)
    {
      foreach (QString dataparts, frame.split("\r\n"))
      {
        // Parse all stuff
        QString datatype = dataparts.left(dataparts.indexOf(" "));

        if(datatype == "fr")
        {
          frameid++;
          if(!frame.contains("ts ")) // if no timestamp, insert without it...
          {
            queryTracking += "("+QString::number(trackingid)+", "+QString::number(internalframes)+", -1), ";
            internalframes++;
            if(queryTracking.size() > 500000)
            {
              queryTrackingList.append(queryTracking.left(queryTracking.size() - 2));
              queryTracking = "INSERT INTO ART_Tracking (Tracking_id, fr, ts) VALUES ";
            }
          }
        }
        else if(datatype == "ts")
        {
          queryTracking += "("+QString::number(trackingid)+", "+QString::number(internalframes)+", "+dataparts.split(" ")[1]+"), ";
          internalframes++;
          if(queryTracking.size() > 500000)
          {
            queryTrackingList.append(queryTracking.left(queryTracking.size() - 2));
            queryTracking = "INSERT INTO ART_Tracking (Tracking_id, fr, ts) VALUES ";
          }
        }
        else if(datatype == "6dcal")
        {

        }
        else if(datatype == "6d")
        {
          if(!lockstring.contains("ART_6d"))
            lockstring += ", ART_6d WRITE";

          dataparts = dataparts.right(dataparts.size() - dataparts.indexOf("[") - 1);
          dataparts = dataparts.left(dataparts.size() - 1);
          foreach(QString target, dataparts.split("] [")) // letztes ] fehlt .. 1. aus beim nächsten
          {
            QStringList parts = target.split("][");
            query6d += "("+QString::number(frameid)+", "+parts[0].split(" ")[0]+", 1.000" +                                   // Frame, Target, Quality
                ", " + parts[1].split(" ")[0] + ", " + parts[1].split(" ")[1] + ", " + parts[1].split(" ")[2] +        // Position 1
                ", " + parts[1].split(" ")[3] + ", " + parts[1].split(" ")[4] + ", " + parts[1].split(" ")[5] +        // Position 2
                ", " + parts[2].split(" ")[0] + ", " + parts[2].split(" ")[1] + ", " + parts[2].split(" ")[2] +        // Rotation Row 1
                ", " + parts[2].split(" ")[3] + ", " + parts[2].split(" ")[4] + ", " + parts[2].split(" ")[5] +        // Rotation Row 2
                ", " + parts[2].split(" ")[6] + ", " + parts[2].split(" ")[7] + ", " + parts[2].split(" ")[8] + "), ";  // Rotation Row 3
            if(query6d.size() > 500000)
            {
              query6dList.append(query6d.left(query6d.size() - 2));
              query6d = "INSERT INTO ART_6d (ART_Tracking_id, target, q, px, py, pz, px2, py2, pz2, m00, m01, m02, m10, m11, m12, m20, m21, m22) VALUES ";
            }
          }
        }
        else if(datatype == "6di")
        {

        }
        else if(datatype == "3d")
        {

        }
        else if(datatype == "6df2")
        {

        }
        else if(datatype == "gl")
        {

        }
        else if(datatype == "glcal")
        {

        }
      }
    }
  }
  else // UR5
  {
    foreach(QString state, data)
    {
      if(!lockstring.contains("UR5_Tracking_Simple"))
        lockstring += ", UR5_Tracking_Simple WRITE";

      if(!lockstring.contains("UR5_Vec6"))
        lockstring += ", UR5_Vec6 WRITE";

      QStringList parts = state.split("#");
      // DEBUG //////// write stuff to file ////////////////
      QFile file(QString::fromStdString(getenv("IMOD_ROOT")) + "tmp/BlenderDump.dat");
      file.open(QIODevice::Append);
      file.write(QString(parts[1] + "\n").toUtf8());
      file.flush();
      file.close();
      /////////////////////////////////////////////////////
      QByteArray robotstate = QByteArray::fromBase64(parts[0].toUtf8());

      int offset = 4; // timeoffset
      QString tmp = readDouble(robotstate.data(), &offset);

      frameid++;

      queryTracking += "("+QString::number(trackingid)+", "+tmp+", '"+parts[0]+"', "+QString::number(-1)+", " + QString::number(frameid) + "), ";
      if(queryTracking.size() > 500000)
      {
        queryTrackingList.append(queryTracking.left(queryTracking.size() - 2));
        queryTracking = "INSERT INTO UR5_Tracking_Simple (Tracking_id, time, robotstate, handstate, blenderjoints) VALUES ";
      }

      query6d += "("+parts[1]+"), ";
      if(query6d.size() > 500000)
      {
        query6dList.append(query6d.left(query6d.size() - 2));
        query6d = "INSERT INTO UR5_Vec6 (a1, a2, a3, a4, a5, a6) VALUES ";
      }
    }
  }
  query6dList.append(query6d.left(query6d.size() - 2));
  queryTrackingList.append(queryTracking.left(queryTracking.size() - 2));

  // Push to database
  success = query.exec(lockstring);
  qDebug() << "Locking Tables:" << success;

  for(int i = 0; i < 2; i++)
  {
    QStringList list;
    if(art && i == 0) list = queryTrackingList;
    if(art && i == 1) list = query6dList;
    if(!art && i == 0) list = query6dList;
    if(!art && i == 1) list = queryTrackingList;
    foreach(QString querystring, list)
    {
      if(!querystring.endsWith("VALUES "))
      {
        if(success)
        {
          success = query.prepare(querystring);
          qDebug() << "Preparing Frames:" << success << query.lastError().text() << (success?" ":queryTracking);
        }
        if(success)
        {
          success = query.exec();
          qDebug() << "Inserting Frames:" << success << query.lastError().text() << (success?" ":queryTracking);
        }
      }
    }
  }

  if(success)
  {
    success = query.exec("COMMIT");
    qDebug() << "Commiting Changes:" << success << query.lastError().text();
  }
  else
  {
    success = query.exec("ROLLBACK");
    qDebug() << "Rollback Changes:" << success << query.lastError().text();
  }
  success = query.exec("UNLOCK TABLES");
  qDebug() << "Unlocking Tables" << success;
  databasebussy = false;
}

QString iModDatabase::getStatusString()
{
  QString result;
  result += "<h4>Database</h4>";
  result += QString("Current category: %1 <br>").arg(getCurrentCategory());
  result += QString("Current table: %1 <br>").arg(getCurrentTable());
  result += QString("Recording: %1 <br>");
  result += "<a href=\"ssr\">Start/Stop Recording</a><br>";
  return result;
}


