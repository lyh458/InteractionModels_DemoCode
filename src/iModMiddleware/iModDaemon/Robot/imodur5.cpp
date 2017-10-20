#include "imodur5.h"
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtCore/QCoreApplication>
#include <qmath.h>

#include "../imoddatabase.h"

static inline qint32 ArrayToInt(QByteArray source);

iModUR5::iModUR5(iModDatabase* db, QObject *parent) :
  iModRobot(parent),
  drivetoinit(false),
  ur5dumpfile(NULL)
{
  Q_UNUSED(db);
  robot_socket = new QTcpSocket(this);
  connect(robot_socket, &QTcpSocket::readyRead, this, &iModUR5::slotRobotIncomming);
  lastangle.reserve(6); // do it... we cant allocate memory in the reciver -> too slow
  for(int i = 0; i < 6; i++)
    lastangle.append(0);
  this->ros_socket = new QUdpSocket( this ); // http://doc.qt.io/qt-5/qtnetwork-programming.html
  //qDebug() << "[UDP2ROS] bound_state: " << this->ros_socket->BoundState;
  //this->ros_socket->bind(QHostAddress::LocalHost, 33997);

  QByteArray *buffer = new QByteArray();
  qint32 *s = new qint32(0);
  buffers.insert(robot_socket, buffer);
  sizes.insert(robot_socket, s);


  for(int i = 0; i < 3; i++)
    lasttcp.append(0);
}

iModUR5::~iModUR5()
{

}

QVector3D iModUR5::getLastTCP()
{
  QMutexLocker ml(&datamutex);
  return QVector3D(lasttcp[0], lasttcp[1], lasttcp[2]);
}

void iModUR5::slotRobotIncomming()
{
  qint32 packet_size = 1060; // ?
  while(robot_socket->bytesAvailable() > 0)
  {
    // Packetsize: 1060
    // Offset: q_actual 252
    // Offset: tool_actual 444
    robot_fpscount++;
    QByteArray buffer;
    buffer.resize(packet_size);
    for(int i = 0; i < buffer.size();)
    {
      int read = robot_socket->read(buffer.data() + i, buffer.size() - i);

      if(read > 0)
        i += read;
      else if(read == 0)
        robot_socket->waitForReadyRead(2000);
      else
      {
        qInfo() << "[iModUR5@slotRobotIncomming] Robot has disconnected";
        robot_socket->close();
        return;
      }
    }

    readCurrentAngle(buffer.data(), 252);
    readCurrentTCP(buffer.data(), 444);
    emit this->singalUpdateRobotTCP();

    if(outputmode == 0x01)
      ros_socket->writeDatagram(buffer.data(), buffer.size(), QHostAddress::Broadcast, 33997);
  }
}

void iModUR5::slotUR5Output(char type)
{
  outputmode = type;
  qDebug() << "UR5 output type:" << (int)type;

  if(ur5dumpfile && outputmode != 0x02)
  {
    ur5dumpfile->close();
    delete ur5dumpfile;
    ur5dumpfile = NULL;
  }

  if(outputmode == 0x02)
  {
    ur5dumpfile = new QFile("C:\\Users\\Hunabku\\Desktop\\robotdump.csv");
    if(ur5dumpfile->open(QIODevice::ReadWrite))
      qDebug() << "Opening Dumpfile: success";
    else
    {
      qDebug() << "Cant open Dumpfile";
      delete ur5dumpfile;
      ur5dumpfile = NULL;
    }
  }
}

