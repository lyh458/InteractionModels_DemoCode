#include "iMod.protocol.h"

#include <QtNetwork/QTcpSocket>
#include <QtCore/QMetaProperty>
#include <QtCore/QDataStream>
#include <QtCore/QDataStream>
#include <QtCore/QTimer>
#include <QtCore/QEventLoop>

using namespace iModNetwork;

NetworkBase::NetworkBase(QObject* parent) :
  QObject(parent),
  socket(NULL)
{

}

bool NetworkBase::initialize(QTcpSocket* _socket)
{
  this->socket = _socket;
  connect(this->socket, &QTcpSocket::readyRead, this, &NetworkBase::slotReciveNetworkData);
  connect(this->socket, &QTcpSocket::disconnected, this, &NetworkBase::slotSocketDisconnected);
  return true; // init success
}

bool NetworkBase::sendPacketSynchron(PacketBase* packet)
{
  if(synchron_request.in_progress)
    return false;
  QTimer* countdown = new QTimer(this);
  QEventLoop* waitloop = new QEventLoop(this);
  connect(countdown, &QTimer::timeout, waitloop, &QEventLoop::quit);
  connect(this, &NetworkBase::signalSynchronAnswerRecived, waitloop, &QEventLoop::quit);
  synchron_request.in_progress = true;
  synchron_request.packet = packet;

  countdown->start(3000);
  
  char flags = 0x00;
  setBit(flags, 0); // Set first bit to 1 -> request
  setBit(flags, 1); // Set second bit to 1 -> syncron
  QByteArray buffer = serializeClass<PacketBase>(packet);
  bool returnvalue = sendData(packet->opcode(), flags, buffer);
  if(returnvalue)
    waitloop->exec();

  countdown->stop();
  delete waitloop;
  delete countdown;
  if(synchron_request.in_progress) // The timer killed it
  {
    synchron_request.in_progress = false;
    synchron_request.packet = NULL;
    return false;
  }
  return returnvalue;
}

bool NetworkBase::sendPacketAsynchron(PacketBase* packet)
{
  char flags = 0x00;
  setBit(flags, 0); // Set first bit to 1 -> request
  clearBit(flags, 1); // Set second bit to 0 -> asyncron
  QByteArray buffer = serializeClass<PacketBase>(packet);
  return sendData(packet->opcode(), flags, buffer);
}

bool NetworkBase::respondPacketSynchron(PacketBase* packet)
{
  char flags = 0x00;
  clearBit(flags, 0); // Set first bit to 0 -> response
  setBit(flags, 1); // Set second bit to 1 -> syncron
  QByteArray buffer = serializeClass<PacketBase>(packet);
  return sendData(packet->opcode(), flags, buffer);
}

bool NetworkBase::respondPacketAsynchron(PacketBase* packet)
{
  char flags = 0x00;
  clearBit(flags, 0); // Set first bit to 0 -> response
  clearBit(flags, 1); // Set second bit to 0 -> asyncron
  QByteArray buffer = serializeClass<PacketBase>(packet);
  return sendData(packet->opcode(), flags, buffer);
}

bool NetworkBase::sendPacketNotification(PacketBase* packet)
{
  return respondPacketAsynchron(packet);
}

