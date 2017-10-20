#include "imodrift.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QThread>

#include "OSGViewer/oculusviewer.h"
#include "OSGViewer/oculusdevice.h"
#include "OSGViewer/oculuseventhandler.h"
#include <osgGA/TrackballManipulator>

#include "imodriftnetwork.h"

iModRift::iModRift(osgViewer::Viewer* _viewer, osg::ref_ptr<osg::Group> _root, QObject *parent) :
    QObject(parent),
    viewer(_viewer),
    root(_root),
    oculusDevice(NULL),
    network(NULL),
    oculusViewer(NULL)
{
    eventloop = new QEventLoop(this);
}

iModRift::~iModRift()
{
	if(network)
		delete network;
    eventloop->quit();
    delete eventloop;
}

void iModRift::updateRift(QVector3D pos)
{
    eventloop->processEvents();
    if(oculusDevice && oculusViewer && network)// && usearttracking)
    {
	   QVector3D qtposition = network->getARTPosition();
	   
       oculusDevice->updateHMDPosition(qtposition);
       //oculusDevice->updateHMDPosition(pos);
       oculusDevice->updateHMDRotation(network->getARTRotation());
       
       osg::Vec3 position(qtposition.x(), qtposition.y(), qtposition.z());
                 
       osgGA::TrackballManipulator* tbm = dynamic_cast<osgGA::TrackballManipulator*>(viewer->getCameraManipulator());
       if(tbm)
       {
           tbm->setTransformation(position, osg::Quat( osg::PI_2, osg::Vec3(1,0,0)));
           tbm->home(0);
	   }
	   else
	   {
		   qDebug() << "Failed to use CameraManipulator!";
	   }
    }
}

void iModRift::initRift()
{
    float nearClip = 0.01f;
    float farClip = 10000.0f;
    float pixelsPerDisplayPixel = 1.0;
    bool useTimewarp = true;
    
    oculusDevice = new OculusDevice(nearClip, farClip, pixelsPerDisplayPixel, useTimewarp);
    network = new iModRiftNetwork(oculusDevice);
    network->start();

    viewer->getCameraManipulator()->setHomePosition(osg::Vec3(0,0,0),osg::Vec3(1,0,0),osg::Vec3(0,0,1));
    //viewer->getCameraManipulator()->home(0);
    // Get the suggested context traits
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = oculusDevice->graphicsContextTraits();

    // Create a graphic context based on our desired traits
    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
    if (!gc) {
        osg::notify(osg::NOTICE) << "Error, GraphicsWindow has not been created successfully" << std::endl;
        return;
    }

    if (gc.valid()) {
        gc->setClearColor(osg::Vec4(0.2f, 0.2f, 0.4f, 1.0f));
        gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    viewer->getCamera()->setGraphicsContext(gc);
    viewer->getCamera()->setViewport(0, 0, traits->width, traits->height);
    //viewer->getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );

    viewer->addEventHandler(new OculusEventHandler(oculusDevice));

    
    oculusViewer = new OculusViewer(viewer, oculusDevice);
    viewer->setSceneData(oculusViewer);
    oculusViewer->addChild(root.get());
    oculusViewer->setName("OculusViewer");
}

void iModRift::useArtTracking(bool tmp)
{
	if(oculusDevice)
		oculusDevice->useArtTracking(tmp);
	
}

bool iModRift::artTracking()
{
	if(oculusDevice)
		return oculusDevice->artTracking();
	else
	    return false;
}