void iModUR5::parseBlenderData(char opcode, QList<float> data)
{
  drivetoinit = false;

  switch(opcode)
  {
    case 0x01: // Movedata
      blender_fpscount++;
      break;
    case 0x02: // Move to Blender pose
      drivetoinit = true;
      break;
    default:
      qDebug() << "Found some strange codes...";
      break;
  }

  if(outputmode == 0x00) // Send to UR5
  {
    // For Debug -> This thing is blocking :P
    if(robot_socket->state() != QTcpSocket::ConnectedState || !robot_sending_allowed)
      return;

    // Handle drivetoinit
    if(drivetoinit)
    {
      QString command = "movej([" + vectorToString(&data, 0, 6) + "], t=10)\n";
      robot_socket->write(command.toUtf8());
      drivetoinit = false;
      return;
    }

    QList<float> result = capMovement(data.mid(0, 6));
    QString command = "movep([" + vectorToString(&result, 0, 6)  + "], a=" + QString::number(robot_speedup,'f',4) + ", v=" + QString::number(robot_speed,'f',4) + ", r=" + QString::number(robot_blending,'f',4) + ")\n";
    robot_socket->write(command.toUtf8());
  }
  else if(outputmode == 0x01) // ROS
  {
    if(robot_socket->state() != QTcpSocket::ConnectedState || !robot_sending_allowed)
      return;

    QList<double> ros_result;
    QList<float> tmp_result;

    if(drivetoinit)
    {
      for(int i = 0; i < 6; i++)
        tmp_result.append(data[i]);
      tmp_result.append(10.0);
      drivetoinit = false;
    }
    else
    {
      tmp_result.append(capMovement(data.mid(0, 6)));
    }

    ros_result.append(QDateTime::currentMSecsSinceEpoch() / 1000.0);
    for(int i = 0; i < tmp_result.size(); i++)
      ros_result.append(tmp_result[i]);

    QByteArray buffer;
    for(int i = 0; i < ros_result.count(); i++)
      appendValue<double>(&buffer, ros_result[i]);

    ros_socket->writeDatagram(buffer.data(), buffer.size(), QHostAddress("192.168.1.9"), 25444); // Julius Util
  }
  else if(outputmode == 0x02 && ur5dumpfile) // File
  {
    QList<float> result = data.mid(0,6);
    ur5dumpfile->write(QString(QString::number(QDateTime::currentMSecsSinceEpoch()) + "," + vectorToString(&result, 0, 6) + "\n").toLatin1());
    ur5dumpfile->flush();
  }
}

void iModUR5::connectRobot()
{
  if(robot_socket->state() == QTcpSocket::ConnectedState)
  {
    robot_socket->close();
    robot_socket->waitForDisconnected();
    qDebug() << "Robot disconnected";
  }

  robot_socket->connectToHost(robot_ip, robot_port);
  if(robot_socket->waitForConnected())
  {
    slotStateRequest(true);
    qDebug() << "UR5 Connected";
  }
  else
  {
    slotStateRequest(false);
    qDebug() << "Cant connect UR5 via [IP Port]:" << robot_ip << robot_port;
    robot_ip = "";
    robot_port = -1;
  }

}

QByteArray iModUR5::reverseArray(QByteArray& seq)
{
  QByteArray reverse;
  for( QByteArray::const_iterator i = seq.constEnd(); i !=seq.constBegin(); )
  {
    --i;
    reverse += *i;
  }
  return reverse;
}

void iModUR5::readCurrentAngle(char* data, int offset)
{
  for(int i = 0; i < 6; i++)
  {
    QByteArray tmp = QByteArray(data + offset, sizeof(double));
    offset += sizeof(double);
    tmp = reverseArray(tmp);
    memcpy(&lastangle[i], tmp.data(), sizeof(double));
  }
}

void iModUR5::readCurrentTCP(char* data, int offset)
{
  QMutexLocker ml(&datamutex);
  for(int i = 0; i < 3; i++)
  {
    QByteArray tmp = QByteArray(data + offset, sizeof(double));
    offset += sizeof(double);
    tmp = reverseArray(tmp);
    memcpy(&lasttcp[i], tmp.data(), sizeof(double));
  }
}

QList<float> iModUR5::capMovement(QList<float> blender)
{
  QList<float> result;
  for(int i = 0; i < 6; i++)
    result.append(0);

  for(int i = 0; i < 6; i++)
  {
    double diff = qAbs(blender[i] - lastangle[i]);

    while(diff > M_PI)
    {
      blender[i] += blender[i]>0.0?(-M_PI * 2.0):(M_PI * 2.0);
      diff = qAbs(blender[i] - lastangle[i]);
    }

    double mdif = (10.0 / 180.0) * M_PI;

    if(diff > mdif)
      result[i] = blender[i]>lastangle[i]?lastangle[i]+mdif:lastangle[i]-mdif;
    else
      result[i] = blender[i];
  }

  return result;
}

qint32 ArrayToInt(QByteArray source)
{
  qint32 temp;
  QDataStream data(&source, QIODevice::ReadWrite);
  data >> temp;
  return temp;
}
