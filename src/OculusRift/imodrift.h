#ifndef IMODRIFT_H
#define IMODRIFT_H

#include "imodrift_global.h"

#include <QtCore/QObject>
#include <QtCore/QEventLoop>
#include <QtGui/QVector3D>

#include <osgViewer/Viewer>

class iModRiftNetwork;
class QThread;
class OculusDevice;
class OculusViewer;
class iModRift : public QObject
{
  Q_OBJECT
  public:
    explicit iModRift(osgViewer::Viewer* _viewer, osg::ref_ptr<osg::Group> _root, QObject* parent = 0);
    ~iModRift();

    void updateRift(QVector3D pos);
    void initRift();
    void useArtTracking(bool tmp = true);
    bool artTracking();

  public slots:

  signals:

  private:
    osg::ref_ptr<osgViewer::Viewer> viewer;
    osg::ref_ptr<osg::Group> root;
    osg::ref_ptr<OculusDevice> oculusDevice;
    osg::ref_ptr<OculusViewer> oculusViewer;
    
    iModRiftNetwork* network;
    QEventLoop* eventloop;
    bool usearttracking;
};

#endif // IMODRIFT_H
