#include "imodgripper.h"

#include <QtNetwork/QTcpSocket>
#include <QtCore/QTimer>

iModGripper::iModGripper(QObject *parent) :
  QObject(parent),
  transactionidentifier(0),
  controlltype(0)
{
  resetLocalOutputRegister();
  resetLocalInputRegister();

  grippersocket = new QTcpSocket(this);
  connect(grippersocket, &QTcpSocket::readyRead, this, &iModGripper::slotGripperStatusIncomming);
  connect(grippersocket, &QTcpSocket::disconnected, this, &iModGripper::slotGripperDisconnected);

  updatetimer = new QTimer();
  connect(updatetimer, &QTimer::timeout, this, &iModGripper::slotUpdateTimer);
  dbg_counter = 0;

  //dbg_timer = new QTimer();
  //connect(dbg_timer, &QTimer::timeout, this, &iModGripper::slotDBG_Timer);
  //dbg_timer->start(1000);
}

void iModGripper::resetLocalOutputRegister()
{
  memset(&output, 0, sizeof(RobotGripperOutput));
}

void iModGripper::setValuesOnGripper()
{
  QByteArray data = serializeGripperCommand();
  QByteArray package = getMBAPHeader(data.size() + 1); // + 1 for "bytes to follow" byte
  package.append(getRobotiqHeader(functioncode_set, 0x0000, 0x0008));
  package.append(data.size());
  package.append(data);
  writeToSocket(package);
}

void iModGripper::getAllValuesFromGripper()
{
  QByteArray package = getMBAPHeader(0);
  package.append(getRobotiqHeader(functioncode_get, 0x0000, 0x0008));
  writeToSocket(package);
}

bool iModGripper::slotConnectGripper(QString ip)
{
  qDebug() << "[Robotiq Gripper] Trying to connect";
  if(grippersocket && grippersocket->state() == QTcpSocket::ConnectedState)
  {
    grippersocket->close();
    grippersocket->waitForDisconnected();
  }
  grippersocket->connectToHost(ip, 502);
  bool success = grippersocket->waitForConnected(10000);

  if(!success)
  {
    qDebug() << "[Robotiq Gripper] Can't connect:" << grippersocket->errorString();
    return false;
  }

  qDebug() << "[Robotiq Gripper] Connected to Gripper";
  resetLocalOutputRegister();
  updatetimer->start(1000/10);
  setActivateGripper(true);
  setValuesOnGripper();
  return true;
}

void iModGripper::slotAutomaticGripperDriverMatlab(quint8 os, qreal pb, qreal cv)
{
  if(controlltype != 1)
    return;

  //qDebug() << "[Gripper] Os:" << os << "| Pb:" << pb << "| Cv:" << cv;
  Q_UNUSED(os)
  Q_UNUSED(pb)
  output.rPRA = (quint8)(cv * 255);
  setValuesOnGripper();

  // DEBUG
  /*
  if(cv == 0)
    emit this->signalGripperState(0x03);
  else
    emit this->signalGripperState(0x02);
  */
}

void iModGripper::slotAutomaticGripperDriverFTSensor(QVector3D force, QVector3D torque)
{
  if(controlltype != 2)
    return;

  Q_UNUSED(torque)
  if(force.z() < -15)
  {
    output.rPRA = (quint8) 255;
    setValuesOnGripper();
  }
  if(force.z() > 15)
  {
    output.rPRA = (quint8) 0;
    setValuesOnGripper();
  }
}

void iModGripper::slotGripperAutomatic(quint8 type)
{
  controlltype = type;
}

void iModGripper::disconnectGripper()
{
  grippersocket->close();
}

void iModGripper::slotSetGripperStatus(QByteArray status)
{
  memcpy(&output, status.data(), sizeof(RobotGripperOutput));
  setValuesOnGripper();
}

void iModGripper::slotGripperStatusIncomming()
{
  while(grippersocket->bytesAvailable())
  {
    dbg_counter++;
    quint16 tid = readType<quint16>();  // Transactionid
    quint16 pid = readType<quint16>();  // Protocollid
    quint16 length = readType<quint16>(); // Length
    quint8 uid = readType<quint8>();  // Clientid
    quint8 fid = readType<quint8>();  // Functionid

    Q_UNUSED(tid);
    Q_UNUSED(pid);
    Q_UNUSED(uid);

    if(fid == functioncode_get)
    {
      quint8 datalength = readType<quint8>(); // Datalength
      QByteArray data = readFromSocket(datalength); // Actual data
      deserializeGripperStatus(data);
    }
    else if(fid == functioncode_set)
    {
      quint16 first_reg = readType<quint16>(); // First register number
      quint16 num_reg = readType<quint16>(); // Number of registers

      Q_UNUSED(first_reg);
      Q_UNUSED(num_reg);
    }
    else if(fid == 132) // Error
    {
      // Find out more about this functioncode... it offers a kind of id
      QByteArray data = readFromSocket(length - 2);
    }
    else
    {
      qDebug() << "[Robotiq Gripper] Recived unknown function ID" << fid << " and dumping remaining data.";
      readFromSocket(length - 2);
    }
  }
}

void iModGripper::slotUpdateTimer()
{
  getAllValuesFromGripper();
}

