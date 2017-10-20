#include "RobotPart.h"

#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>

RobotPart::RobotPart(osg::ref_ptr< osg::Group > _parent, std::string _fileName, osg::Vec3 _scale, osg::Vec3 _position){

    parent = new osg::Group();
	parent = _parent;

	scale = _scale;
	position = _position;

	transfRobotPart = new osg::PositionAttitudeTransform();


	osg::ref_ptr< osg::Node > nodeDB = (osgDB::readNodeFile(_fileName));
	transfRobotPart->addChild(nodeDB);
	transfRobotPart->setScale(scale);
	transfRobotPart->setPosition(position);


	//nodeDB->setCullingActive(false);
	if( !nodeDB.valid() )
	{
		osg::notify( osg::FATAL ) << "Can't find \"" << _fileName << "\". Make sure OSG_FILE_PATH includes the OSG sample data directory." << std::endl;
		exit( 0 );
	}
	
	
	nodeDB->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	osgUtil::Optimizer optimizer;
	optimizer.optimize(nodeDB.get(),osgUtil::Optimizer::DEFAULT_OPTIMIZATIONS);

	parent->addChild(transfRobotPart);
}

RobotPart::~RobotPart(){
	
}

osg::PositionAttitudeTransform *RobotPart::getPAT(void){
	return this->transfRobotPart;
}

void RobotPart::setOrientation(osg::Quat _orientation){
	transfRobotPart->setAttitude(_orientation);
}

osg::Vec3 RobotPart::getPositionOfPart(void){
	osg::Vec3 pos;
	
	return pos;

}
osg::Vec3 RobotPart::getOrientationOfPart(void){
	osg::Vec3 o;
	
	return o;

}
