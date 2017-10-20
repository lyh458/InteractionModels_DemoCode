#include "imodcontrol.h"
#include <QtCore/QThread>
#include <QtCore/QDebug>

#ifndef WIN64
extern "C"
{
  iModControl* controller;
  QThread* runthread;

  IMODCONTROLLSHARED_EXPORT void DC_InitFunc(void) __attribute__ ((constructor));
  IMODCONTROLLSHARED_EXPORT void DC_TermFunc(void) __attribute__ ((destructor));


  void DC_InitFunc(void)
  {
    // Attention: When loading this library with the original iModView, this is called too! -> Dont initialize anything here!
    controller = NULL;
    runthread = NULL;
  }

  void DC_TermFunc(void)
  {
    if(controller)
    {
      qDebug() << "Terminating iModSkynet";
      runthread->terminate();
      delete runthread; // This will delete the controller too
    }
  }

  IMODCONTROLLSHARED_EXPORT void DC_LoadLibrary()
  {
    // The controller requires an eventloop.
    // Because the C-Interface ist started with Blender, there is no eventloop
    // -> Create a Thread here and move the controller to the new thread

    qDebug() << "Loading iMod CInterface in thread" << QThread::currentThreadId();

    runthread  = new QThread();
    controller = new iModControl(true);

    controller->connect(runthread, &QThread::destroyed, controller, &iModControl::deleteLater);
    controller->connect(runthread, &QThread::started, controller, &iModControl::slotCInterfaceStartup);

    controller->moveToThread(runthread);
    runthread->start();
  }

  IMODCONTROLLSHARED_EXPORT void DC_SendData(char oc, char* data, int length)
  {
    QByteArray qdata = QByteArray(data, length);
    controller->sendBlenderCommunication(oc, qdata);
  }

  IMODCONTROLLSHARED_EXPORT void DC_getGripperState(char* state)
  {
    *state = controller->getGripperState();
  }

  IMODCONTROLLSHARED_EXPORT void DC_setGripperValues(unsigned char os, double pb, double cv)
  {
    controller->sendTriadicObjectData(os, pb, cv);
  }

  IMODCONTROLLSHARED_EXPORT void DC_getRobotTCP(float* x, float* y, float* z)
  {
    QVector3D tcp = controller->getLastRobotTCP();
    *x = tcp.x();
    *y = tcp.y();
    *z = tcp.z();
  }

  IMODCONTROLLSHARED_EXPORT void DC_getFTSensorValues(float* fx, float* fy, float* fz, float* tx, float* ty, float* tz)
  {
    QVector3D f = controller->getLastFTForce();
    QVector3D t = controller->getLastFTTorque();
    *fx = f.x();
    *fy = f.y();
    *fz = f.z();
    *tx = t.x();
    *ty = t.y();
    *tz = t.z();
  }

}
#endif