void NetworkBase::slotReciveNetworkData()
{
  while(this->socket->bytesAvailable())
  {
    char opcode = readFromSocket(1)[0];
    char flags = readFromSocket(1)[0];
    qint32 size = readType<qint32>();
    QByteArray data = readFromSocket(size);

    switch(opcode)
    {
      case 1:
      {
        bool deletelater = false;
        ARTControllValues* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<ARTControllValues*>(synchron_request.packet);
        else
        {
          pkg = new ARTControllValues();
          deletelater = true;
        }
        parseClass<ARTControllValues>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_ARTControllValues(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_ARTControllValues(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_ARTControllValues(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_ARTControllValues(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 2:
      {
        bool deletelater = false;
        BlenderCommunication* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<BlenderCommunication*>(synchron_request.packet);
        else
        {
          pkg = new BlenderCommunication();
          deletelater = true;
        }
        parseClass<BlenderCommunication>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_BlenderCommunication(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_BlenderCommunication(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_BlenderCommunication(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_BlenderCommunication(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 3:
      {
        bool deletelater = false;
        BlenderRobotFPS* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<BlenderRobotFPS*>(synchron_request.packet);
        else
        {
          pkg = new BlenderRobotFPS();
          deletelater = true;
        }
        parseClass<BlenderRobotFPS>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_BlenderRobotFPS(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_BlenderRobotFPS(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_BlenderRobotFPS(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_BlenderRobotFPS(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 4:
      {
        bool deletelater = false;
        DaemonControll* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<DaemonControll*>(synchron_request.packet);
        else
        {
          pkg = new DaemonControll();
          deletelater = true;
        }
        parseClass<DaemonControll>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_DaemonControll(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_DaemonControll(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_DaemonControll(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_DaemonControll(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 5:
      {
        bool deletelater = false;
        DaemonState* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<DaemonState*>(synchron_request.packet);
        else
        {
          pkg = new DaemonState();
          deletelater = true;
        }
        parseClass<DaemonState>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_DaemonState(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_DaemonState(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_DaemonState(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_DaemonState(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 6:
      {
        bool deletelater = false;
        DatabaseControllValues* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<DatabaseControllValues*>(synchron_request.packet);
        else
        {
          pkg = new DatabaseControllValues();
          deletelater = true;
        }
        parseClass<DatabaseControllValues>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_DatabaseControllValues(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_DatabaseControllValues(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_DatabaseControllValues(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_DatabaseControllValues(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 7:
      {
        bool deletelater = false;
        DatabaseRecordingControll* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<DatabaseRecordingControll*>(synchron_request.packet);
        else
        {
          pkg = new DatabaseRecordingControll();
          deletelater = true;
        }
        parseClass<DatabaseRecordingControll>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_DatabaseRecordingControll(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_DatabaseRecordingControll(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_DatabaseRecordingControll(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_DatabaseRecordingControll(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 8:
      {
        bool deletelater = false;
        FTSensorValues* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<FTSensorValues*>(synchron_request.packet);
        else
        {
          pkg = new FTSensorValues();
          deletelater = true;
        }
        parseClass<FTSensorValues>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_FTSensorValues(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_FTSensorValues(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_FTSensorValues(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_FTSensorValues(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 9:
      {
        bool deletelater = false;
        GripperAutomatic* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<GripperAutomatic*>(synchron_request.packet);
        else
        {
          pkg = new GripperAutomatic();
          deletelater = true;
        }
        parseClass<GripperAutomatic>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_GripperAutomatic(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_GripperAutomatic(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_GripperAutomatic(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_GripperAutomatic(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 10:
      {
        bool deletelater = false;
        GripperControlValues* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<GripperControlValues*>(synchron_request.packet);
        else
        {
          pkg = new GripperControlValues();
          deletelater = true;
        }
        parseClass<GripperControlValues>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_GripperControlValues(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_GripperControlValues(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_GripperControlValues(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_GripperControlValues(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 11:
      {
        bool deletelater = false;
        InputMethode* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<InputMethode*>(synchron_request.packet);
        else
        {
          pkg = new InputMethode();
          deletelater = true;
        }
        parseClass<InputMethode>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_InputMethode(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_InputMethode(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_InputMethode(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_InputMethode(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 12:
      {
        bool deletelater = false;
        KinectControllValues* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<KinectControllValues*>(synchron_request.packet);
        else
        {
          pkg = new KinectControllValues();
          deletelater = true;
        }
        parseClass<KinectControllValues>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_KinectControllValues(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_KinectControllValues(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_KinectControllValues(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_KinectControllValues(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 13:
      {
        bool deletelater = false;
        KinectObjectTracking* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<KinectObjectTracking*>(synchron_request.packet);
        else
        {
          pkg = new KinectObjectTracking();
          deletelater = true;
        }
        parseClass<KinectObjectTracking>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_KinectObjectTracking(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_KinectObjectTracking(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_KinectObjectTracking(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_KinectObjectTracking(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 14:
      {
        bool deletelater = false;
        KinectOpenCV* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<KinectOpenCV*>(synchron_request.packet);
        else
        {
          pkg = new KinectOpenCV();
          deletelater = true;
        }
        parseClass<KinectOpenCV>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_KinectOpenCV(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_KinectOpenCV(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_KinectOpenCV(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_KinectOpenCV(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 15:
      {
        bool deletelater = false;
        KinectRange* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<KinectRange*>(synchron_request.packet);
        else
        {
          pkg = new KinectRange();
          deletelater = true;
        }
        parseClass<KinectRange>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_KinectRange(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_KinectRange(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_KinectRange(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_KinectRange(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 16:
      {
        bool deletelater = false;
        KinectTrackingData* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<KinectTrackingData*>(synchron_request.packet);
        else
        {
          pkg = new KinectTrackingData();
          deletelater = true;
        }
        parseClass<KinectTrackingData>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_KinectTrackingData(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_KinectTrackingData(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_KinectTrackingData(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_KinectTrackingData(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 17:
      {
        bool deletelater = false;
        ReadRemoteSetting* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<ReadRemoteSetting*>(synchron_request.packet);
        else
        {
          pkg = new ReadRemoteSetting();
          deletelater = true;
        }
        parseClass<ReadRemoteSetting>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_ReadRemoteSetting(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_ReadRemoteSetting(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_ReadRemoteSetting(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_ReadRemoteSetting(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 18:
      {
        bool deletelater = false;
        RobotControllValues* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<RobotControllValues*>(synchron_request.packet);
        else
        {
          pkg = new RobotControllValues();
          deletelater = true;
        }
        parseClass<RobotControllValues>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_RobotControllValues(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_RobotControllValues(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_RobotControllValues(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_RobotControllValues(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 19:
      {
        bool deletelater = false;
        TriadicObjectData* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<TriadicObjectData*>(synchron_request.packet);
        else
        {
          pkg = new TriadicObjectData();
          deletelater = true;
        }
        parseClass<TriadicObjectData>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_TriadicObjectData(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_TriadicObjectData(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_TriadicObjectData(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_TriadicObjectData(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 20:
      {
        bool deletelater = false;
        UR5Connection* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<UR5Connection*>(synchron_request.packet);
        else
        {
          pkg = new UR5Connection();
          deletelater = true;
        }
        parseClass<UR5Connection>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_UR5Connection(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_UR5Connection(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_UR5Connection(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_UR5Connection(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }
      case 21:
      {
        bool deletelater = false;
        UR5Output* pkg;
        if(!checkBit(flags, 0) &&
           checkBit(flags, 1)  && 
           synchron_request.packet &&
           synchron_request.packet->opcode() == opcode) // response and synchron
          pkg = dynamic_cast<UR5Output*>(synchron_request.packet);
        else
        {
          pkg = new UR5Output();
          deletelater = true;
        }
        parseClass<UR5Output>(pkg, data);
        if(checkBit(flags, 0) && checkBit(flags, 1)) // request and synchron
        {
          recvSyn_UR5Output(pkg, true);
          respondPacketSynchron(pkg);
        }
        else if(!checkBit(flags, 0) && checkBit(flags, 1)) // response and synchron
        recvSyn_UR5Output(pkg, false);
        else if(checkBit(flags, 0) && !checkBit(flags, 1)) // request and asynchron
        {
          recvAyn_UR5Output(pkg, true);
          respondPacketAsynchron(pkg);
        }
        else // response and asynchron
          recvAyn_UR5Output(pkg, false);  
        if(deletelater) delete pkg; 
        break;
      }

    }

    if(synchron_request.in_progress && synchron_request.packet->opcode() == opcode) // recheck with a list of syncron requests...
    {
      emit this->signalSynchronAnswerRecived();
      synchron_request.in_progress = false;
      synchron_request.packet = NULL;
    }
  }
}

void NetworkBase::slotSocketDisconnected()
{
  this->socket = NULL;
}

QByteArray NetworkBase::readFromSocket(qint32 size)
{
  QByteArray data;
  if(!socket)
    return data;

  data.resize(size);
  qint32 lasti;
  
  for(qint32 i = 0; i < size;)
  {
    lasti = i;
    i += this->socket->read(data.data() + i, size - i);
    if(lasti == i)
      this->socket->waitForReadyRead();
  }

  return data;
}

void NetworkBase::writeToSocket(QByteArray data)
{
  if(!socket)
    return;
  for(qint32 i = 0; i < (qint32)data.size();)
    i += this->socket->write(data.data() + i, data.size() - i);
}

bool NetworkBase::sendData(char opcode, char flags, QByteArray data)
{
  if(!socket)
    return false;
  if(socket->state() != QTcpSocket::ConnectedState)
    return false;
  writeToSocket(QByteArray(1, opcode));
  writeToSocket(QByteArray(1, flags));
  writeToSocket(typeToByteArray<qint32>((qint32)data.size()));
  writeToSocket(data);
  socket->flush();
  return true;
}

void NetworkBase::setBit(char& byte, int place)
{
  byte |= 1 << place;
}

void NetworkBase::clearBit(char& byte, int place)
{
  byte &= ~(1 << place);
}

void NetworkBase::toggleBit(char& byte, int place)
{
  byte ^= 1 << place;
}

bool NetworkBase::checkBit(char& byte, int place)
{
  return (byte >> place) & 1;
}
