#include "imodriftnetwork.h"

#include <QtCore/QFile>
#include <QtGui/QVector4D>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QFile> 

#include "OSGViewer/oculusdevice.h"

iModRiftNetwork::iModRiftNetwork(OculusDevice* _od, QObject *parent) : QThread(parent), od(_od)
{
    writeLog("Started Network");
    if(!od)
        writeLog("Oculus Device is invalid!");

    if(getenv("IMOD_ROOT") == NULL)
    {
        qDebug() << "IMOD_ROOT not set. Aborting";
        settingsfile = nullptr;
    }
    else
    {
        QDir configPath = QDir((QString::fromStdString(getenv("IMOD_ROOT")) + "/CPP/src/"));
        if(!QFileInfo(configPath, "configuration.ini").exists())
        {
            qDebug() << "Can't load configuration.ini from " + configPath.path() + ". Aborting.";
            settingsfile = nullptr;
        }
        else
            settingsfile = new QSettings(configPath.path() + "/configuration.ini", QSettings::IniFormat, 0);
    }

    if(settingsfile != nullptr)
    {
        udpport = settingsfile->value("iModMiddleware/OculusDestinationPort").toInt();
    }
    else
    {
        qDebug() << "Cant read information from settings file. Listening on 33888";
        udpport = 33888;
    }
    
    logfile = new QFile("trackingdata.txt");
}

iModRiftNetwork::~iModRiftNetwork()
{
    delete socket;
}

void iModRiftNetwork::writeLog(QString message)
{
    /*
    QFile file("rift.log");
    file.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&file);
    out << message + "\n";

    // optional, as QFile destructor will already do it:
    file.close();
    */
}

void iModRiftNetwork::run()
{
    socket = new QUdpSocket(this);

    if(!socket->bind(QHostAddress::Any, udpport))
        writeLog("Error, cant bind UDP socket");
    else
        writeLog("Seems to be connected");

    while(true)
    {
        if (socket->hasPendingDatagrams())
        {
            QByteArray datagram;
            datagram.resize(socket->pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;

            socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
            processDatagram(datagram);
        }
        msleep(10); // offers round about 100 FPS;
    }
}

void iModRiftNetwork::processDatagram(QByteArray datagram)
{
	writeFile(QString(datagram));
	
    artposition.setX(QString(datagram).split(" ", QString::SkipEmptyParts)[0].toDouble());
    artposition.setY(QString(datagram).split(" ", QString::SkipEmptyParts)[1].toDouble());
    artposition.setZ(QString(datagram).split(" ", QString::SkipEmptyParts)[2].toDouble());
    
    artrotation.setScalar(QString(datagram).split(" ", QString::SkipEmptyParts)[3].toDouble());
    artrotation.setX(QString(datagram).split(" ", QString::SkipEmptyParts)[4].toDouble());
    artrotation.setY(QString(datagram).split(" ", QString::SkipEmptyParts)[5].toDouble());
    artrotation.setZ(QString(datagram).split(" ", QString::SkipEmptyParts)[6].toDouble());
    
    //qDebug() << artposition << artrotation;
}

void iModRiftNetwork::writeFile(QString message)
{
  logfile->open(QIODevice::Append);
  if(logfile->isOpen())
  {
    QTextStream out(logfile);
    out << message + "\n";
    logfile->close();
  }
  else
  {
    qDebug() << "Logfile not open";
  }
}


