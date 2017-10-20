#ifndef IMODGRIPPER_H
#define IMODGRIPPER_H

#include <QObject>
#include <QtGui/QVector3D>

#ifdef WIN64
typedef struct __declspec( align( 8 ) ) _rgo
#else
typedef struct __attribute__ ((aligned (8))) _rgo
#endif
{
    quint8 rACT;
    quint8 rMOD;
    quint8 rGTO;
    quint8 rATR;
    quint8 rGLV;
    quint8 rICF;
    quint8 rICS;
    quint8 rPRA;
    quint8 rSPA;
    quint8 rFRA;
    quint8 rPRB;
    quint8 rSPB;
    quint8 rFRB;
    quint8 rPRC;
    quint8 rSPC;
    quint8 rFRC;
    quint8 rPRS;
    quint8 rSPS;
    quint8 rFRS;
} RobotGripperOutput;


#ifdef WIN64
typedef struct __declspec( align( 8 ) ) _rgi
#else
typedef struct __attribute__ ((aligned (8))) _rgi
#endif
{
    quint8 gACT;
    quint8 gMOD;
    quint8 gGTO;
    quint8 gIMC;
    quint8 gSTA;
    quint8 gDTA;
    quint8 gDTB;
    quint8 gDTC;
    quint8 gDTS;
    quint8 gFLT;
    quint8 gPRA;
    quint8 gPOA;
    quint8 gCUA;
    quint8 gPRB;
    quint8 gPOB;
    quint8 gCUB;
    quint8 gPRC;
    quint8 gPOC;
    quint8 gCUC;
    quint8 gPRS;
    quint8 gPOS;
    quint8 gCUS;
} RobotGripperInput;

class QTcpSocket;
class QTimer;
class iModGripper : public QObject
{
    Q_OBJECT
  public:
    explicit iModGripper(QObject *parent = 0);
    quint8 getInitializationStatus()          { return input.gACT; }
    quint8 getOperationModeStatus()           { return input.gMOD; }
    quint8 getActionStatus()                  { return input.gGTO; }
    quint8 getGripperStatus()                 { return input.gIMC; }
    quint8 getMotionStatus()                  { return input.gSTA; }
    quint8 getFingerAObjectDetectionStatus()  { return input.gDTA; }
    quint8 getFingerBObjectDetectionStatus()  { return input.gDTB; }
    quint8 getFingerCObjectDetectionStatus()  { return input.gDTC; }
    quint8 getScissorObjectDetectionStatus()  { return input.gDTS; }
    quint8 getFaultStatus()                   { return input.gFLT; }
    quint8 getFingerARequestedPosition()      { return input.gPRA; }
    quint8 getFingerBRequestedPosition()      { return input.gPRB; }
    quint8 getFingerCRequestedPosition()      { return input.gPRC; }
    quint8 getScissorRequestedPosition()      { return input.gPRS; }
    quint8 getFingerAPosition()               { return input.gPOA; }
    quint8 getFingerBPosition()               { return input.gPOB; }
    quint8 getFingerCPosition()               { return input.gPOC; }
    quint8 getScissorPosition()               { return input.gPOS; }
    qreal getFingerACurrentConsumption()      { return input.gCUA * 0.1; }
    qreal getFingerBCurrentConsumption()      { return input.gCUB * 0.1; }
    qreal getFingerCCurrentConsumption()      { return input.gCUC * 0.1; }
    qreal getScissorCurrentConsumption()      { return input.gCUS * 0.1; }

    void setActivateGripper(bool value)           { output.rACT = value?0x01:0x00; }
    void setGraspingMode(quint8 value)            { output.rMOD = value; }
    void setGoTo(bool value)                      { output.rGTO = value?0x01:0x00; }
    void setAutomaticRelease(bool value)          { output.rATR = value?0x01:0x00; }
    void setGloveMode(bool value)                 { output.rGLV = value?0x01:0x00; }
    void setIndividualFingerControl(bool value)   { output.rICF = value?0x01:0x00; }
    void setIndividualScissorControl(bool value)  { output.rICS = value?0x01:0x00; }
    void setFingerAPosition(quint8 value)         { output.rPRA = value; }
    void setFingerBPosition(quint8 value)         { output.rPRB = value; }
    void setFingerCPosition(quint8 value)         { output.rPRC = value; }
    void setScissorPosition(quint8 value)         { output.rPRS = value; }
    void setFingerASpeed(quint8 value)            { output.rSPA = value; }
    void setFingerBSpeed(quint8 value)            { output.rSPB = value; }
    void setFingerCSpeed(quint8 value)            { output.rSPC = value; }
    void setScissorSpeed(quint8 value)            { output.rSPS = value; }
    void setFingerAForce(quint8 value)            { output.rFRA = value; }
    void setFingerBForce(quint8 value)            { output.rFRB = value; }
    void setFingerCForce(quint8 value)            { output.rFRC = value; }
    void setScissorForce(quint8 value)            { output.rFRS = value; }

    void resetLocalOutputRegister();
    void setValuesOnGripper();
    void disconnectGripper();

  signals:
    void signalGripperStatus(QByteArray);
    void signalGripperState(char);

  public slots:
    void slotSetGripperStatus(QByteArray status);
    bool slotConnectGripper(QString ip);
    void slotAutomaticGripperDriverMatlab(quint8 os, qreal pb, qreal cv);
    void slotAutomaticGripperDriverFTSensor(QVector3D force, QVector3D torque);
    void slotGripperAutomatic(quint8 type);

  private slots:
    void slotGripperStatusIncomming();
    void slotUpdateTimer();
    void slotGripperDisconnected();
    void slotDBG_Timer();

  private:
    void getAllValuesFromGripper();
    QByteArray serializeGripperCommand();
    void deserializeGripperStatus(QByteArray data);
    void resetLocalInputRegister();
    QByteArray getMBAPHeader(quint16 size);
    QByteArray getRobotiqHeader(quint8 fcode, quint16 first_reg, quint16 wordcount);
    QByteArray readFromSocket(qint32 size);
    void writeToSocket(QByteArray data);

    template <typename type>
    void swap_endian(type& d)
    {
      char& raw = reinterpret_cast<char&>(d);
      std::reverse(&raw, &raw + sizeof(type));
    }
    template<typename type>
    type readType()
    {
      type value;
      QByteArray buffer = readFromSocket(sizeof(type));
      memcpy(&value, buffer.data(), sizeof(type));
      swap_endian(value);
      return value;
    }

    RobotGripperInput input;
    RobotGripperOutput output;
    quint16 transactionidentifier;
    QTcpSocket* grippersocket;
    const quint8 clientidentifier = 0x02;
    const quint8 functioncode_get = 0x04;
    const quint8 functioncode_set = 0x10;
    QTimer* updatetimer;
    quint8 controlltype;

    quint32 dbg_counter;
    QTimer* dbg_timer;
};

#endif // IMODGRIPPER_H
