#include "imodsatellite.h"

#include "imodcontrol.h"

iModSatellite::iModSatellite(QObject *parent) :
  QObject(parent),
  clientid(-1)
{
  controller = new iModControl(this);
  connect(controller, &iModControl::signalDaemonControll, this, &iModSatellite::slotDaemonControll);
}

iModSatellite::~iModSatellite()
{
  delete controller;
}

void iModSatellite::connectDemon()
{
  controller->connectDaemon();
}

void iModSatellite::startKinect()
{
  controller->startKinect(clientid);
}

void iModSatellite::slotDaemonControll(quint8 rc, quint8 rk)
{
  if(clientid == -1)
  {
    clientid = rc;
    startKinect();
  }
  clientid = rc;
  rootkinect = rk;

}

