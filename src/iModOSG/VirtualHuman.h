/*
 * VirtualHuman.h
 *
 *  Created on: Oct 30, 2013
 *     Authors: ddvogt, bjung
 */

#ifndef VIRTUALHUMAN_H_
#define VIRTUALHUMAN_H_

#include <osg/Array>
#include <osg/Geometry>
#include <osg/Group>
#include <osgAnimation/Animation>
#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/Bone>
#include <osgAnimation/UpdateBone>
#include <osgAnimation/StackedQuaternionElement>
#include <osgAnimation/StackedRotateAxisElement>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/StackedMatrixElement>
#include <osgAnimation/Skeleton>
#include <osgDB/ReadFile>
#include <osgDB/Options>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>

#include <iostream>
#include <vector>
#include <map>


class VirtualHuman {

	//!A virtual humans current configuration
	osg::Matrixf _rootMatrix;
	std::map<std::string, osg::Quat> _currentRotations;

	//!Available animation
	osgAnimation::Animation* _currentRotationAnimation;
	std::map<std::string,osgAnimation::Animation*> _currentAnimation;

	std::map<std::string, osgAnimation::Bone*> _bonemap;

	osg::ref_ptr<osgAnimation::BasicAnimationManager> _manager;


protected:
	//!The model of the virtual human, e.g. its shape
	osg::ref_ptr<osg::Node> _model;


public:
	VirtualHuman();
	virtual ~VirtualHuman();

	/*!
	 * Load a virtual humans model.
	 * */
	void load(std::string fileNameAndPath);

	/*!
	 * Set a virtual humans posture
	 * */
	void setPosture(const std::vector<std::string> &boneNames,
					const std::vector<osg::Quat> &rotations,
					std::vector<double> &times);

	/*!
     * Set a virtual humans root transformation over time
	 * */
    void setRootTransformation(osg::Quat &quat, osg::Vec3 &position, double &duration);

	/*!
	 * Setup the virtual human, e.g. callbacks
	 * TODO: consider private
	 * */
     void setup(osg::ref_ptr<osg::Group> root, osg::Vec3 defaultPos, osg::Quat defaultRot);


	 /*!
	  *  remove light source
	  */
	 void removeLightSource();


private:

	/*!
	 * Update a given animation
	 * @param anim The animation which will be added
	 * @param name The name of the channel which will be used. name="translate" or name="rotate" are implemented
	 * */
	void updateAnimation(osgAnimation::Animation *anim, std::string name);


};


#endif /* VIRTUALHUMAN_H_ */