void iModGripper::slotGripperDisconnected()
{
  qDebug() << "[Robotiq Gripper] Gripper disconnected";
}

void iModGripper::slotDBG_Timer()
{
  //qDebug() << dbg_counter << "FPS";
  //dbg_counter = 0;
}

QByteArray iModGripper::serializeGripperCommand()
{
  QByteArray data;
  data.resize(16);
  memset(data.data(), 0, sizeof(data[0]));

  // Pack the Action Request register byte
  data[0] = (output.rACT & 0x1) | (output.rMOD << 0x1) & 0x6 | ((output.rGTO << 0x3) & 0x8) | ((output.rATR << 0x4) & 0x10);

  // Pack the Gripper Options register byte
  data[1] =  ((output.rICF << 0x2) & 0x4) | ((output.rICS << 0x3) & 0x8);

  // map[2] is empty

  // Requested Position, Speed and Force (Finger A).
  data[3]  = output.rPRA;
  data[4]  = output.rSPA;
  data[5]  = output.rFRA;

  // Finger B
  data[6]  = output.rPRB;
  data[7]  = output.rSPB;
  data[8]  = output.rFRB;

  // Finger C
  data[9]  = output.rPRC;
  data[10] = output.rSPC;
  data[11] = output.rFRC;

  // Scissor Mode
  data[12] = output.rPRS;
  data[13] = output.rSPS;
  data[14] = output.rFRS;

  data[15] = 0x00; // padding to get 8 words

  return data;
}

void iModGripper::deserializeGripperStatus(QByteArray data)
{
  // Gripper Status
  input.gACT = data[0] & 0x1;
  input.gMOD = (data[0] >> 0x1) & 0x3;
  input.gGTO = (data[0] >> 0x3) & 0x1;
  input.gIMC = (data[0] >> 0x4) & 0x3;
  input.gSTA = (data[0] >> 0x6) & 0x3;

  // Object Status
  input.gDTA = data[1] & 0x3;
  input.gDTB = (data[1] >> 0x2) & 0x3;
  input.gDTC = (data[1] >> 0x4) & 0x3;
  input.gDTS = (data[1] >> 0x6) & 0x3;

  // Fault Status
  input.gFLT = data[2] & 0xF;

  // Requested Position, Speed and Force (Finger A).
  input.gPRA = data[3];
  input.gPOA = data[4];
  input.gCUA = data[5];

  // Finger B
  input.gPRB = data[6];
  input.gPOB = data[7];
  input.gCUB = data[8];

  // Finger C
  input.gPRC = data[9];
  input.gPOC = data[10];
  input.gCUC = data[11];

  // Scissor Mode
  input.gPRS = data[12];
  input.gPOS = data[13];
  input.gCUS = data[14];

  QByteArray status;
  status.resize(sizeof(RobotGripperInput));
  memcpy(status.data(), &input, sizeof(RobotGripperInput));
  emit this->signalGripperStatus(status);
  emit this->signalGripperState(input.gDTA);
}

void iModGripper::resetLocalInputRegister()
{
  memset(&input, 0, sizeof(RobotGripperInput));
}

QByteArray iModGripper::getMBAPHeader(quint16 size)
{
  size += 1 + 5; // Gripper data (size) + clientidentifier (1) + Robotiq header (5)
  swap_endian<quint16>(size);

  quint16 tid = transactionidentifier;
  swap_endian<quint16>(tid);

  QByteArray header;
  header.resize(7);
  memcpy(header.data(), &tid, sizeof(quint16));
  memset(header.data() + 2, 0, sizeof(quint16));
  memcpy(header.data() + 4, &size, sizeof(quint16));
  memcpy(header.data() + 6, &clientidentifier, sizeof(quint8));

  transactionidentifier++;
  if(transactionidentifier >= 0xFFFF)
    transactionidentifier = 0;
  return header;
}

QByteArray iModGripper::getRobotiqHeader(quint8 fcode, quint16 first_reg, quint16 wordcount)
{
  swap_endian<quint8>(fcode);
  swap_endian<quint16>(first_reg);
  swap_endian<quint16>(wordcount);

  QByteArray header;
  header.resize(5);
  memcpy(header.data() + 0, &fcode, sizeof(quint8));
  memcpy(header.data() + 1, &first_reg, sizeof(quint16));
  memcpy(header.data() + 3, &wordcount, sizeof(quint16));
  return header;
}

QByteArray iModGripper::readFromSocket(qint32 size)
{
  QByteArray data;
  if(!grippersocket || grippersocket->state() != QTcpSocket::ConnectedState)
    return data;

  data.resize(size);
  qint32 lasti;

  for(qint32 i = 0; i < size;)
  {
    lasti = i;
    i += this->grippersocket->read(data.data() + i, size - i);
    if(lasti == i)
      this->grippersocket->waitForReadyRead();
  }

  return data;
}

void iModGripper::writeToSocket(QByteArray data)
{
  if(!grippersocket || grippersocket->state() != QTcpSocket::ConnectedState)
    return;
  for(qint32 i = 0; i < (qint32)data.size();)
    i += this->grippersocket->write(data.data() + i, data.size() - i);
}
