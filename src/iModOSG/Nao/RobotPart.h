#include <string>

#include <osg/Group>
#include <osg/PositionAttitudeTransform>

class RobotPart {
	
public:
	RobotPart(osg::ref_ptr< osg::Group > _parent, const std::string _fileName, osg::Vec3 _scale, osg::Vec3 _position);
	~RobotPart();
	osg::PositionAttitudeTransform *getPAT(void);
	void setOrientation(osg::Quat _orientation);
	
	osg::Vec3 getPositionOfPart(void);
	osg::Vec3 getOrientationOfPart(void);



protected:

	osg::Vec3 scale;
	osg::ref_ptr< osg::PositionAttitudeTransform> transfRobotPart;
	osg::ref_ptr< osg::Group > parent;
	osg::Vec3 position;


};
