#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>
#include <osg/Switch>

#include "CaveAndLocalWrapper/CaveAndLocalWrapper.h"
#include "MyInteractionMeshController.h"

#ifdef IMOD_RIFT
	#include <imodrift.h>
	#include <QtCore/QCoreApplication>
#endif


int main(){

    CaveAndLocalWrapper * dummy = new CaveAndLocalWrapper();

    dummy->init();

    osgViewer::Viewer viewer;
    viewer.setUpViewInWindow(640,480,640,480);
    viewer.setSceneData(dummy->_root.get());
    viewer.setCameraManipulator(new osgGA::TrackballManipulator);
    viewer.getCamera()->setClearColor(osg::Vec4(1,1,1,1)); 

//    osg::Vec3d eye;
//    osg::Vec3d center;
//    osg::Vec3d up;

//    viewer.getCameraManipulator()->setHomePosition(osg::Vec3(1.64,-9.13,3.46),osg::Vec3(1.42,-8.2,3.16),osg::Vec3(-0.05,0.3,1));
    
    viewer.getCameraManipulator()->setHomePosition(osg::Vec3(0,-9.0,3.46),osg::Vec3(0,-1.5,1.0),osg::Vec3(-0.05,0.3,1));
    viewer.getCameraManipulator()->home(0);

    viewer.addEventHandler(new MyInteractionMesh::MyInteractionMeshKeyboardEventHandler(dummy->_meshController));
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);
    // viewer.addEventHandler(new osgViewer::ThreadingHandler);
    viewer.addEventHandler(new osgViewer::HelpHandler);
    // viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));
    // viewer.addEventHandler(new osgViewer::ScreenCaptureHandler);

#ifdef IMOD_RIFT
    iModRift* rift = new iModRift(&viewer, dummy->_root);
    rift->initRift();
    rift->useArtTracking(true);
#endif

    while(!viewer.done()){
        //usleep(100000);
        dummy->simulate();
        //dummy->update();

#ifdef IMOD_RIFT
        osg::Vec3 position = dummy->getHeadPosition();
        rift->updateRift(QVector3D(position[0], position[1], position[2]));
#endif

//        viewer.getCamera()->getViewMatrixAsLookAt(eye,center,up);

//        std::cout<<"Eye: "<<eye.x()<<","<<eye.y()<<","<<eye.z()<<std::endl;
//        std::cout<<"Cen: "<<center.x()<<","<<center.y()<<","<<center.z()<<std::endl;
//        std::cout<<"Up : "<<up.x()<<","<<up.y()<<","<<up.z()<<std::endl;

        viewer.frame();
    }

    return 0;
}
