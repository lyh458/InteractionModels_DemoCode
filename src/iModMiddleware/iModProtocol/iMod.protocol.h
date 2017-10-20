/**
* @file iMod.protocol.h
* @brief Automatically generated network class.
* @details This file is generated based on a configuration file containing the network protocoll description. Dont change anything withing this file. If you want to change the logic, please edit the templates for this class. To add a new network package or change datatypes, edit the configuration file and run the network generator.
*
*/

#ifndef iModNetwork_H
#define iModNetwork_H

#include <QObject>
#include <QtCore/QDataStream>
#include <QtCore/QVariant>
#include <QtGui/QMatrix4x4>

class QTcpSocket;
/**
 * @brief Generated networking
 * @details This namespace contains only generated code based on a configuration file containing the network protocoll description. Dont change anything withing this namespace. If you want to change the logic, please edit the templates for this class. To add a new network package or change datatypes, edit the configuration file and run the network generator.
 */
namespace iModNetwork
{
  class PacketBase;
  class ARTControllValues;
  class BlenderCommunication;
  class BlenderRobotFPS;
  class DaemonControll;
  class DaemonState;
  class DatabaseControllValues;
  class DatabaseRecordingControll;
  class FTSensorValues;
  class GripperAutomatic;
  class GripperControlValues;
  class InputMethode;
  class KinectControllValues;
  class KinectObjectTracking;
  class KinectOpenCV;
  class KinectRange;
  class KinectTrackingData;
  class ReadRemoteSetting;
  class RobotControllValues;
  class TriadicObjectData;
  class UR5Connection;
  class UR5Output;


  typedef struct _scs
  {
    _scs() 
    {
      in_progress = false;
      packet = NULL;
    }
    
    bool in_progress;
    PacketBase* packet;
  } SynchroneCallState;
  
  /**
   * @class NetworkBase
   * @brief Management class from dynamic network generator.
   * @details This class provides the functions required to send and recive packets. The class is intendet to be inherited from. For every packet definded in the configuration, a synchrone and an asynchrone function is generated and called by the internal packet parsing algorithm. To handle the logic of the single packages, inherite this class and reimplement the appropriate function. This class is generated based on a configuration file containing the network protocoll description. Dont change anything withing this file. If you want to change the logic, please edit the templates for this class. To add a new network package or change datatypes, edit the configuration file and run the network generator.
  */
  class NetworkBase : public QObject
  {
      Q_OBJECT
    public:
      /**
       * @brief Constructor of NetworkBase
       * @param parent Parent of this class.
       */
      explicit NetworkBase(QObject *parent = 0);

    protected:
      // Callable stuff
      /**
       * @brief Initializes the network.
       * @details Setting up the TCPSocket and connecting the required signals.
       * @param _socket The network socket.
       * @return Returns if the initialisation was successfull.
       */
      bool initialize(QTcpSocket* _socket);
      /**
       * @brief Sending synchrone packets to the other side.
       * @details Use this function for requests. This function wont return untill the other side returned the result. It will not block the current eventloop so every GUI will stay usable.
       * @param packet The packet that is sent to the other side.
       * @return Returns, if the request was successfull. Returns false, if the remote side encountered an error or the request timed out.
       */
      bool sendPacketSynchron(PacketBase* packet);
      /**
       * @brief Sending packets asynchrone.
       * @details Use this packet for requests. The function will return immediately and tirgger a signal, whenn the answer arrived from the remote.
       * @param packet The packet that is sent to the other side.
       * @return Returns, if the sending was successfull.
       */
      bool sendPacketAsynchron(PacketBase* packet);
      /**
       * @brief Sendig a notification to the remote side.
       * @details Use this packet to inform the remot side. The function returns immediately and you will get no response form the remote side.
       * @param packet The packet, that is sent to the other side.
       * @return Returns, if the sending was successfull.
       */
      bool sendPacketNotification(PacketBase* packet);

