#include "RobotPart.h"

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/StdVector>

class Robot {
	enum ConstraintType {coneTwistConstraint=1};	

public:
	Robot(osg::ref_ptr< osg::Group > _root, osg::Vec3 _scale,osg::Vec3 _position);
	~Robot();
	void init();
	
	void setScale(osg::Vec3 _scale);
	void updateJointAngles(std::vector<Eigen::Quaterniond,Eigen::aligned_allocator<Eigen::Quaterniond>> *_jointAngles);

	RobotPart *chest;
	RobotPart *neck;
	RobotPart *head;

	RobotPart *lShoulderPadMobile;
	RobotPart *lArm;
	RobotPart *lForeArm;
	RobotPart *lHand;

	RobotPart *lHip;
	RobotPart *lUpperThighHinge;
	RobotPart *lUpperThigh;
	RobotPart *lShinbone;
	RobotPart *lAnkle;
	RobotPart *lFoot;

	RobotPart *rShoulderPadMobile;
	RobotPart *rArm;
	RobotPart *rForeArm;
	RobotPart *rHand;

	RobotPart *rHip;
	RobotPart *rUpperThighHinge;
	RobotPart *rUpperThigh;
	RobotPart *rShinbone;
	RobotPart *rAnkle;
	RobotPart *rFoot;

	osg::ref_ptr<osg::PositionAttitudeTransform> transfRobot;
	osg::ref_ptr< osg::Group > root;
	
	osg::Vec3 scale;
	osg::Vec3 position;
};
