#include "Robot.h"

// scale factor for Nao shape files
#define SCALE_FACTOR 0.001

Robot::Robot(osg::ref_ptr< osg::Group > _root,osg::Vec3 _scale,osg::Vec3 _position){
	position = _position;
	scale = _scale;
	transfRobot = new osg::PositionAttitudeTransform();
	root = new osg::Group();
	root = _root;
	root->addChild(transfRobot);
	transfRobot->setPosition(position);
}

Robot::~Robot(){

	delete(chest);
	delete(neck);
	delete(head);

	delete(lShoulderPadMobile);
	delete(lArm);
	delete(lForeArm);
	delete(lHand);

	delete(lHip);
	delete(lUpperThighHinge);
	delete(lUpperThigh);
	delete(lShinbone);
	delete(lAnkle);
	delete(lFoot);

	delete(rShoulderPadMobile);
	delete(rArm);
	delete(rForeArm);
	delete(rHand);

	delete(rHip);
	delete(rUpperThighHinge);
	delete(rUpperThigh);
	delete(rShinbone);
	delete(rAnkle);
	delete(rFoot);

}



void Robot::init(){

	// robot parts

    chest = new RobotPart(transfRobot, "Nao/chest.3ds", scale, osg::Vec3(0,0,0)*SCALE_FACTOR);
    neck = new RobotPart(chest->getPAT(), "Nao/neck.3ds", scale, osg::Vec3(0,0,100)*SCALE_FACTOR);
    head = new RobotPart(neck->getPAT(), "Nao/head.3ds", scale, osg::Vec3(0,0,0)*SCALE_FACTOR);

    lShoulderPadMobile = new RobotPart(chest->getPAT(), "Nao/lShoulderPadMobile.3ds", scale, osg::Vec3 (90,-15,70)*SCALE_FACTOR);
    lArm = new RobotPart(lShoulderPadMobile->getPAT(), "Nao/lArm.3ds", scale, osg::Vec3(10,0,0)*SCALE_FACTOR);
    lForeArm = new RobotPart(lArm->getPAT(), "Nao/lForeArm.3ds", scale, osg::Vec3(10,-100,0)*SCALE_FACTOR);
    lHand = new RobotPart(lForeArm->getPAT(), "Nao/lHand.3ds",scale, osg::Vec3(0,-50,0)*SCALE_FACTOR);

    lHip = new RobotPart(chest->getPAT(), "Nao/lHip.3ds", scale, osg::Vec3 (50,-30,-110)*SCALE_FACTOR);
    lUpperThighHinge = new RobotPart(lHip->getPAT(), "Nao/lUpperThighHinge.3ds", scale, osg::Vec3(0,0,0)*SCALE_FACTOR);
    lUpperThigh = new RobotPart(lUpperThighHinge->getPAT(), "Nao/lUpperThigh.3ds", scale, osg::Vec3(0,0,0)*SCALE_FACTOR);
    lShinbone = new RobotPart(lUpperThigh->getPAT(), "Nao/lShinebone.3ds", scale, osg::Vec3(0,0,-100)*SCALE_FACTOR);
    lAnkle = new RobotPart(lShinbone->getPAT(), "Nao/lAnkle.3ds",scale, osg::Vec3(0,0,-100)*SCALE_FACTOR);
    lFoot = new RobotPart(lAnkle->getPAT(), "Nao/lFoot.3ds", scale, osg::Vec3(0,0,0)*SCALE_FACTOR);

    rShoulderPadMobile = new RobotPart(chest->getPAT(), "Nao/rShoulderPadMobile.3ds", scale, osg::Vec3(-90,-15,70)*SCALE_FACTOR);
    rArm = new RobotPart(rShoulderPadMobile->getPAT(), "Nao/rArm.3ds", scale, osg::Vec3(-10,0,0)*SCALE_FACTOR);
    rForeArm = new RobotPart(rArm->getPAT(), "Nao/rForeArm.3ds",scale, osg::Vec3(-10,-100,0)*SCALE_FACTOR);
    rHand = new RobotPart(rForeArm->getPAT(), "Nao/rHand.3ds", scale, osg::Vec3(0,-50,0)*SCALE_FACTOR);

    rHip = new RobotPart(chest->getPAT(), "Nao/rHip.3ds",  scale, osg::Vec3(-50,-30,-110)*SCALE_FACTOR);
    rUpperThighHinge = new RobotPart(rHip->getPAT(), "Nao/rUpperThighHinge.3ds",scale, osg::Vec3(0,0,0)*SCALE_FACTOR);
    rUpperThigh = new RobotPart(rUpperThighHinge->getPAT(), "Nao/rUpperThigh.3ds", scale, osg::Vec3(0,0,0)*SCALE_FACTOR);
    rShinbone = new RobotPart(rUpperThigh->getPAT(), "Nao/rShinebone.3ds", scale, osg::Vec3(0,0,-100)*SCALE_FACTOR);
    rAnkle = new RobotPart(rShinbone->getPAT(), "Nao/rAnkle.3ds",  scale, osg::Vec3(0,0,-100)*SCALE_FACTOR);
    rFoot = new RobotPart(rAnkle->getPAT(), "Nao/rFoot.3ds",  scale, osg::Vec3(0,0,0)*SCALE_FACTOR);

}

void Robot::updateJointAngles(std::vector<Eigen::Quaterniond,Eigen::aligned_allocator<Eigen::Quaterniond>> *_jointAngles){
	lShoulderPadMobile->setOrientation(osg::Quat(_jointAngles[4].data()->x(),_jointAngles[4].data()->y(),_jointAngles[4].data()->z(),_jointAngles[4].data()->w()));
}