      // Virtual functions for syncron calls
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_ARTControllValues(ARTControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_ARTControllValues(ARTControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_BlenderCommunication(BlenderCommunication* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_BlenderCommunication(BlenderCommunication* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_BlenderRobotFPS(BlenderRobotFPS* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_BlenderRobotFPS(BlenderRobotFPS* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_DaemonControll(DaemonControll* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_DaemonControll(DaemonControll* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_DaemonState(DaemonState* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_DaemonState(DaemonState* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_DatabaseControllValues(DatabaseControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_DatabaseControllValues(DatabaseControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_DatabaseRecordingControll(DatabaseRecordingControll* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_DatabaseRecordingControll(DatabaseRecordingControll* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_FTSensorValues(FTSensorValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_FTSensorValues(FTSensorValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_GripperAutomatic(GripperAutomatic* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_GripperAutomatic(GripperAutomatic* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_GripperControlValues(GripperControlValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_GripperControlValues(GripperControlValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_InputMethode(InputMethode* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_InputMethode(InputMethode* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_KinectControllValues(KinectControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_KinectControllValues(KinectControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_KinectObjectTracking(KinectObjectTracking* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_KinectObjectTracking(KinectObjectTracking* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_KinectOpenCV(KinectOpenCV* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_KinectOpenCV(KinectOpenCV* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_KinectRange(KinectRange* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_KinectRange(KinectRange* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_KinectTrackingData(KinectTrackingData* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_KinectTrackingData(KinectTrackingData* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_ReadRemoteSetting(ReadRemoteSetting* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_ReadRemoteSetting(ReadRemoteSetting* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_RobotControllValues(RobotControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_RobotControllValues(RobotControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_TriadicObjectData(TriadicObjectData* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_TriadicObjectData(TriadicObjectData* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_UR5Connection(UR5Connection* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_UR5Connection(UR5Connection* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive synchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvSyn_UR5Output(UR5Output* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
      /**
       * @brief Virtual function to recive asynchrone packages
       * @param packet The network package
       * @param request Specifies, if it is a request or not
      */
      virtual void recvAyn_UR5Output(UR5Output* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }

    signals:
      /**
       * @brief Signal for synchrone request management.
       * @details This signal is emitted, when a synchrone response arrives from the remote side. It is ment to be used only internally.
       */
      void signalSynchronAnswerRecived();

    private slots:
      /**
       * @brief Reciving network data.
       * @details This slot is used to recive any network data comming at the TCPSocket.
       */
      void slotReciveNetworkData();
      /**
       * @brief Disconnection notice.
       * @details This slot is triggered, whenever the socket disconnects. The Socket will try to reconnect to the clients every 5 seconds using UDP Broadcast.
       */
      void slotSocketDisconnected();

    private: // Function Definitions
      /**
       * @brief Responding to a synchrone packet.
       * @details Seting the flags, triggering serialisation, sending packet.
       * @param packet The packet to be sent.
       * @return Returns, if the response was successfull.
       */
      bool respondPacketSynchron(PacketBase* packet);
      /**
       * @brief Responding to a asynchrone packet.
       * @details Seting the flags, triggering serialisation, sending packet.
       * @param packet The packet to be sent.
       * @return Returns, if the response was successfull.
       */
      bool respondPacketAsynchron(PacketBase* packet);
      /**
       * @brief Reads data from the socket.
       * @details This function reads exactly size bytes from the socket and blocks untill it has them. DO NOT use it to implement blocking sockets waiting for new packets... Qt is not ment for that!
       * @param size Size to read.
       * @return Returns the data from the socket.
       */
      QByteArray readFromSocket(qint32 size);
      /**
       * @brief This function writes to the socket.
       * @details The function makes sure, that all the data are realy written to the socket.
       * @param data Data to write.
       */
      void writeToSocket(QByteArray data);
      /**
       * @brief Building a network package.
       * @param opcode Operation code.
       * @param flags Flags containing the packet type.
       * @param data Packet data.
       * @return Returns, if the packet was sent successfull.
       */
      bool sendData(char opcode, char flags, QByteArray data);
      /**
       * @brief Set a single bit.
       * @param byte The byte to write in.
       * @param place The place to write (Number of the bit).
       */
      void setBit(char& byte, int place);
      /**
       * @brief Set a bit to zero.
       * @param byte The byte to change.
       * @param place The bitnumber to change.
       */
      void clearBit(char& byte, int place);
      /**
       * @brief Flipps a bit vom zero to one and vice versa.
       * @param byte The byte to change.
       * @param place The bitnumber to change.
       */
      void toggleBit(char& byte, int place);
      /**
       * @brief Checks, if a certain bit is set or not.
       * @param byte The byte to read in.
       * @param place The bitnumber to check.
       * @return Returns, if the byte is checked.
       */
      bool checkBit(char& byte, int place);

    private: // Template Definitions
      template<typename type>
      /**
       * @brief Reads a datatype type from the socket.
       * @details Reading exactly sizeof(type) from the socket and returning the type.
       * @return Returns the type.
       */
      type readType()
      {
        type value;
        QByteArray buffer = readFromSocket(sizeof(type));
        memcpy(&value, buffer.data(), sizeof(type));
        return value;
      }

      template<typename type>
      /**
       * @brief Reads a type from a bytearray
       * @param data Pointer to bytearray
       * @param offset Offset to add to bytearray
       * @return Returns the type
       */
      type parseType(char* data, qint32 offset = 0)
      {
        type value;
        memcpy(&value, data + offset, sizeof(type));
        return value;
      }

      template<typename type>
      /**
       * @brief Converts a type to a bytearray
       * @param value The value to write
       * @return Returns the bytearray
       */
      QByteArray typeToByteArray(type value)
      {
        QByteArray buffer;
        buffer.resize(sizeof(type));
        memcpy(buffer.data(), &value, sizeof(type));
        return buffer;
      }

      template<class tclass>
      /**
       * @brief Serializes a class
       * @details This function serializes a packet to a QByteArray using the QMetaObject-System
       * @param object The packet to serialize
       * @return Returns the serialized package as QByteArray
       */
      QByteArray serializeClass(tclass* object)
      {
        QByteArray buffer;
        QDataStream ds(&buffer, QIODevice::WriteOnly);
        for(int i=0; i<object->metaObject()->propertyCount(); ++i)
          if(object->metaObject()->property(i).isStored(object))
            ds << object->metaObject()->property(i).read(object);
        return buffer;
      }

      template<class tclass>
      /**
       * @brief Parses a class from a QByteArray
       * @details This function rebuilds the network package from a QByteArray using the QMetaObject-System.
       * @param object The object to build to
       * @param buffer The array to build from
       */
      void parseClass(tclass* object, QByteArray& buffer)
      {
        QVariant var;
        QDataStream ds(&buffer, QIODevice::ReadOnly);
        for(int i=0; i<object->metaObject()->propertyCount(); ++i)
          if(object->metaObject()->property(i).isStored(object))
          {
            ds >> var;
            object->metaObject()->property(i).write(object, var);
          }
      }

    private: // Variable Definitions
      /**
       * @brief Holding the network socket
       */
      QTcpSocket* socket;
      /**
       * @brief Holds, if a synchrone request is in progress. Only one synchrone request can be done at a time.
       */
      SynchroneCallState synchron_request;
      //bool synchron_request_in_progress;
      //PacketBase* current_synchrone;
  };


  /**
   * @class PacketBase
   * @brief Base class to inherite other packets from.
   * @details This class is generated. All network packeges need to inherite from this class. The serializer expects a pointer to this class.
   */
  class PacketBase : public QObject
  {
      Q_OBJECT
      Q_PROPERTY(char p_opcode READ opcode)
      Q_PROPERTY(bool p_success READ success WRITE set_success)

    public:
      /**
       * @brief Constructor of PacketBase
       * @param _oc The operationcode for the packet
       */
      explicit PacketBase(char _oc) : p_opcode(_oc), p_success(false) {}
      /**
       * @brief Returns the operationcode from the current packet
       * @return Operationcode
       */
      char opcode() { return p_opcode; }
      /**
       * @brief Returns, if the packet was processed successfully
       * @return
       */
      bool success() { return p_success; }
      /**
       * @brief Sets, if the processing was successfull
       * @param value Value
       */
      void set_success(bool value) { p_success = value; }

    protected:
      /**
       * @brief Holds the current opcode
       */
      const char p_opcode;
      /**
       * @brief Holds the current success state
       */
      bool p_success;
  };

  /**
   * @class ARTControllValues
   * @brief Generated class for network package ARTControllValues
   */
  class ARTControllValues : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(quint8 p_currenttargets READ currenttargets WRITE set_currenttargets)
      Q_PROPERTY(bool p_has_currenttargets READ has_currenttargets WRITE set_has_currenttargets)
      Q_PROPERTY(bool p_starttracking READ starttracking WRITE set_starttracking)
      Q_PROPERTY(bool p_has_starttracking READ has_starttracking WRITE set_has_starttracking)


    public:
      /**
       * @brief Constructor for class ARTControllValues
       */
      explicit ARTControllValues() :
        PacketBase(1),
        p_has_currenttargets(false),
        p_has_starttracking(false)
      {}

      /**
       * @brief Requesting the value currenttargets
       * @return Returns the value
      */
      quint8 currenttargets() { return p_currenttargets; }
      /**
       * @brief Set currenttargets in packet
      */
      void set_currenttargets(quint8 value) { p_currenttargets = value; }
      /**
       * @brief Checks if currenttargets is valid/set in this package
       * @return Returns true, if currenttargets is valid
       */
      bool has_currenttargets() { return p_has_currenttargets; }
      /**
       * @brief Sets if currenttargets is valid in this package
       * @param value Value
      */
      void set_has_currenttargets(bool value) { p_has_currenttargets = value; }
      /**
       * @brief Requesting the value starttracking
       * @return Returns the value
      */
      bool starttracking() { return p_starttracking; }
      /**
       * @brief Set starttracking in packet
      */
      void set_starttracking(bool value) { p_starttracking = value; }
      /**
       * @brief Checks if starttracking is valid/set in this package
       * @return Returns true, if starttracking is valid
       */
      bool has_starttracking() { return p_has_starttracking; }
      /**
       * @brief Sets if starttracking is valid in this package
       * @param value Value
      */
      void set_has_starttracking(bool value) { p_has_starttracking = value; }


    private:
      /**
       * @brief Holds the value of p_currenttargets
      */
      quint8 p_currenttargets;
      /**
       * @brief Holds if the value of p_currenttargets is valid in the package
      */
      bool p_has_currenttargets;
      /**
       * @brief Holds the value of p_starttracking
      */
      bool p_starttracking;
      /**
       * @brief Holds if the value of p_starttracking is valid in the package
      */
      bool p_has_starttracking;

  };
  /**
   * @class BlenderCommunication
   * @brief Generated class for network package BlenderCommunication
   */
  class BlenderCommunication : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(char p_gripperstate READ gripperstate WRITE set_gripperstate)
      Q_PROPERTY(bool p_has_gripperstate READ has_gripperstate WRITE set_has_gripperstate)
      Q_PROPERTY(char p_type READ type WRITE set_type)
      Q_PROPERTY(bool p_has_type READ has_type WRITE set_has_type)
      Q_PROPERTY(QByteArray p_ur5pose READ ur5pose WRITE set_ur5pose)
      Q_PROPERTY(bool p_has_ur5pose READ has_ur5pose WRITE set_has_ur5pose)
      Q_PROPERTY(QVector3D p_ur5tcp READ ur5tcp WRITE set_ur5tcp)
      Q_PROPERTY(bool p_has_ur5tcp READ has_ur5tcp WRITE set_has_ur5tcp)


    public:
      /**
       * @brief Constructor for class BlenderCommunication
       */
      explicit BlenderCommunication() :
        PacketBase(2),
        p_has_gripperstate(false),
        p_has_type(false),
        p_has_ur5pose(false),
        p_has_ur5tcp(false)
      {}

      /**
       * @brief Requesting the value gripperstate
       * @return Returns the value
      */
      char gripperstate() { return p_gripperstate; }
      /**
       * @brief Set gripperstate in packet
      */
      void set_gripperstate(char value) { p_gripperstate = value; }
      /**
       * @brief Checks if gripperstate is valid/set in this package
       * @return Returns true, if gripperstate is valid
       */
      bool has_gripperstate() { return p_has_gripperstate; }
      /**
       * @brief Sets if gripperstate is valid in this package
       * @param value Value
      */
      void set_has_gripperstate(bool value) { p_has_gripperstate = value; }
      /**
       * @brief Requesting the value type
       * @return Returns the value
      */
      char type() { return p_type; }
      /**
       * @brief Set type in packet
      */
      void set_type(char value) { p_type = value; }
      /**
       * @brief Checks if type is valid/set in this package
       * @return Returns true, if type is valid
       */
      bool has_type() { return p_has_type; }
      /**
       * @brief Sets if type is valid in this package
       * @param value Value
      */
      void set_has_type(bool value) { p_has_type = value; }
      /**
       * @brief Requesting the value ur5pose
       * @return Returns the value
      */
      QByteArray ur5pose() { return p_ur5pose; }
      /**
       * @brief Set ur5pose in packet
      */
      void set_ur5pose(QByteArray value) { p_ur5pose = value; }
      /**
       * @brief Checks if ur5pose is valid/set in this package
       * @return Returns true, if ur5pose is valid
       */
      bool has_ur5pose() { return p_has_ur5pose; }
      /**
       * @brief Sets if ur5pose is valid in this package
       * @param value Value
      */
      void set_has_ur5pose(bool value) { p_has_ur5pose = value; }
      /**
       * @brief Requesting the value ur5tcp
       * @return Returns the value
      */
      QVector3D ur5tcp() { return p_ur5tcp; }
      /**
       * @brief Set ur5tcp in packet
      */
      void set_ur5tcp(QVector3D value) { p_ur5tcp = value; }
      /**
       * @brief Checks if ur5tcp is valid/set in this package
       * @return Returns true, if ur5tcp is valid
       */
      bool has_ur5tcp() { return p_has_ur5tcp; }
      /**
       * @brief Sets if ur5tcp is valid in this package
       * @param value Value
      */
      void set_has_ur5tcp(bool value) { p_has_ur5tcp = value; }


    private:
      /**
       * @brief Holds the value of p_gripperstate
      */
      char p_gripperstate;
      /**
       * @brief Holds if the value of p_gripperstate is valid in the package
      */
      bool p_has_gripperstate;
      /**
       * @brief Holds the value of p_type
      */
      char p_type;
      /**
       * @brief Holds if the value of p_type is valid in the package
      */
      bool p_has_type;
      /**
       * @brief Holds the value of p_ur5pose
      */
      QByteArray p_ur5pose;
      /**
       * @brief Holds if the value of p_ur5pose is valid in the package
      */
      bool p_has_ur5pose;
      /**
       * @brief Holds the value of p_ur5tcp
      */
      QVector3D p_ur5tcp;
      /**
       * @brief Holds if the value of p_ur5tcp is valid in the package
      */
      bool p_has_ur5tcp;

  };
  /**
   * @class BlenderRobotFPS
   * @brief Generated class for network package BlenderRobotFPS
   */
  class BlenderRobotFPS : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(quint32 p_blender_fps READ blender_fps WRITE set_blender_fps)
      Q_PROPERTY(bool p_has_blender_fps READ has_blender_fps WRITE set_has_blender_fps)
      Q_PROPERTY(quint32 p_robot_fps READ robot_fps WRITE set_robot_fps)
      Q_PROPERTY(bool p_has_robot_fps READ has_robot_fps WRITE set_has_robot_fps)


    public:
      /**
       * @brief Constructor for class BlenderRobotFPS
       */
      explicit BlenderRobotFPS() :
        PacketBase(3),
        p_has_blender_fps(false),
        p_has_robot_fps(false)
      {}

      /**
       * @brief Requesting the value blender_fps
       * @return Returns the value
      */
      quint32 blender_fps() { return p_blender_fps; }
      /**
       * @brief Set blender_fps in packet
      */
      void set_blender_fps(quint32 value) { p_blender_fps = value; }
      /**
       * @brief Checks if blender_fps is valid/set in this package
       * @return Returns true, if blender_fps is valid
       */
      bool has_blender_fps() { return p_has_blender_fps; }
      /**
       * @brief Sets if blender_fps is valid in this package
       * @param value Value
      */
      void set_has_blender_fps(bool value) { p_has_blender_fps = value; }
      /**
       * @brief Requesting the value robot_fps
       * @return Returns the value
      */
      quint32 robot_fps() { return p_robot_fps; }
      /**
       * @brief Set robot_fps in packet
      */
      void set_robot_fps(quint32 value) { p_robot_fps = value; }
      /**
       * @brief Checks if robot_fps is valid/set in this package
       * @return Returns true, if robot_fps is valid
       */
      bool has_robot_fps() { return p_has_robot_fps; }
      /**
       * @brief Sets if robot_fps is valid in this package
       * @param value Value
      */
      void set_has_robot_fps(bool value) { p_has_robot_fps = value; }


    private:
      /**
       * @brief Holds the value of p_blender_fps
      */
      quint32 p_blender_fps;
      /**
       * @brief Holds if the value of p_blender_fps is valid in the package
      */
      bool p_has_blender_fps;
      /**
       * @brief Holds the value of p_robot_fps
      */
      quint32 p_robot_fps;
      /**
       * @brief Holds if the value of p_robot_fps is valid in the package
      */
      bool p_has_robot_fps;

  };
  /**
   * @class DaemonControll
   * @brief Generated class for network package DaemonControll
   */
  class DaemonControll : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(bool p_forceroot READ forceroot WRITE set_forceroot)
      Q_PROPERTY(bool p_has_forceroot READ has_forceroot WRITE set_has_forceroot)
      Q_PROPERTY(bool p_reset READ reset WRITE set_reset)
      Q_PROPERTY(bool p_has_reset READ has_reset WRITE set_has_reset)
      Q_PROPERTY(quint8 p_rootclient READ rootclient WRITE set_rootclient)
      Q_PROPERTY(bool p_has_rootclient READ has_rootclient WRITE set_has_rootclient)
      Q_PROPERTY(quint8 p_rootkinect READ rootkinect WRITE set_rootkinect)
      Q_PROPERTY(bool p_has_rootkinect READ has_rootkinect WRITE set_has_rootkinect)
      Q_PROPERTY(quint8 p_setrootkinect READ setrootkinect WRITE set_setrootkinect)
      Q_PROPERTY(bool p_has_setrootkinect READ has_setrootkinect WRITE set_has_setrootkinect)


    public:
      /**
       * @brief Constructor for class DaemonControll
       */
      explicit DaemonControll() :
        PacketBase(4),
        p_has_forceroot(false),
        p_has_reset(false),
        p_has_rootclient(false),
        p_has_rootkinect(false),
        p_has_setrootkinect(false)
      {}

      /**
       * @brief Requesting the value forceroot
       * @return Returns the value
      */
      bool forceroot() { return p_forceroot; }
      /**
       * @brief Set forceroot in packet
      */
      void set_forceroot(bool value) { p_forceroot = value; }
      /**
       * @brief Checks if forceroot is valid/set in this package
       * @return Returns true, if forceroot is valid
       */
      bool has_forceroot() { return p_has_forceroot; }
      /**
       * @brief Sets if forceroot is valid in this package
       * @param value Value
      */
      void set_has_forceroot(bool value) { p_has_forceroot = value; }
      /**
       * @brief Requesting the value reset
       * @return Returns the value
      */
      bool reset() { return p_reset; }
      /**
       * @brief Set reset in packet
      */
      void set_reset(bool value) { p_reset = value; }
      /**
       * @brief Checks if reset is valid/set in this package
       * @return Returns true, if reset is valid
       */
      bool has_reset() { return p_has_reset; }
      /**
       * @brief Sets if reset is valid in this package
       * @param value Value
      */
      void set_has_reset(bool value) { p_has_reset = value; }
      /**
       * @brief Requesting the value rootclient
       * @return Returns the value
      */
      quint8 rootclient() { return p_rootclient; }
      /**
       * @brief Set rootclient in packet
      */
      void set_rootclient(quint8 value) { p_rootclient = value; }
      /**
       * @brief Checks if rootclient is valid/set in this package
       * @return Returns true, if rootclient is valid
       */
      bool has_rootclient() { return p_has_rootclient; }
      /**
       * @brief Sets if rootclient is valid in this package
       * @param value Value
      */
      void set_has_rootclient(bool value) { p_has_rootclient = value; }
      /**
       * @brief Requesting the value rootkinect
       * @return Returns the value
      */
      quint8 rootkinect() { return p_rootkinect; }
      /**
       * @brief Set rootkinect in packet
      */
      void set_rootkinect(quint8 value) { p_rootkinect = value; }
      /**
       * @brief Checks if rootkinect is valid/set in this package
       * @return Returns true, if rootkinect is valid
       */
      bool has_rootkinect() { return p_has_rootkinect; }
      /**
       * @brief Sets if rootkinect is valid in this package
       * @param value Value
      */
      void set_has_rootkinect(bool value) { p_has_rootkinect = value; }
      /**
       * @brief Requesting the value setrootkinect
       * @return Returns the value
      */
      quint8 setrootkinect() { return p_setrootkinect; }
      /**
       * @brief Set setrootkinect in packet
      */
      void set_setrootkinect(quint8 value) { p_setrootkinect = value; }
      /**
       * @brief Checks if setrootkinect is valid/set in this package
       * @return Returns true, if setrootkinect is valid
       */
      bool has_setrootkinect() { return p_has_setrootkinect; }
      /**
       * @brief Sets if setrootkinect is valid in this package
       * @param value Value
      */
      void set_has_setrootkinect(bool value) { p_has_setrootkinect = value; }


    private:
      /**
       * @brief Holds the value of p_forceroot
      */
      bool p_forceroot;
      /**
       * @brief Holds if the value of p_forceroot is valid in the package
      */
      bool p_has_forceroot;
      /**
       * @brief Holds the value of p_reset
      */
      bool p_reset;
      /**
       * @brief Holds if the value of p_reset is valid in the package
      */
      bool p_has_reset;
      /**
       * @brief Holds the value of p_rootclient
      */
      quint8 p_rootclient;
      /**
       * @brief Holds if the value of p_rootclient is valid in the package
      */
      bool p_has_rootclient;
      /**
       * @brief Holds the value of p_rootkinect
      */
      quint8 p_rootkinect;
      /**
       * @brief Holds if the value of p_rootkinect is valid in the package
      */
      bool p_has_rootkinect;
      /**
       * @brief Holds the value of p_setrootkinect
      */
      quint8 p_setrootkinect;
      /**
       * @brief Holds if the value of p_setrootkinect is valid in the package
      */
      bool p_has_setrootkinect;

  };
  /**
   * @class DaemonState
   * @brief Generated class for network package DaemonState
   */
  class DaemonState : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(QStringList p_currentclients READ currentclients WRITE set_currentclients)
      Q_PROPERTY(bool p_has_currentclients READ has_currentclients WRITE set_has_currentclients)


    public:
      /**
       * @brief Constructor for class DaemonState
       */
      explicit DaemonState() :
        PacketBase(5),
        p_has_currentclients(false)
      {}

      /**
       * @brief Requesting the value currentclients
       * @return Returns the value
      */
      QStringList currentclients() { return p_currentclients; }
      /**
       * @brief Set currentclients in packet
      */
      void set_currentclients(QStringList value) { p_currentclients = value; }
      /**
       * @brief Checks if currentclients is valid/set in this package
       * @return Returns true, if currentclients is valid
       */
      bool has_currentclients() { return p_has_currentclients; }
      /**
       * @brief Sets if currentclients is valid in this package
       * @param value Value
      */
      void set_has_currentclients(bool value) { p_has_currentclients = value; }


    private:
      /**
       * @brief Holds the value of p_currentclients
      */
      QStringList p_currentclients;
      /**
       * @brief Holds if the value of p_currentclients is valid in the package
      */
      bool p_has_currentclients;

  };
  /**
   * @class DatabaseControllValues
   * @brief Generated class for network package DatabaseControllValues
   */
  class DatabaseControllValues : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(QString p_category READ category WRITE set_category)
      Q_PROPERTY(bool p_has_category READ has_category WRITE set_has_category)
      Q_PROPERTY(QStringList p_categorylist READ categorylist WRITE set_categorylist)
      Q_PROPERTY(bool p_has_categorylist READ has_categorylist WRITE set_has_categorylist)
      Q_PROPERTY(bool p_load READ load WRITE set_load)
      Q_PROPERTY(bool p_has_load READ has_load WRITE set_has_load)
      Q_PROPERTY(QString p_loadfromcategory READ loadfromcategory WRITE set_loadfromcategory)
      Q_PROPERTY(bool p_has_loadfromcategory READ has_loadfromcategory WRITE set_has_loadfromcategory)
      Q_PROPERTY(QString p_table READ table WRITE set_table)
      Q_PROPERTY(bool p_has_table READ has_table WRITE set_has_table)
      Q_PROPERTY(QStringList p_tablelist READ tablelist WRITE set_tablelist)
      Q_PROPERTY(bool p_has_tablelist READ has_tablelist WRITE set_has_tablelist)


    public:
      /**
       * @brief Constructor for class DatabaseControllValues
       */
      explicit DatabaseControllValues() :
        PacketBase(6),
        p_has_category(false),
        p_has_categorylist(false),
        p_has_load(false),
        p_has_loadfromcategory(false),
        p_has_table(false),
        p_has_tablelist(false)
      {}

      /**
       * @brief Requesting the value category
       * @return Returns the value
      */
      QString category() { return p_category; }
      /**
       * @brief Set category in packet
      */
      void set_category(QString value) { p_category = value; }
      /**
       * @brief Checks if category is valid/set in this package
       * @return Returns true, if category is valid
       */
      bool has_category() { return p_has_category; }
      /**
       * @brief Sets if category is valid in this package
       * @param value Value
      */
      void set_has_category(bool value) { p_has_category = value; }
      /**
       * @brief Requesting the value categorylist
       * @return Returns the value
      */
      QStringList categorylist() { return p_categorylist; }
      /**
       * @brief Set categorylist in packet
      */
      void set_categorylist(QStringList value) { p_categorylist = value; }
      /**
       * @brief Checks if categorylist is valid/set in this package
       * @return Returns true, if categorylist is valid
       */
      bool has_categorylist() { return p_has_categorylist; }
      /**
       * @brief Sets if categorylist is valid in this package
       * @param value Value
      */
      void set_has_categorylist(bool value) { p_has_categorylist = value; }
      /**
       * @brief Requesting the value load
       * @return Returns the value
      */
      bool load() { return p_load; }
      /**
       * @brief Set load in packet
      */
      void set_load(bool value) { p_load = value; }
      /**
       * @brief Checks if load is valid/set in this package
       * @return Returns true, if load is valid
       */
      bool has_load() { return p_has_load; }
      /**
       * @brief Sets if load is valid in this package
       * @param value Value
      */
      void set_has_load(bool value) { p_has_load = value; }
      /**
       * @brief Requesting the value loadfromcategory
       * @return Returns the value
      */
      QString loadfromcategory() { return p_loadfromcategory; }
      /**
       * @brief Set loadfromcategory in packet
      */
      void set_loadfromcategory(QString value) { p_loadfromcategory = value; }
      /**
       * @brief Checks if loadfromcategory is valid/set in this package
       * @return Returns true, if loadfromcategory is valid
       */
      bool has_loadfromcategory() { return p_has_loadfromcategory; }
      /**
       * @brief Sets if loadfromcategory is valid in this package
       * @param value Value
      */
      void set_has_loadfromcategory(bool value) { p_has_loadfromcategory = value; }
      /**
       * @brief Requesting the value table
       * @return Returns the value
      */
      QString table() { return p_table; }
      /**
       * @brief Set table in packet
      */
      void set_table(QString value) { p_table = value; }
      /**
       * @brief Checks if table is valid/set in this package
       * @return Returns true, if table is valid
       */
      bool has_table() { return p_has_table; }
      /**
       * @brief Sets if table is valid in this package
       * @param value Value
      */
      void set_has_table(bool value) { p_has_table = value; }
      /**
       * @brief Requesting the value tablelist
       * @return Returns the value
      */
      QStringList tablelist() { return p_tablelist; }
      /**
       * @brief Set tablelist in packet
      */
      void set_tablelist(QStringList value) { p_tablelist = value; }
      /**
       * @brief Checks if tablelist is valid/set in this package
       * @return Returns true, if tablelist is valid
       */
      bool has_tablelist() { return p_has_tablelist; }
      /**
       * @brief Sets if tablelist is valid in this package
       * @param value Value
      */
      void set_has_tablelist(bool value) { p_has_tablelist = value; }


    private:
      /**
       * @brief Holds the value of p_category
      */
      QString p_category;
      /**
       * @brief Holds if the value of p_category is valid in the package
      */
      bool p_has_category;
      /**
       * @brief Holds the value of p_categorylist
      */
      QStringList p_categorylist;
      /**
       * @brief Holds if the value of p_categorylist is valid in the package
      */
      bool p_has_categorylist;
      /**
       * @brief Holds the value of p_load
      */
      bool p_load;
      /**
       * @brief Holds if the value of p_load is valid in the package
      */
      bool p_has_load;
      /**
       * @brief Holds the value of p_loadfromcategory
      */
      QString p_loadfromcategory;
      /**
       * @brief Holds if the value of p_loadfromcategory is valid in the package
      */
      bool p_has_loadfromcategory;
      /**
       * @brief Holds the value of p_table
      */
      QString p_table;
      /**
       * @brief Holds if the value of p_table is valid in the package
      */
      bool p_has_table;
      /**
       * @brief Holds the value of p_tablelist
      */
      QStringList p_tablelist;
      /**
       * @brief Holds if the value of p_tablelist is valid in the package
      */
      bool p_has_tablelist;

  };
  /**
   * @class DatabaseRecordingControll
   * @brief Generated class for network package DatabaseRecordingControll
   */
  class DatabaseRecordingControll : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(bool p_recorde READ recorde WRITE set_recorde)
      Q_PROPERTY(bool p_has_recorde READ has_recorde WRITE set_has_recorde)
      Q_PROPERTY(quint8 p_replayfps READ replayfps WRITE set_replayfps)
      Q_PROPERTY(bool p_has_replayfps READ has_replayfps WRITE set_has_replayfps)
      Q_PROPERTY(QString p_writecategory READ writecategory WRITE set_writecategory)
      Q_PROPERTY(bool p_has_writecategory READ has_writecategory WRITE set_has_writecategory)
      Q_PROPERTY(QString p_writetable READ writetable WRITE set_writetable)
      Q_PROPERTY(bool p_has_writetable READ has_writetable WRITE set_has_writetable)


    public:
      /**
       * @brief Constructor for class DatabaseRecordingControll
       */
      explicit DatabaseRecordingControll() :
        PacketBase(7),
        p_has_recorde(false),
        p_has_replayfps(false),
        p_has_writecategory(false),
        p_has_writetable(false)
      {}

      /**
       * @brief Requesting the value recorde
       * @return Returns the value
      */
      bool recorde() { return p_recorde; }
      /**
       * @brief Set recorde in packet
      */
      void set_recorde(bool value) { p_recorde = value; }
      /**
       * @brief Checks if recorde is valid/set in this package
       * @return Returns true, if recorde is valid
       */
      bool has_recorde() { return p_has_recorde; }
      /**
       * @brief Sets if recorde is valid in this package
       * @param value Value
      */
      void set_has_recorde(bool value) { p_has_recorde = value; }
      /**
       * @brief Requesting the value replayfps
       * @return Returns the value
      */
      quint8 replayfps() { return p_replayfps; }
      /**
       * @brief Set replayfps in packet
      */
      void set_replayfps(quint8 value) { p_replayfps = value; }
      /**
       * @brief Checks if replayfps is valid/set in this package
       * @return Returns true, if replayfps is valid
       */
      bool has_replayfps() { return p_has_replayfps; }
      /**
       * @brief Sets if replayfps is valid in this package
       * @param value Value
      */
      void set_has_replayfps(bool value) { p_has_replayfps = value; }
      /**
       * @brief Requesting the value writecategory
       * @return Returns the value
      */
      QString writecategory() { return p_writecategory; }
      /**
       * @brief Set writecategory in packet
      */
      void set_writecategory(QString value) { p_writecategory = value; }
      /**
       * @brief Checks if writecategory is valid/set in this package
       * @return Returns true, if writecategory is valid
       */
      bool has_writecategory() { return p_has_writecategory; }
      /**
       * @brief Sets if writecategory is valid in this package
       * @param value Value
      */
      void set_has_writecategory(bool value) { p_has_writecategory = value; }
      /**
       * @brief Requesting the value writetable
       * @return Returns the value
      */
      QString writetable() { return p_writetable; }
      /**
       * @brief Set writetable in packet
      */
      void set_writetable(QString value) { p_writetable = value; }
      /**
       * @brief Checks if writetable is valid/set in this package
       * @return Returns true, if writetable is valid
       */
      bool has_writetable() { return p_has_writetable; }
      /**
       * @brief Sets if writetable is valid in this package
       * @param value Value
      */
      void set_has_writetable(bool value) { p_has_writetable = value; }


    private:
      /**
       * @brief Holds the value of p_recorde
      */
      bool p_recorde;
      /**
       * @brief Holds if the value of p_recorde is valid in the package
      */
      bool p_has_recorde;
      /**
       * @brief Holds the value of p_replayfps
      */
      quint8 p_replayfps;
      /**
       * @brief Holds if the value of p_replayfps is valid in the package
      */
      bool p_has_replayfps;
      /**
       * @brief Holds the value of p_writecategory
      */
      QString p_writecategory;
      /**
       * @brief Holds if the value of p_writecategory is valid in the package
      */
      bool p_has_writecategory;
      /**
       * @brief Holds the value of p_writetable
      */
      QString p_writetable;
      /**
       * @brief Holds if the value of p_writetable is valid in the package
      */
      bool p_has_writetable;

  };
  /**
   * @class FTSensorValues
   * @brief Generated class for network package FTSensorValues
   */
  class FTSensorValues : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(QVector3D p_force READ force WRITE set_force)
      Q_PROPERTY(bool p_has_force READ has_force WRITE set_has_force)
      Q_PROPERTY(QVector3D p_torque READ torque WRITE set_torque)
      Q_PROPERTY(bool p_has_torque READ has_torque WRITE set_has_torque)


    public:
      /**
       * @brief Constructor for class FTSensorValues
       */
      explicit FTSensorValues() :
        PacketBase(8),
        p_has_force(false),
        p_has_torque(false)
      {}

      /**
       * @brief Requesting the value force
       * @return Returns the value
      */
      QVector3D force() { return p_force; }
      /**
       * @brief Set force in packet
      */
      void set_force(QVector3D value) { p_force = value; }
      /**
       * @brief Checks if force is valid/set in this package
       * @return Returns true, if force is valid
       */
      bool has_force() { return p_has_force; }
      /**
       * @brief Sets if force is valid in this package
       * @param value Value
      */
      void set_has_force(bool value) { p_has_force = value; }
      /**
       * @brief Requesting the value torque
       * @return Returns the value
      */
      QVector3D torque() { return p_torque; }
      /**
       * @brief Set torque in packet
      */
      void set_torque(QVector3D value) { p_torque = value; }
      /**
       * @brief Checks if torque is valid/set in this package
       * @return Returns true, if torque is valid
       */
      bool has_torque() { return p_has_torque; }
      /**
       * @brief Sets if torque is valid in this package
       * @param value Value
      */
      void set_has_torque(bool value) { p_has_torque = value; }


    private:
      /**
       * @brief Holds the value of p_force
      */
      QVector3D p_force;
      /**
       * @brief Holds if the value of p_force is valid in the package
      */
      bool p_has_force;
      /**
       * @brief Holds the value of p_torque
      */
      QVector3D p_torque;
      /**
       * @brief Holds if the value of p_torque is valid in the package
      */
      bool p_has_torque;

  };
  /**
   * @class GripperAutomatic
   * @brief Generated class for network package GripperAutomatic
   */
  class GripperAutomatic : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(quint8 p_type READ type WRITE set_type)
      Q_PROPERTY(bool p_has_type READ has_type WRITE set_has_type)


    public:
      /**
       * @brief Constructor for class GripperAutomatic
       */
      explicit GripperAutomatic() :
        PacketBase(9),
        p_has_type(false)
      {}

      /**
       * @brief Requesting the value type
       * @return Returns the value
      */
      quint8 type() { return p_type; }
      /**
       * @brief Set type in packet
      */
      void set_type(quint8 value) { p_type = value; }
      /**
       * @brief Checks if type is valid/set in this package
       * @return Returns true, if type is valid
       */
      bool has_type() { return p_has_type; }
      /**
       * @brief Sets if type is valid in this package
       * @param value Value
      */
      void set_has_type(bool value) { p_has_type = value; }


    private:
      /**
       * @brief Holds the value of p_type
      */
      quint8 p_type;
      /**
       * @brief Holds if the value of p_type is valid in the package
      */
      bool p_has_type;

  };
  /**
   * @class GripperControlValues
   * @brief Generated class for network package GripperControlValues
   */
  class GripperControlValues : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(QByteArray p_data READ data WRITE set_data)
      Q_PROPERTY(bool p_has_data READ has_data WRITE set_has_data)
      Q_PROPERTY(QString p_ip READ ip WRITE set_ip)
      Q_PROPERTY(bool p_has_ip READ has_ip WRITE set_has_ip)
      Q_PROPERTY(bool p_setvalues READ setvalues WRITE set_setvalues)
      Q_PROPERTY(bool p_has_setvalues READ has_setvalues WRITE set_has_setvalues)


    public:
      /**
       * @brief Constructor for class GripperControlValues
       */
      explicit GripperControlValues() :
        PacketBase(10),
        p_has_data(false),
        p_has_ip(false),
        p_has_setvalues(false)
      {}

      /**
       * @brief Requesting the value data
       * @return Returns the value
      */
      QByteArray data() { return p_data; }
      /**
       * @brief Set data in packet
      */
      void set_data(QByteArray value) { p_data = value; }
      /**
       * @brief Checks if data is valid/set in this package
       * @return Returns true, if data is valid
       */
      bool has_data() { return p_has_data; }
      /**
       * @brief Sets if data is valid in this package
       * @param value Value
      */
      void set_has_data(bool value) { p_has_data = value; }
      /**
       * @brief Requesting the value ip
       * @return Returns the value
      */
      QString ip() { return p_ip; }
      /**
       * @brief Set ip in packet
      */
      void set_ip(QString value) { p_ip = value; }
      /**
       * @brief Checks if ip is valid/set in this package
       * @return Returns true, if ip is valid
       */
      bool has_ip() { return p_has_ip; }
      /**
       * @brief Sets if ip is valid in this package
       * @param value Value
      */
      void set_has_ip(bool value) { p_has_ip = value; }
      /**
       * @brief Requesting the value setvalues
       * @return Returns the value
      */
      bool setvalues() { return p_setvalues; }
      /**
       * @brief Set setvalues in packet
      */
      void set_setvalues(bool value) { p_setvalues = value; }
      /**
       * @brief Checks if setvalues is valid/set in this package
       * @return Returns true, if setvalues is valid
       */
      bool has_setvalues() { return p_has_setvalues; }
      /**
       * @brief Sets if setvalues is valid in this package
       * @param value Value
      */
      void set_has_setvalues(bool value) { p_has_setvalues = value; }


    private:
      /**
       * @brief Holds the value of p_data
      */
      QByteArray p_data;
      /**
       * @brief Holds if the value of p_data is valid in the package
      */
      bool p_has_data;
      /**
       * @brief Holds the value of p_ip
      */
      QString p_ip;
      /**
       * @brief Holds if the value of p_ip is valid in the package
      */
      bool p_has_ip;
      /**
       * @brief Holds the value of p_setvalues
      */
      bool p_setvalues;
      /**
       * @brief Holds if the value of p_setvalues is valid in the package
      */
      bool p_has_setvalues;

  };
  /**
   * @class InputMethode
   * @brief Generated class for network package InputMethode
   */
  class InputMethode : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(char p_methode READ methode WRITE set_methode)
      Q_PROPERTY(bool p_has_methode READ has_methode WRITE set_has_methode)


    public:
      /**
       * @brief Constructor for class InputMethode
       */
      explicit InputMethode() :
        PacketBase(11),
        p_has_methode(false)
      {}

      /**
       * @brief Requesting the value methode
       * @return Returns the value
      */
      char methode() { return p_methode; }
      /**
       * @brief Set methode in packet
      */
      void set_methode(char value) { p_methode = value; }
      /**
       * @brief Checks if methode is valid/set in this package
       * @return Returns true, if methode is valid
       */
      bool has_methode() { return p_has_methode; }
      /**
       * @brief Sets if methode is valid in this package
       * @param value Value
      */
      void set_has_methode(bool value) { p_has_methode = value; }


    private:
      /**
       * @brief Holds the value of p_methode
      */
      char p_methode;
      /**
       * @brief Holds if the value of p_methode is valid in the package
      */
      bool p_has_methode;

  };
  /**
   * @class KinectControllValues
   * @brief Generated class for network package KinectControllValues
   */
  class KinectControllValues : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(QVector3D p_positionoffset READ positionoffset WRITE set_positionoffset)
      Q_PROPERTY(bool p_has_positionoffset READ has_positionoffset WRITE set_has_positionoffset)
      Q_PROPERTY(QQuaternion p_rotationoffset READ rotationoffset WRITE set_rotationoffset)
      Q_PROPERTY(bool p_has_rotationoffset READ has_rotationoffset WRITE set_has_rotationoffset)


    public:
      /**
       * @brief Constructor for class KinectControllValues
       */
      explicit KinectControllValues() :
        PacketBase(12),
        p_has_positionoffset(false),
        p_has_rotationoffset(false)
      {}

      /**
       * @brief Requesting the value positionoffset
       * @return Returns the value
      */
      QVector3D positionoffset() { return p_positionoffset; }
      /**
       * @brief Set positionoffset in packet
      */
      void set_positionoffset(QVector3D value) { p_positionoffset = value; }
      /**
       * @brief Checks if positionoffset is valid/set in this package
       * @return Returns true, if positionoffset is valid
       */
      bool has_positionoffset() { return p_has_positionoffset; }
      /**
       * @brief Sets if positionoffset is valid in this package
       * @param value Value
      */
      void set_has_positionoffset(bool value) { p_has_positionoffset = value; }
      /**
       * @brief Requesting the value rotationoffset
       * @return Returns the value
      */
      QQuaternion rotationoffset() { return p_rotationoffset; }
      /**
       * @brief Set rotationoffset in packet
      */
      void set_rotationoffset(QQuaternion value) { p_rotationoffset = value; }
      /**
       * @brief Checks if rotationoffset is valid/set in this package
       * @return Returns true, if rotationoffset is valid
       */
      bool has_rotationoffset() { return p_has_rotationoffset; }
      /**
       * @brief Sets if rotationoffset is valid in this package
       * @param value Value
      */
      void set_has_rotationoffset(bool value) { p_has_rotationoffset = value; }


    private:
      /**
       * @brief Holds the value of p_positionoffset
      */
      QVector3D p_positionoffset;
      /**
       * @brief Holds if the value of p_positionoffset is valid in the package
      */
      bool p_has_positionoffset;
      /**
       * @brief Holds the value of p_rotationoffset
      */
      QQuaternion p_rotationoffset;
      /**
       * @brief Holds if the value of p_rotationoffset is valid in the package
      */
      bool p_has_rotationoffset;

  };
  /**
   * @class KinectObjectTracking
   * @brief Generated class for network package KinectObjectTracking
   */
  class KinectObjectTracking : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(QByteArray p_data READ data WRITE set_data)
      Q_PROPERTY(bool p_has_data READ has_data WRITE set_has_data)
      Q_PROPERTY(quint8 p_kinectid READ kinectid WRITE set_kinectid)
      Q_PROPERTY(bool p_has_kinectid READ has_kinectid WRITE set_has_kinectid)


    public:
      /**
       * @brief Constructor for class KinectObjectTracking
       */
      explicit KinectObjectTracking() :
        PacketBase(13),
        p_has_data(false),
        p_has_kinectid(false)
      {}

      /**
       * @brief Requesting the value data
       * @return Returns the value
      */
      QByteArray data() { return p_data; }
      /**
       * @brief Set data in packet
      */
      void set_data(QByteArray value) { p_data = value; }
      /**
       * @brief Checks if data is valid/set in this package
       * @return Returns true, if data is valid
       */
      bool has_data() { return p_has_data; }
      /**
       * @brief Sets if data is valid in this package
       * @param value Value
      */
      void set_has_data(bool value) { p_has_data = value; }
      /**
       * @brief Requesting the value kinectid
       * @return Returns the value
      */
      quint8 kinectid() { return p_kinectid; }
      /**
       * @brief Set kinectid in packet
      */
      void set_kinectid(quint8 value) { p_kinectid = value; }
      /**
       * @brief Checks if kinectid is valid/set in this package
       * @return Returns true, if kinectid is valid
       */
      bool has_kinectid() { return p_has_kinectid; }
      /**
       * @brief Sets if kinectid is valid in this package
       * @param value Value
      */
      void set_has_kinectid(bool value) { p_has_kinectid = value; }


    private:
      /**
       * @brief Holds the value of p_data
      */
      QByteArray p_data;
      /**
       * @brief Holds if the value of p_data is valid in the package
      */
      bool p_has_data;
      /**
       * @brief Holds the value of p_kinectid
      */
      quint8 p_kinectid;
      /**
       * @brief Holds if the value of p_kinectid is valid in the package
      */
      bool p_has_kinectid;

  };
  /**
   * @class KinectOpenCV
   * @brief Generated class for network package KinectOpenCV
   */
  class KinectOpenCV : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(QVector3D p_colormax READ colormax WRITE set_colormax)
      Q_PROPERTY(bool p_has_colormax READ has_colormax WRITE set_has_colormax)
      Q_PROPERTY(QVector3D p_colormin READ colormin WRITE set_colormin)
      Q_PROPERTY(bool p_has_colormin READ has_colormin WRITE set_has_colormin)
      Q_PROPERTY(quint8 p_delate READ delate WRITE set_delate)
      Q_PROPERTY(bool p_has_delate READ has_delate WRITE set_has_delate)
      Q_PROPERTY(quint8 p_erode READ erode WRITE set_erode)
      Q_PROPERTY(bool p_has_erode READ has_erode WRITE set_has_erode)
      Q_PROPERTY(bool p_showwindow READ showwindow WRITE set_showwindow)
      Q_PROPERTY(bool p_has_showwindow READ has_showwindow WRITE set_has_showwindow)


    public:
      /**
       * @brief Constructor for class KinectOpenCV
       */
      explicit KinectOpenCV() :
        PacketBase(14),
        p_has_colormax(false),
        p_has_colormin(false),
        p_has_delate(false),
        p_has_erode(false),
        p_has_showwindow(false)
      {}

      /**
       * @brief Requesting the value colormax
       * @return Returns the value
      */
      QVector3D colormax() { return p_colormax; }
      /**
       * @brief Set colormax in packet
      */
      void set_colormax(QVector3D value) { p_colormax = value; }
      /**
       * @brief Checks if colormax is valid/set in this package
       * @return Returns true, if colormax is valid
       */
      bool has_colormax() { return p_has_colormax; }
      /**
       * @brief Sets if colormax is valid in this package
       * @param value Value
      */
      void set_has_colormax(bool value) { p_has_colormax = value; }
      /**
       * @brief Requesting the value colormin
       * @return Returns the value
      */
      QVector3D colormin() { return p_colormin; }
      /**
       * @brief Set colormin in packet
      */
      void set_colormin(QVector3D value) { p_colormin = value; }
      /**
       * @brief Checks if colormin is valid/set in this package
       * @return Returns true, if colormin is valid
       */
      bool has_colormin() { return p_has_colormin; }
      /**
       * @brief Sets if colormin is valid in this package
       * @param value Value
      */
      void set_has_colormin(bool value) { p_has_colormin = value; }
      /**
       * @brief Requesting the value delate
       * @return Returns the value
      */
      quint8 delate() { return p_delate; }
      /**
       * @brief Set delate in packet
      */
      void set_delate(quint8 value) { p_delate = value; }
      /**
       * @brief Checks if delate is valid/set in this package
       * @return Returns true, if delate is valid
       */
      bool has_delate() { return p_has_delate; }
      /**
       * @brief Sets if delate is valid in this package
       * @param value Value
      */
      void set_has_delate(bool value) { p_has_delate = value; }
      /**
       * @brief Requesting the value erode
       * @return Returns the value
      */
      quint8 erode() { return p_erode; }
      /**
       * @brief Set erode in packet
      */
      void set_erode(quint8 value) { p_erode = value; }
      /**
       * @brief Checks if erode is valid/set in this package
       * @return Returns true, if erode is valid
       */
      bool has_erode() { return p_has_erode; }
      /**
       * @brief Sets if erode is valid in this package
       * @param value Value
      */
      void set_has_erode(bool value) { p_has_erode = value; }
      /**
       * @brief Requesting the value showwindow
       * @return Returns the value
      */
      bool showwindow() { return p_showwindow; }
      /**
       * @brief Set showwindow in packet
      */
      void set_showwindow(bool value) { p_showwindow = value; }
      /**
       * @brief Checks if showwindow is valid/set in this package
       * @return Returns true, if showwindow is valid
       */
      bool has_showwindow() { return p_has_showwindow; }
      /**
       * @brief Sets if showwindow is valid in this package
       * @param value Value
      */
      void set_has_showwindow(bool value) { p_has_showwindow = value; }


    private:
      /**
       * @brief Holds the value of p_colormax
      */
      QVector3D p_colormax;
      /**
       * @brief Holds if the value of p_colormax is valid in the package
      */
      bool p_has_colormax;
      /**
       * @brief Holds the value of p_colormin
      */
      QVector3D p_colormin;
      /**
       * @brief Holds if the value of p_colormin is valid in the package
      */
      bool p_has_colormin;
      /**
       * @brief Holds the value of p_delate
      */
      quint8 p_delate;
      /**
       * @brief Holds if the value of p_delate is valid in the package
      */
      bool p_has_delate;
      /**
       * @brief Holds the value of p_erode
      */
      quint8 p_erode;
      /**
       * @brief Holds if the value of p_erode is valid in the package
      */
      bool p_has_erode;
      /**
       * @brief Holds the value of p_showwindow
      */
      bool p_showwindow;
      /**
       * @brief Holds if the value of p_showwindow is valid in the package
      */
      bool p_has_showwindow;

  };
  /**
   * @class KinectRange
   * @brief Generated class for network package KinectRange
   */
  class KinectRange : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(QByteArray p_clients READ clients WRITE set_clients)
      Q_PROPERTY(bool p_has_clients READ has_clients WRITE set_has_clients)
      Q_PROPERTY(qreal p_maxrange READ maxrange WRITE set_maxrange)
      Q_PROPERTY(bool p_has_maxrange READ has_maxrange WRITE set_has_maxrange)
      Q_PROPERTY(qreal p_minrange READ minrange WRITE set_minrange)
      Q_PROPERTY(bool p_has_minrange READ has_minrange WRITE set_has_minrange)


    public:
      /**
       * @brief Constructor for class KinectRange
       */
      explicit KinectRange() :
        PacketBase(15),
        p_has_clients(false),
        p_has_maxrange(false),
        p_has_minrange(false)
      {}

      /**
       * @brief Requesting the value clients
       * @return Returns the value
      */
      QByteArray clients() { return p_clients; }
      /**
       * @brief Set clients in packet
      */
      void set_clients(QByteArray value) { p_clients = value; }
      /**
       * @brief Checks if clients is valid/set in this package
       * @return Returns true, if clients is valid
       */
      bool has_clients() { return p_has_clients; }
      /**
       * @brief Sets if clients is valid in this package
       * @param value Value
      */
      void set_has_clients(bool value) { p_has_clients = value; }
      /**
       * @brief Requesting the value maxrange
       * @return Returns the value
      */
      qreal maxrange() { return p_maxrange; }
      /**
       * @brief Set maxrange in packet
      */
      void set_maxrange(qreal value) { p_maxrange = value; }
      /**
       * @brief Checks if maxrange is valid/set in this package
       * @return Returns true, if maxrange is valid
       */
      bool has_maxrange() { return p_has_maxrange; }
      /**
       * @brief Sets if maxrange is valid in this package
       * @param value Value
      */
      void set_has_maxrange(bool value) { p_has_maxrange = value; }
      /**
       * @brief Requesting the value minrange
       * @return Returns the value
      */
      qreal minrange() { return p_minrange; }
      /**
       * @brief Set minrange in packet
      */
      void set_minrange(qreal value) { p_minrange = value; }
      /**
       * @brief Checks if minrange is valid/set in this package
       * @return Returns true, if minrange is valid
       */
      bool has_minrange() { return p_has_minrange; }
      /**
       * @brief Sets if minrange is valid in this package
       * @param value Value
      */
      void set_has_minrange(bool value) { p_has_minrange = value; }


    private:
      /**
       * @brief Holds the value of p_clients
      */
      QByteArray p_clients;
      /**
       * @brief Holds if the value of p_clients is valid in the package
      */
      bool p_has_clients;
      /**
       * @brief Holds the value of p_maxrange
      */
      qreal p_maxrange;
      /**
       * @brief Holds if the value of p_maxrange is valid in the package
      */
      bool p_has_maxrange;
      /**
       * @brief Holds the value of p_minrange
      */
      qreal p_minrange;
      /**
       * @brief Holds if the value of p_minrange is valid in the package
      */
      bool p_has_minrange;

  };
  /**
   * @class KinectTrackingData
   * @brief Generated class for network package KinectTrackingData
   */
  class KinectTrackingData : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(quint64 p_bodyid READ bodyid WRITE set_bodyid)
      Q_PROPERTY(bool p_has_bodyid READ has_bodyid WRITE set_has_bodyid)
      Q_PROPERTY(quint8 p_jointcount READ jointcount WRITE set_jointcount)
      Q_PROPERTY(bool p_has_jointcount READ has_jointcount WRITE set_has_jointcount)
      Q_PROPERTY(quint8 p_kinectid READ kinectid WRITE set_kinectid)
      Q_PROPERTY(bool p_has_kinectid READ has_kinectid WRITE set_has_kinectid)
      Q_PROPERTY(QByteArray p_trackingdata READ trackingdata WRITE set_trackingdata)
      Q_PROPERTY(bool p_has_trackingdata READ has_trackingdata WRITE set_has_trackingdata)


    public:
      /**
       * @brief Constructor for class KinectTrackingData
       */
      explicit KinectTrackingData() :
        PacketBase(16),
        p_has_bodyid(false),
        p_has_jointcount(false),
        p_has_kinectid(false),
        p_has_trackingdata(false)
      {}

      /**
       * @brief Requesting the value bodyid
       * @return Returns the value
      */
      quint64 bodyid() { return p_bodyid; }
      /**
       * @brief Set bodyid in packet
      */
      void set_bodyid(quint64 value) { p_bodyid = value; }
      /**
       * @brief Checks if bodyid is valid/set in this package
       * @return Returns true, if bodyid is valid
       */
      bool has_bodyid() { return p_has_bodyid; }
      /**
       * @brief Sets if bodyid is valid in this package
       * @param value Value
      */
      void set_has_bodyid(bool value) { p_has_bodyid = value; }
      /**
       * @brief Requesting the value jointcount
       * @return Returns the value
      */
      quint8 jointcount() { return p_jointcount; }
      /**
       * @brief Set jointcount in packet
      */
      void set_jointcount(quint8 value) { p_jointcount = value; }
      /**
       * @brief Checks if jointcount is valid/set in this package
       * @return Returns true, if jointcount is valid
       */
      bool has_jointcount() { return p_has_jointcount; }
      /**
       * @brief Sets if jointcount is valid in this package
       * @param value Value
      */
      void set_has_jointcount(bool value) { p_has_jointcount = value; }
      /**
       * @brief Requesting the value kinectid
       * @return Returns the value
      */
      quint8 kinectid() { return p_kinectid; }
      /**
       * @brief Set kinectid in packet
      */
      void set_kinectid(quint8 value) { p_kinectid = value; }
      /**
       * @brief Checks if kinectid is valid/set in this package
       * @return Returns true, if kinectid is valid
       */
      bool has_kinectid() { return p_has_kinectid; }
      /**
       * @brief Sets if kinectid is valid in this package
       * @param value Value
      */
      void set_has_kinectid(bool value) { p_has_kinectid = value; }
      /**
       * @brief Requesting the value trackingdata
       * @return Returns the value
      */
      QByteArray trackingdata() { return p_trackingdata; }
      /**
       * @brief Set trackingdata in packet
      */
      void set_trackingdata(QByteArray value) { p_trackingdata = value; }
      /**
       * @brief Checks if trackingdata is valid/set in this package
       * @return Returns true, if trackingdata is valid
       */
      bool has_trackingdata() { return p_has_trackingdata; }
      /**
       * @brief Sets if trackingdata is valid in this package
       * @param value Value
      */
      void set_has_trackingdata(bool value) { p_has_trackingdata = value; }


    private:
      /**
       * @brief Holds the value of p_bodyid
      */
      quint64 p_bodyid;
      /**
       * @brief Holds if the value of p_bodyid is valid in the package
      */
      bool p_has_bodyid;
      /**
       * @brief Holds the value of p_jointcount
      */
      quint8 p_jointcount;
      /**
       * @brief Holds if the value of p_jointcount is valid in the package
      */
      bool p_has_jointcount;
      /**
       * @brief Holds the value of p_kinectid
      */
      quint8 p_kinectid;
      /**
       * @brief Holds if the value of p_kinectid is valid in the package
      */
      bool p_has_kinectid;
      /**
       * @brief Holds the value of p_trackingdata
      */
      QByteArray p_trackingdata;
      /**
       * @brief Holds if the value of p_trackingdata is valid in the package
      */
      bool p_has_trackingdata;

  };
  /**
   * @class ReadRemoteSetting
   * @brief Generated class for network package ReadRemoteSetting
   */
  class ReadRemoteSetting : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(QString p_key READ key WRITE set_key)
      Q_PROPERTY(bool p_has_key READ has_key WRITE set_has_key)
      Q_PROPERTY(QVariant p_value READ value WRITE set_value)
      Q_PROPERTY(bool p_has_value READ has_value WRITE set_has_value)


    public:
      /**
       * @brief Constructor for class ReadRemoteSetting
       */
      explicit ReadRemoteSetting() :
        PacketBase(17),
        p_has_key(false),
        p_has_value(false)
      {}

      /**
       * @brief Requesting the value key
       * @return Returns the value
      */
      QString key() { return p_key; }
      /**
       * @brief Set key in packet
      */
      void set_key(QString value) { p_key = value; }
      /**
       * @brief Checks if key is valid/set in this package
       * @return Returns true, if key is valid
       */
      bool has_key() { return p_has_key; }
      /**
       * @brief Sets if key is valid in this package
       * @param value Value
      */
      void set_has_key(bool value) { p_has_key = value; }
      /**
       * @brief Requesting the value value
       * @return Returns the value
      */
      QVariant value() { return p_value; }
      /**
       * @brief Set value in packet
      */
      void set_value(QVariant value) { p_value = value; }
      /**
       * @brief Checks if value is valid/set in this package
       * @return Returns true, if value is valid
       */
      bool has_value() { return p_has_value; }
      /**
       * @brief Sets if value is valid in this package
       * @param value Value
      */
      void set_has_value(bool value) { p_has_value = value; }


    private:
      /**
       * @brief Holds the value of p_key
      */
      QString p_key;
      /**
       * @brief Holds if the value of p_key is valid in the package
      */
      bool p_has_key;
      /**
       * @brief Holds the value of p_value
      */
      QVariant p_value;
      /**
       * @brief Holds if the value of p_value is valid in the package
      */
      bool p_has_value;

  };
  /**
   * @class RobotControllValues
   * @brief Generated class for network package RobotControllValues
   */
  class RobotControllValues : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(qreal p_blending READ blending WRITE set_blending)
      Q_PROPERTY(bool p_has_blending READ has_blending WRITE set_has_blending)
      Q_PROPERTY(bool p_enablerobot READ enablerobot WRITE set_enablerobot)
      Q_PROPERTY(bool p_has_enablerobot READ has_enablerobot WRITE set_has_enablerobot)
      Q_PROPERTY(qreal p_maxspeed READ maxspeed WRITE set_maxspeed)
      Q_PROPERTY(bool p_has_maxspeed READ has_maxspeed WRITE set_has_maxspeed)
      Q_PROPERTY(qreal p_maxspeedup READ maxspeedup WRITE set_maxspeedup)
      Q_PROPERTY(bool p_has_maxspeedup READ has_maxspeedup WRITE set_has_maxspeedup)
      Q_PROPERTY(char p_robotoutputmode READ robotoutputmode WRITE set_robotoutputmode)
      Q_PROPERTY(bool p_has_robotoutputmode READ has_robotoutputmode WRITE set_has_robotoutputmode)


    public:
      /**
       * @brief Constructor for class RobotControllValues
       */
      explicit RobotControllValues() :
        PacketBase(18),
        p_has_blending(false),
        p_has_enablerobot(false),
        p_has_maxspeed(false),
        p_has_maxspeedup(false),
        p_has_robotoutputmode(false)
      {}

      /**
       * @brief Requesting the value blending
       * @return Returns the value
      */
      qreal blending() { return p_blending; }
      /**
       * @brief Set blending in packet
      */
      void set_blending(qreal value) { p_blending = value; }
      /**
       * @brief Checks if blending is valid/set in this package
       * @return Returns true, if blending is valid
       */
      bool has_blending() { return p_has_blending; }
      /**
       * @brief Sets if blending is valid in this package
       * @param value Value
      */
      void set_has_blending(bool value) { p_has_blending = value; }
      /**
       * @brief Requesting the value enablerobot
       * @return Returns the value
      */
      bool enablerobot() { return p_enablerobot; }
      /**
       * @brief Set enablerobot in packet
      */
      void set_enablerobot(bool value) { p_enablerobot = value; }
      /**
       * @brief Checks if enablerobot is valid/set in this package
       * @return Returns true, if enablerobot is valid
       */
      bool has_enablerobot() { return p_has_enablerobot; }
      /**
       * @brief Sets if enablerobot is valid in this package
       * @param value Value
      */
      void set_has_enablerobot(bool value) { p_has_enablerobot = value; }
      /**
       * @brief Requesting the value maxspeed
       * @return Returns the value
      */
      qreal maxspeed() { return p_maxspeed; }
      /**
       * @brief Set maxspeed in packet
      */
      void set_maxspeed(qreal value) { p_maxspeed = value; }
      /**
       * @brief Checks if maxspeed is valid/set in this package
       * @return Returns true, if maxspeed is valid
       */
      bool has_maxspeed() { return p_has_maxspeed; }
      /**
       * @brief Sets if maxspeed is valid in this package
       * @param value Value
      */
      void set_has_maxspeed(bool value) { p_has_maxspeed = value; }
      /**
       * @brief Requesting the value maxspeedup
       * @return Returns the value
      */
      qreal maxspeedup() { return p_maxspeedup; }
      /**
       * @brief Set maxspeedup in packet
      */
      void set_maxspeedup(qreal value) { p_maxspeedup = value; }
      /**
       * @brief Checks if maxspeedup is valid/set in this package
       * @return Returns true, if maxspeedup is valid
       */
      bool has_maxspeedup() { return p_has_maxspeedup; }
      /**
       * @brief Sets if maxspeedup is valid in this package
       * @param value Value
      */
      void set_has_maxspeedup(bool value) { p_has_maxspeedup = value; }
      /**
       * @brief Requesting the value robotoutputmode
       * @return Returns the value
      */
      char robotoutputmode() { return p_robotoutputmode; }
      /**
       * @brief Set robotoutputmode in packet
      */
      void set_robotoutputmode(char value) { p_robotoutputmode = value; }
      /**
       * @brief Checks if robotoutputmode is valid/set in this package
       * @return Returns true, if robotoutputmode is valid
       */
      bool has_robotoutputmode() { return p_has_robotoutputmode; }
      /**
       * @brief Sets if robotoutputmode is valid in this package
       * @param value Value
      */
      void set_has_robotoutputmode(bool value) { p_has_robotoutputmode = value; }


    private:
      /**
       * @brief Holds the value of p_blending
      */
      qreal p_blending;
      /**
       * @brief Holds if the value of p_blending is valid in the package
      */
      bool p_has_blending;
      /**
       * @brief Holds the value of p_enablerobot
      */
      bool p_enablerobot;
      /**
       * @brief Holds if the value of p_enablerobot is valid in the package
      */
      bool p_has_enablerobot;
      /**
       * @brief Holds the value of p_maxspeed
      */
      qreal p_maxspeed;
      /**
       * @brief Holds if the value of p_maxspeed is valid in the package
      */
      bool p_has_maxspeed;
      /**
       * @brief Holds the value of p_maxspeedup
      */
      qreal p_maxspeedup;
      /**
       * @brief Holds if the value of p_maxspeedup is valid in the package
      */
      bool p_has_maxspeedup;
      /**
       * @brief Holds the value of p_robotoutputmode
      */
      char p_robotoutputmode;
      /**
       * @brief Holds if the value of p_robotoutputmode is valid in the package
      */
      bool p_has_robotoutputmode;

  };
  /**
   * @class TriadicObjectData
   * @brief Generated class for network package TriadicObjectData
   */
  class TriadicObjectData : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(qreal p_closingvalue READ closingvalue WRITE set_closingvalue)
      Q_PROPERTY(bool p_has_closingvalue READ has_closingvalue WRITE set_has_closingvalue)
      Q_PROPERTY(quint8 p_ownership READ ownership WRITE set_ownership)
      Q_PROPERTY(bool p_has_ownership READ has_ownership WRITE set_has_ownership)
      Q_PROPERTY(qreal p_probability READ probability WRITE set_probability)
      Q_PROPERTY(bool p_has_probability READ has_probability WRITE set_has_probability)


    public:
      /**
       * @brief Constructor for class TriadicObjectData
       */
      explicit TriadicObjectData() :
        PacketBase(19),
        p_has_closingvalue(false),
        p_has_ownership(false),
        p_has_probability(false)
      {}

      /**
       * @brief Requesting the value closingvalue
       * @return Returns the value
      */
      qreal closingvalue() { return p_closingvalue; }
      /**
       * @brief Set closingvalue in packet
      */
      void set_closingvalue(qreal value) { p_closingvalue = value; }
      /**
       * @brief Checks if closingvalue is valid/set in this package
       * @return Returns true, if closingvalue is valid
       */
      bool has_closingvalue() { return p_has_closingvalue; }
      /**
       * @brief Sets if closingvalue is valid in this package
       * @param value Value
      */
      void set_has_closingvalue(bool value) { p_has_closingvalue = value; }
      /**
       * @brief Requesting the value ownership
       * @return Returns the value
      */
      quint8 ownership() { return p_ownership; }
      /**
       * @brief Set ownership in packet
      */
      void set_ownership(quint8 value) { p_ownership = value; }
      /**
       * @brief Checks if ownership is valid/set in this package
       * @return Returns true, if ownership is valid
       */
      bool has_ownership() { return p_has_ownership; }
      /**
       * @brief Sets if ownership is valid in this package
       * @param value Value
      */
      void set_has_ownership(bool value) { p_has_ownership = value; }
      /**
       * @brief Requesting the value probability
       * @return Returns the value
      */
      qreal probability() { return p_probability; }
      /**
       * @brief Set probability in packet
      */
      void set_probability(qreal value) { p_probability = value; }
      /**
       * @brief Checks if probability is valid/set in this package
       * @return Returns true, if probability is valid
       */
      bool has_probability() { return p_has_probability; }
      /**
       * @brief Sets if probability is valid in this package
       * @param value Value
      */
      void set_has_probability(bool value) { p_has_probability = value; }


    private:
      /**
       * @brief Holds the value of p_closingvalue
      */
      qreal p_closingvalue;
      /**
       * @brief Holds if the value of p_closingvalue is valid in the package
      */
      bool p_has_closingvalue;
      /**
       * @brief Holds the value of p_ownership
      */
      quint8 p_ownership;
      /**
       * @brief Holds if the value of p_ownership is valid in the package
      */
      bool p_has_ownership;
      /**
       * @brief Holds the value of p_probability
      */
      qreal p_probability;
      /**
       * @brief Holds if the value of p_probability is valid in the package
      */
      bool p_has_probability;

  };
  /**
   * @class UR5Connection
   * @brief Generated class for network package UR5Connection
   */
  class UR5Connection : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(bool p_connected READ connected WRITE set_connected)
      Q_PROPERTY(bool p_has_connected READ has_connected WRITE set_has_connected)
      Q_PROPERTY(QString p_robot_ip READ robot_ip WRITE set_robot_ip)
      Q_PROPERTY(bool p_has_robot_ip READ has_robot_ip WRITE set_has_robot_ip)
      Q_PROPERTY(quint16 p_robot_port READ robot_port WRITE set_robot_port)
      Q_PROPERTY(bool p_has_robot_port READ has_robot_port WRITE set_has_robot_port)


    public:
      /**
       * @brief Constructor for class UR5Connection
       */
      explicit UR5Connection() :
        PacketBase(20),
        p_has_connected(false),
        p_has_robot_ip(false),
        p_has_robot_port(false)
      {}

      /**
       * @brief Requesting the value connected
       * @return Returns the value
      */
      bool connected() { return p_connected; }
      /**
       * @brief Set connected in packet
      */
      void set_connected(bool value) { p_connected = value; }
      /**
       * @brief Checks if connected is valid/set in this package
       * @return Returns true, if connected is valid
       */
      bool has_connected() { return p_has_connected; }
      /**
       * @brief Sets if connected is valid in this package
       * @param value Value
      */
      void set_has_connected(bool value) { p_has_connected = value; }
      /**
       * @brief Requesting the value robot_ip
       * @return Returns the value
      */
      QString robot_ip() { return p_robot_ip; }
      /**
       * @brief Set robot_ip in packet
      */
      void set_robot_ip(QString value) { p_robot_ip = value; }
      /**
       * @brief Checks if robot_ip is valid/set in this package
       * @return Returns true, if robot_ip is valid
       */
      bool has_robot_ip() { return p_has_robot_ip; }
      /**
       * @brief Sets if robot_ip is valid in this package
       * @param value Value
      */
      void set_has_robot_ip(bool value) { p_has_robot_ip = value; }
      /**
       * @brief Requesting the value robot_port
       * @return Returns the value
      */
      quint16 robot_port() { return p_robot_port; }
      /**
       * @brief Set robot_port in packet
      */
      void set_robot_port(quint16 value) { p_robot_port = value; }
      /**
       * @brief Checks if robot_port is valid/set in this package
       * @return Returns true, if robot_port is valid
       */
      bool has_robot_port() { return p_has_robot_port; }
      /**
       * @brief Sets if robot_port is valid in this package
       * @param value Value
      */
      void set_has_robot_port(bool value) { p_has_robot_port = value; }


    private:
      /**
       * @brief Holds the value of p_connected
      */
      bool p_connected;
      /**
       * @brief Holds if the value of p_connected is valid in the package
      */
      bool p_has_connected;
      /**
       * @brief Holds the value of p_robot_ip
      */
      QString p_robot_ip;
      /**
       * @brief Holds if the value of p_robot_ip is valid in the package
      */
      bool p_has_robot_ip;
      /**
       * @brief Holds the value of p_robot_port
      */
      quint16 p_robot_port;
      /**
       * @brief Holds if the value of p_robot_port is valid in the package
      */
      bool p_has_robot_port;

  };
  /**
   * @class UR5Output
   * @brief Generated class for network package UR5Output
   */
  class UR5Output : public PacketBase
  {
      Q_OBJECT
      Q_PROPERTY(char p_outputtype READ outputtype WRITE set_outputtype)
      Q_PROPERTY(bool p_has_outputtype READ has_outputtype WRITE set_has_outputtype)


    public:
      /**
       * @brief Constructor for class UR5Output
       */
      explicit UR5Output() :
        PacketBase(21),
        p_has_outputtype(false)
      {}

      /**
       * @brief Requesting the value outputtype
       * @return Returns the value
      */
      char outputtype() { return p_outputtype; }
      /**
       * @brief Set outputtype in packet
      */
      void set_outputtype(char value) { p_outputtype = value; }
      /**
       * @brief Checks if outputtype is valid/set in this package
       * @return Returns true, if outputtype is valid
       */
      bool has_outputtype() { return p_has_outputtype; }
      /**
       * @brief Sets if outputtype is valid in this package
       * @param value Value
      */
      void set_has_outputtype(bool value) { p_has_outputtype = value; }


    private:
      /**
       * @brief Holds the value of p_outputtype
      */
      char p_outputtype;
      /**
       * @brief Holds if the value of p_outputtype is valid in the package
      */
      bool p_has_outputtype;

  };

}

#endif // IMODPROTOCOL_H
